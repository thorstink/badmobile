port module Main exposing (main)

import Browser
import Html exposing (Html,Attribute, div, li, p, text, ul, input, button)
import Html.Attributes exposing (..)
import Html.Events exposing (onInput,onClick)
import Keyboard exposing (Key(..))
import Style
import Teleop exposing (..)
import Robotconfig exposing (..)
import Json.Decode as D
import Json.Encode as E
import ImuViz 
import FakeImu exposing (ImuData, acc, gyr)

port websocketCmdVelIn : (String -> msg) -> Sub msg
port websocketCmdVelOut : String -> Cmd msg
port websocketSettingsOut : String -> Cmd msg
port websocketSettingsIn : (String -> msg) -> Sub msg
port websocketImuIn : (String -> msg) -> Sub msg

-- ENCODE

encodeRobotName : String -> E.Value
encodeRobotName name =
  E.object
    [ ( "robot"
      , E.object
        [ ( "name", E.string name )
        ] 
      )
    ]

{- MODEL -}

type alias Model =
    {
      pressedKeys : List Key
    , twist : Twist
    , action : Action
    , responses : String
    , imu_values : String
    , name : String
    , input : String
    , t : Int
    , lastImu : ImuData
    , imuDatas : List ImuData
    , robotConfig : Maybe RobotConfig
    }


init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = Twist 1.0 1.0
      , action = DontPublish
      , responses = ""
      , imu_values = "hi2"
      , name = ""
      , input = ""
      , t = 0
      , lastImu = ImuData 0 0.0 0.0 0.0 0.0 0.0 0.0
      , imuDatas = []
      , robotConfig = Nothing
      }
    , Cmd.none
    )

{- UPDATE -}

type Msg
  = KeyboardMsg Keyboard.Msg
  | WebsocketCmdVelIn String 
  | WebsocketImuIn String 
  | WebsocketSettingsIn String 
  | Change String
  | UpdateName

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
                          new_sample = case (D.decodeString ImuViz.replyImuDecoder model.imu_values) of
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
          ( { model | robotConfig = case (D.decodeString decodeConfig value) of
                                      Ok config -> Just config.robot
                                      _ -> Nothing

            }
            , Cmd.none
          )
        Change value ->
          ( { model | name = value }
          , Cmd.none
          )
        UpdateName ->
          ( model
          , websocketSettingsOut (E.encode 0 (encodeRobotName model.name))
          )




{- VIEW -}

view : Model -> Html Msg
view model =
    let      
      reply = case (D.decodeString replyDecoder model.responses) of
                Ok value -> "linear-x: " ++ (value.linear_x |> String.fromFloat) ++ ", angular z: " ++ (value.angular_z |> String.fromFloat)
                _ -> "not a valid json"

      imu_reply = case (D.decodeString ImuViz.replyImuDecoder model.imu_values) of
                Ok value -> "ax: " ++ (value.ax |> String.fromFloat) ++ ", ay: " ++ (value.ay |> String.fromFloat) ++ ", az: " ++ (value.az |> String.fromFloat) ++ "gx: " ++ (value.gx |> String.fromFloat) ++ ", gy: " ++ (value.gy |> String.fromFloat) ++ ", gz: " ++ (value.gz |> String.fromFloat)
                _ -> "not a valid json"
      
      sampling_frequency = case model.robotConfig of 
                              Just config -> config.hardware.imu.sampling_frequency |> String.fromInt
                              Nothing  -> "no configuration available"

      led = case model.robotConfig of
                              Just config -> config.hardware.led.gpio |> String.fromInt
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
          [ input [ placeholder "Robot name", value model.name, onInput Change ] []
          , button [ onClick UpdateName ] [ text "update name" ]
          , p []  [text ("robotconfig imu frequency: " ++ sampling_frequency) ] 
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