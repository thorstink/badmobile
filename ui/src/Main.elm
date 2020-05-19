port module Main exposing (main)

import Browser
import Html exposing (Html,Attribute, div, li, p, text, ul, input, button)
import Html.Attributes exposing (..)
import Html.Events exposing (onInput,onClick)
import Keyboard exposing (Key(..))
import Style
import Teleop exposing (..)
import RobotConfig exposing (..)
-- import Robotconfig exposing (..)
import Json.Decode as D
import Json.Encode as E
import ImuViz 
import Imu exposing (ImuData, decodeImuData)

port websocketCmdVelIn : (String -> msg) -> Sub msg
port websocketCmdVelOut : String -> Cmd msg
port websocketSettingsOut : String -> Cmd msg
port websocketSettingsIn : (String -> msg) -> Sub msg
port websocketImuIn : (String -> msg) -> Sub msg

type alias UpdateFields =
    { name : String
    , fs : String
    , fc : String
    }

{- MODEL -}

type alias Model =
    {
      pressedKeys : List Key
    , twist : TwistData
    , action : Action
    , responses : String
    , imu_values : String
    , input : String
    , t : Int
    , lastImu : ImuData
    , imuDatas : List ImuData
    , robotConfig : Maybe RobotConfigRobot
    , fields : UpdateFields
    }


init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = TwistData 1.0 1.0
      , action = DontPublish
      , responses = ""
      , imu_values = "hi2"
      , input = ""
      , t = 0
      , lastImu = ImuData 0 0.0 0.0 0.0 0.0 0.0 0.0
      , imuDatas = []
      , robotConfig = Nothing
      , fields = UpdateFields "" "" ""
      }
    , Cmd.none
    )

{- UPDATE -}

type Msg
  = KeyboardMsg Keyboard.Msg
  | WebsocketCmdVelIn String 
  | WebsocketImuIn String 
  | WebsocketSettingsIn String 
  | ChangeName String
  | ChangeImuFs String
  | ChangeImuFc String
  | SendUpdatesToRobot
  | ResetUpdateFields

update : Msg -> Model -> ( Model, Cmd Msg )
update msg model =
    case msg of
        KeyboardMsg keyMsg ->
          ( 
            let
              pressedKeys = Keyboard.update keyMsg model.pressedKeys
            in
            { 
              model
              | pressedKeys = pressedKeys,
                twist = 
                    case List.head (List.map (\key -> throttleMapper model.twist (keyMapper key)) pressedKeys) of
                      Just (cmd, update_state)  -> 
                        (
                          case update_state of 
                            DontPublish -> cmd
                            Publish     -> model.twist
                        )
                      _                 -> model.twist
            }
            , 
              case List.head (List.map (\key -> throttleMapper model.twist (keyMapper key)) (Keyboard.update keyMsg model.pressedKeys)) of
                                    Just (twist, action) -> 
                                      (
                                        case action of 
                                          Publish -> websocketCmdVelOut (E.encode 0 (encode twist))
                                          DontPublish -> Cmd.none
                                      )
                                    _ -> Cmd.none
              
          )
        WebsocketCmdVelIn value ->
          ( { model | responses = value }
          , Cmd.none
          )
        WebsocketImuIn value ->
          ( { model | imu_values = value
                    , t = model.t + 1
                    , imuDatas = 
                        let 
                          new_sample = case (D.decodeString decodeImuData model.imu_values) of
                            Ok imu_msg -> imu_msg
                            _ -> ImuData 0 0.0 0.0 0.0 0.0 0.0 0.0
                        in
                          case List.tail model.imuDatas of
                            Just tail ->  ( if List.length model.imuDatas > 50 then
                                              tail ++ [new_sample]
                                            else
                                              model.imuDatas ++ [new_sample]
                                          )
                            _         -> model.imuDatas ++ [new_sample]
            }
          , Cmd.none
          )
        WebsocketSettingsIn value ->
          ( { model | robotConfig = case (D.decodeString decodeRobotConfig value) of
                                      Ok config -> Just config.robot
                                      _ -> Nothing
                    , fields = case (D.decodeString decodeRobotConfig value) of
                                      Ok config -> UpdateFields config.robot.name model.fields.fs model.fields.fc

                                      _ -> UpdateFields "no configuration available" "no configuration available" "no configuration available"
            }
            , Cmd.none
          )
        ChangeName name ->
          ( { model | fields = UpdateFields name model.fields.fs model.fields.fc }
          , Cmd.none
          )
        ChangeImuFs fs ->
          ( { model | fields = UpdateFields model.fields.name fs model.fields.fc }
          , Cmd.none
          )
        ChangeImuFc fc ->
          ( { model | fields = UpdateFields model.fields.name model.fields.fs fc }
          , Cmd.none
          )
        ResetUpdateFields -> 
          ( { model | fields = 
              case model.robotConfig of 
                Just config -> UpdateFields config.name (config.hardware.imu.sampling_frequency |> String.fromInt) (config.hardware.imu.low_pass_cut_off_frequency |> String.fromInt)
                Nothing  -> UpdateFields "" "" ""
            }
          , Cmd.none
          )
        SendUpdatesToRobot ->
          ( model
          , 
            case model.robotConfig of 
                  Just config -> 
                    let
                      f = model.fields
                      old_imu_settings = config.hardware.imu 
                      old_hardware_settings = config.hardware 
                      new_fs = (Maybe.withDefault config.hardware.imu.sampling_frequency (String.toInt f.fs))
                      new_fc = (Maybe.withDefault config.hardware.imu.low_pass_cut_off_frequency (String.toInt f.fc))
                      new_imu_settings = {old_imu_settings | sampling_frequency = new_fs, low_pass_cut_off_frequency = new_fc }
                      new_hardware_settings = {old_hardware_settings | imu = new_imu_settings}
                      -- new_config = (setHardware <| setIMU new_imu_settings) ({config | name = fields.name})
                      new_config = {config | name = f.name, hardware = new_hardware_settings}
                    in
                      websocketSettingsOut (E.encode 0 (encodeRobotConfig (RobotConfig new_config)))
                  Nothing  -> Cmd.none
            

          )

