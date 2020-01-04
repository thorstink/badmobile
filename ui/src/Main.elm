module Main exposing (main)

import Browser
import Html exposing (Html, div, li, p, text, ul)
import Keyboard exposing (Key(..), RawKey)
import Keyboard.Arrows
import Style

type alias Twist = {  
                      linear_x : Float
                    , angular_z : Float 
                    }

type Msg
  = KeyboardMsg Keyboard.Msg

type Throttle = IncreaseLinear | DecreaseLinear | IncreaseAngular | DecreaseAngular | Stop | Noop
type Action = Publish | DontPublish

{-| We don't need any other info in the model, since we can get everything we
need using the helpers right in the `view`!
This way we always have a single source of truth, and we don't need to remember
to do anything special in the update.
-}
type alias Model =
    {
      pressedKeys : List Key
    , twist : Twist
    }


keyMapper key =
  -- we use R and F to respectfully increase and decrease linear velocity
  -- we use T and G to respectfully increase and decrease angular velocity
  case key of
    Character "R" ->
      IncreaseLinear
    Character "F" ->
      DecreaseLinear 
    Character "T" ->
      IncreaseAngular
    Character "G" ->
      DecreaseAngular
    Character "W" ->
      Noop
    Character "X" ->
      Noop 
    Character "A" ->
      Noop
    Character "D" ->
      Noop
    _ ->
      Stop

throttleMapper twist throttle = 
  case throttle of 
    IncreaseLinear ->
      ({ twist | linear_x = twist.linear_x + 0.02}, DontPublish)
    DecreaseLinear ->
      ({ twist | linear_x = twist.linear_x - 0.02}, DontPublish)
    IncreaseAngular ->
      ({ twist | angular_z = twist.angular_z + 0.02}, DontPublish)
    DecreaseAngular ->
      ({ twist | angular_z = twist.angular_z - 0.02}, DontPublish)
    Stop ->
      (Twist 0.0 0.0, Publish)
    Noop ->
      (twist, Publish)

init : () -> ( Model, Cmd Msg )
init _ =
    ( { 
        pressedKeys = []
      , twist = Twist 1.0 1.0
      }
    , Cmd.none
    )

update : Msg -> Model -> ( Model, Cmd Msg )
update msg model =
    case msg of
        KeyboardMsg keyMsg ->
            ( { model
                | pressedKeys = Keyboard.update keyMsg model.pressedKeys,
                  twist = 
                    (
                      case List.head (List.map (\key -> throttleMapper model.twist (keyMapper key)) model.pressedKeys) of
                        Just (cmd, action)  -> cmd 
                        _                   -> model.twist
                    )
              }
            , Cmd.none
            )

view : Model -> Html msg
view model =
    let
        shiftPressed =
            List.member Shift model.pressedKeys

        arrows =
            Keyboard.Arrows.arrows model.pressedKeys

        wasd =
            Keyboard.Arrows.wasd model.pressedKeys

    in


    div Style.container
        [ text ("Shift: " ++ Debug.toString shiftPressed)
        , p [] [ text ("Arrows: " ++ Debug.toString arrows) ]
        , p [] [ text ("WASD: " ++ Debug.toString wasd) ]
        , p [] [ text ("cmd_vel: " ++ Debug.toString model.twist) ]
        , p [] [ text "Currently pressed down:" ]
        , ul []
            (List.map (\key -> li [] [ text (Debug.toString key) ]) model.pressedKeys)
        ]


subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.map KeyboardMsg Keyboard.subscriptions


main : Program () Model Msg
main =
    Browser.element
        { init = init
        , update = update
        , view = view
        , subscriptions = subscriptions
        }