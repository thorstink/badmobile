module Teleop exposing (..)

import Keyboard exposing (Key(..))
import Json.Decode as D
import Json.Encode as E

type alias Twist = {  
                      linear_x : Float
                    , angular_z : Float 
                    }

-- ENCODE

encode : Twist -> E.Value
encode twist =
  E.object
    [ ("linear_x", E.float twist.linear_x)
    , ("angular_z", E.float twist.angular_z)
    ]

-- DECODE

type alias Reply = { received : Bool, linear_x : Float, angular_z : Float }
replyDecoder : D.Decoder Reply
replyDecoder =
    D.map3 Reply 
      (D.at [ "received" ] D.bool)
      (D.at [ "linear_x" ] D.float)
      (D.at [ "angular_z" ] D.float)

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
      (Twist twist.linear_x 0.0, Publish)
    Backward ->
      (Twist -twist.linear_x 0.0, Publish)
    Left ->
      (Twist 0.0 twist.angular_z, Publish)
    Right ->
      (Twist 0.0 -twist.angular_z, Publish)
    CurveLeft ->
      (twist, Publish)
    CurveRight ->
      (Twist twist.linear_x -twist.angular_z, Publish)
    Stop ->
      (Twist 0.0 0.0, Publish)
