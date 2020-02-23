port module Main exposing (main)

import Browser
import Html exposing (Html, div, li, p, text, ul)
import Keyboard exposing (Key(..))
import Style
import Teleop exposing (..)
import Json.Decode as D
import Json.Encode as E

port websocketCmdVelIn : (String -> msg) -> Sub msg
port websocketCmdVelOut : String -> Cmd msg
port websocketImuIn : (String -> msg) -> Sub msg

{- MODEL -}

type alias Model =
    {
      pressedKeys : List Key
    , twist : Twist
    , action : Action
    , responses : String
    , imu_values : String
    , input : String
    }


init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = Twist 1.0 1.0
      , action = DontPublish
      , responses = ""
      , imu_values = "hi2"
      , input = ""
      }
    , Cmd.none
    )

{- UPDATE -}

type Msg
  = KeyboardMsg Keyboard.Msg
  | WebsocketCmdVelIn String 
  | WebsocketImuIn String 

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
          ( { model | imu_values = value }
          , Cmd.none
          )

{- VIEW -}

view : Model -> Html Msg
view model =
    let      
      reply = case (D.decodeString replyDecoder model.responses) of
                Ok value -> "linear-x: " ++ (value.linear_x |> String.fromFloat) ++ ", angular z: " ++ (value.angular_z |> String.fromFloat)
                _ -> "not a valid json"

      lin_x = model.twist.linear_x |> String.fromFloat
      ang_z = model.twist.angular_z |> String.fromFloat
    in
    div Style.container
        [ p [] [ text ("state: linear-x: " ++ lin_x ++ ", angular-z: " ++ ang_z) ]
        , p [] [ text ("reply:  " ++ reply) ]
        , p [] [ text (model.imu_values) ]
        ]

{- SUBSCRIPTIONS -}

subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.batch [Sub.map KeyboardMsg Keyboard.subscriptions, websocketCmdVelIn WebsocketCmdVelIn, websocketImuIn WebsocketImuIn]

main : Program () Model Msg
main =
    Browser.element
        { init = init
        , update = update
        , view = view
        , subscriptions = subscriptions
        }