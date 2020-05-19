module Teleop exposing (..)

import Keyboard exposing (Key(..))
import Json.Decode as Decode exposing (Decoder, decodeString, float, int, string, bool)
import Json.Decode.Pipeline exposing (required, optional, hardcoded)
import Json.Encode as E

type alias TwistData = {  
                      linear_x : Float
                    , angular_z : Float 
                    }

-- ENCODE
encode : TwistData -> E.Value
encode twist =
  E.object
    [ ("linear_x", E.float twist.linear_x)
    , ("angular_z", E.float twist.angular_z)
    ]

-- DECODE
decodeTwistData : Decoder TwistData
decodeTwistData =
    Decode.succeed TwistData
        |> required "linear_x" float
        |> required "angular_z" float

type Throttle = IncreaseLinear | DecreaseLinear | IncreaseAngular | DecreaseAngular | Stop 
                | Forward | Backward | Left | Right | CurveLeft | CurveRight

type Action = Publish | DontPublish

keyMapper key =
  -- we use R and F to respectfully increase and decrease linear velocity
  -- we use T and G to respectfully increase and decrease angular velocity
  -- wasdqe-keys can be used for sending the whole twist or only a part of it.
  -- all other keys result in sending a zero twist.
  case key of
    Character "W" ->
      IncreaseLinear
    Character "X" ->
      DecreaseLinear 
    Character "E" ->
      IncreaseAngular
    Character "C" ->
      DecreaseAngular
    Character "I" ->
      Forward
    Character "J" ->
      Left 
    Character "," ->
      Backward
    Character "L" ->
      Right
    Character "U" ->
      CurveLeft
    Character "O" ->
      CurveRight
    _ ->
      Stop

throttleMapper twist throttle = 
  case throttle of 
    IncreaseLinear ->
      ({ twist | linear_x = twist.linear_x * 1.1}, DontPublish)
    DecreaseLinear ->
      ({ twist | linear_x = twist.linear_x * 0.9}, DontPublish)
    IncreaseAngular ->
      ({ twist | angular_z = twist.angular_z * 1.1}, DontPublish)
    DecreaseAngular ->
      ({ twist | angular_z = twist.angular_z * 0.9}, DontPublish)
    Forward ->
      (TwistData twist.linear_x 0.0, Publish)
    Backward ->
      (TwistData -twist.linear_x 0.0, Publish)
    Left ->
      (TwistData 0.0 twist.angular_z, Publish)
    Right ->
      (TwistData 0.0 -twist.angular_z, Publish)
    CurveLeft ->
      (twist, Publish)
    CurveRight ->
      (TwistData twist.linear_x -twist.angular_z, Publish)
    Stop ->
      (TwistData 0.0 0.0, Publish)
