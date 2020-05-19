module FakeImu exposing (generateImuData)

import Imu exposing (ImuData)
import Time

acc : Int -> Float -> Float
acc n offset = 
    (n |> toFloat) / 10.0 + offset |> sin 

gyr : Int -> Float -> Float
gyr n offset = 
    (n |> toFloat) /10.0 + offset |> cos 

generateImuData: Int -> List ImuData -> List ImuData
generateImuData n list =
   case n>0 of
   False  -> list
   True   -> generateImuData (n-1) (ImuData n (acc n 0.0) (acc n 0.2) (acc n 0.4) (gyr n 0.0) (gyr n 0.2) (gyr n 0.4)::list)
