port module Main exposing (main)

import Browser
import Html exposing (Html, div, li, p, text, ul)
import Keyboard exposing (Key(..))
import Keyboard.Arrows
import Style
import Teleop exposing (..)

-- websocket example from https://stackoverflow.com/questions/52010340/how-to-get-websockets-working-in-elm-0-19
import Html.Attributes as HA
import Html.Events as HE
import Json.Encode as JE

-- JavaScript usage: app.ports.websocketIn.send(response);
port websocketIn : (String -> msg) -> Sub msg
-- JavaScript usage: app.ports.websocketOut.subscribe(handler);
port websocketOut : String -> Cmd msg

{- MODEL -}

type alias Model =
    {
      pressedKeys : List Key
    , twist : Twist
    , action : Action
    , responses : List String
    , input : String
    }


init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = Twist 1.0 1.0
      , action = DontPublish
      , responses = []
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
            { model
              | pressedKeys = Keyboard.update keyMsg model.pressedKeys,
                twist = 
                    case List.head (List.map (\key -> throttleMapper model.twist (keyMapper key)) model.pressedKeys) of
                      Just (cmd, action)  -> cmd
                      _                   -> model.twist
            }
          , websocketOut (Debug.toString model.twist)
          )
        WebsocketIn value ->
          ( { model | responses = value :: model.responses }
          , Cmd.none
          )

{- VIEW -}

view : Model -> Html Msg
view model =
    div Style.container
        [ p [] [ text ("cmd_vel: " ++ Debug.toString model.twist) ]
        , p [] [ text "Currently pressed down:" ]
        , model.pressedKeys |> List.map (\key -> li [] [ text (Debug.toString key)]) |> Html.ul []
        , model.responses |> List.map (\string-> Html.li [] [Html.text string]) |> Html.ol []
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