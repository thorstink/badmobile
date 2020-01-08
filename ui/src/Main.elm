port module Main exposing (main)

import Browser
import Html exposing (Html, div, li, p, text, ul)
import Keyboard exposing (Key(..))
import Style
import Teleop exposing (..)
import Json.Decode exposing (..)
import Json.Encode as E

port websocketIn : (String -> msg) -> Sub msg
port websocketOut : String -> Cmd msg

-- ENCODE

encode : Twist -> E.Value
encode twist =
  E.object
    [ ("linear_x", E.float twist.linear_x)
    , ("angular_z", E.float twist.angular_z)
    ]

{- MODEL -}

type alias Model =
    {
      pressedKeys : List Key
    , twist : Twist
    , action : Action
    , responses : String
    , input : String
    }


init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = Twist 1.0 1.0
      , action = DontPublish
      , responses = ""
      , input = ""
      }
    , Cmd.none
    )

{- UPDATE -}

type Msg
  = KeyboardMsg Keyboard.Msg | WebsocketIn String 

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
                      Just (cmd, _)  -> cmd
                      _              -> model.twist
            }
            , 
            let 
              twist = case List.head (List.map (\key -> throttleMapper model.twist (keyMapper key)) (Keyboard.update keyMsg model.pressedKeys)) of
                                    Just (cmd, _)  -> cmd
                                    _              -> model.twist
            in
              -- websocketOut (Debug.toString twist)
              websocketOut (E.encode 0 (encode twist))
          )
        WebsocketIn value ->
          ( { model | responses = value }
          , Cmd.none
          )

{- VIEW -}

type alias User = { received : Float }
userDecoder : Json.Decode.Decoder User
userDecoder =
    Json.Decode.map User (Json.Decode.at [ "received" ] Json.Decode.float)

view : Model -> Html Msg
view model =
    let      
      reply = case (Json.Decode.decodeString userDecoder  model.responses) of
                Ok value -> value.received |> String.fromFloat
                _ -> "not a valid json"

      lin_x = model.twist.linear_x |> String.fromFloat
      ang_z = model.twist.angular_z |> String.fromFloat
    in
    div Style.container
        [ p [] [ text ("cmd_vel: linear-x: " ++ lin_x ++ ", angular-z: " ++ ang_z) ]
        , p [] [ text "Currently pressed down:" ]
        -- , model.pressedKeys |> List.map (\key -> li [] [ text (Debug.toString key)]) |> Html.ul []
        , p [] [ text ("reply: "++reply) ]
        ]

{- SUBSCRIPTIONS -}

subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.batch [Sub.map KeyboardMsg Keyboard.subscriptions, websocketIn WebsocketIn]

main : Program () Model Msg
main =
    Browser.element
        { init = init
        , update = update
        , view = view
        , subscriptions = subscriptions
        }