{- VIEW -}

view : Model -> Html Msg
view model =
    let      
      reply = case (D.decodeString decodeTwistData model.responses) of
                Ok value -> "linear-x: " ++ (value.linear_x |> String.fromFloat) ++ ", angular z: " ++ (value.angular_z |> String.fromFloat)
                _ -> "not a valid json"

      imu_reply = case (D.decodeString decodeImuData model.imu_values) of
                Ok value -> "t: " ++ (value.t |> String.fromInt) ++ ", ax: " ++ (value.ax |> String.fromFloat) ++ ", ay: " ++ (value.ay |> String.fromFloat) ++ ", az: " ++ (value.az |> String.fromFloat) ++ "gx: " ++ (value.gx |> String.fromFloat) ++ ", gy: " ++ (value.gy |> String.fromFloat) ++ ", gz: " ++ (value.gz |> String.fromFloat)
                _ -> "not a valid json"
      
      name = case model.robotConfig of 
                              Just config -> config.name
                              Nothing  -> "no configuration available"

      sampling_frequency = case model.robotConfig of 
                              Just config -> config.hardware.imu.sampling_frequency |> String.fromInt
                              Nothing  -> "no configuration available"
      
      low_pass_cut_off_frequency = case model.robotConfig of 
                              Just config -> config.hardware.imu.low_pass_cut_off_frequency |> String.fromInt
                              Nothing  -> "no configuration available"

      led = case model.robotConfig of
                              Just config -> config.hardware.led.gPIO |> String.fromInt
                              Nothing  -> "no configuration available"

      lin_x = model.twist.linear_x |> String.fromFloat
      ang_z = model.twist.angular_z |> String.fromFloat
    in
    div Style.full [
      div Style.row
        [ div Style.column [     
            div Style.container
            [ p [] [ text ("state: linear-x: " ++ lin_x ++ ", angular-z: " ++ ang_z) ]
            , p [] [ text ("reply:  " ++ reply) ]
            ]
          ]
        , div Style.column [ ImuViz.view model.imuDatas ]
        ]
      ,
      div Style.row
      [ div Style.column [ p [] [text ("imu: " ++ imu_reply)]]
      , div Style.column [   
        div []
          [ p []  [text "Robot name: ", input [ placeholder name, value model.fields.name, onInput ChangeName ] [] ] 
          , p []  [text "IMU fs: ", input [ placeholder sampling_frequency, value model.fields.fs, onInput ChangeImuFs ] [] ] 
          , p []  [text "IMU fc: ", input [ placeholder low_pass_cut_off_frequency, value model.fields.fc, onInput ChangeImuFc ] [] ]           
          , button [ onClick SendUpdatesToRobot ] [ text "Send updates to robot" ], button [ onClick ResetUpdateFields ] [ text "Reset fields" ]
          , p []  [text ("robotconfig led GPIO: " ++ led) ] 
          ] ]
      ]
    ]


{- SUBSCRIPTIONS -}

subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.batch [Sub.map KeyboardMsg Keyboard.subscriptions, websocketCmdVelIn WebsocketCmdVelIn, websocketImuIn WebsocketImuIn, websocketSettingsIn WebsocketSettingsIn]

main : Program () Model Msg
main =
    Browser.element
        { init = init
        , update = update
        , view = view
        , subscriptions = subscriptions
        }