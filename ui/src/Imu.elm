module Imu exposing (decodeImuData, ImuData)

import Json.Decode as Decode exposing (Decoder, decodeString, float, int, string)
import Json.Decode.Pipeline exposing (required, optional, hardcoded)

-- Type
type alias ImuData =
    {  t : Int
    , ax : Float
    , ay : Float
    , az : Float
    , gx : Float
    , gy : Float
    , gz : Float
    }

-- DECODE

decodeImuData : Decoder ImuData
decodeImuData =
    Decode.succeed ImuData
        |> required "t" int
        |> required "ax" float
        |> required "ay" float
        |> required "az" float
        |> required "gx" float
        |> required "gy" float
        |> required "gz" float