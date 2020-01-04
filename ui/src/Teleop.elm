module Teleop exposing (..)

import Keyboard exposing (Key(..))

type alias Twist = {  
                      linear_x : Float
                    , angular_z : Float 
                    }

type Throttle = IncreaseLinear | DecreaseLinear | IncreaseAngular | DecreaseAngular | Stop | Noop
type Action = Publish | DontPublish

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
