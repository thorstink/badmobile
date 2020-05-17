module RobotConfig exposing (..)

import Json.Encode
import Json.Decode as Decode exposing (Decoder, decodeString, float, int, string)
import Json.Decode.Pipeline exposing (required, optional, hardcoded)

type alias RobotConfig =
    { robot : RobotConfigRobot
    }

type alias RobotConfigRobotParameters =
    { track : Float
    , wheel_diameter_left : Float
    , wheel_diameter_right : Float
    }

type alias RobotConfigRobotHardwareImu =
    { sampling_frequency : Int
    , low_pass_cut_off_frequency : Int
    , sCLK : Int
    , mOSI : Int
    , mISO : Int
    , cSAG : Int
    }

type alias RobotConfigRobotHardwareMotor_left =
    { pWM_GPIO_FORWARD : Int
    , pWM_GPIO_REVERSE : Int
    }

type alias RobotConfigRobotHardwareMotor_right =
    { pWM_GPIO_FORWARD : Int
    , pWM_GPIO_REVERSE : Int
    }

type alias RobotConfigRobotHardwareLed =
    { gPIO : Int
    }

type alias RobotConfigRobotHardware =
    { imu : RobotConfigRobotHardwareImu
    , motor_left : RobotConfigRobotHardwareMotor_left
    , motor_right : RobotConfigRobotHardwareMotor_right
    , led : RobotConfigRobotHardwareLed
    }

type alias RobotConfigRobot =
    { name : String
    , parameters : RobotConfigRobotParameters
    , hardware : RobotConfigRobotHardware
    }

decodeRobotConfig : Decoder RobotConfig
decodeRobotConfig =
    Decode.succeed RobotConfig
        |> Json.Decode.Pipeline.required "robot" (decodeRobotConfigRobot)

decodeRobotConfigRobotParameters : Decoder RobotConfigRobotParameters
decodeRobotConfigRobotParameters =
    Decode.succeed RobotConfigRobotParameters
        |> Json.Decode.Pipeline.required "track" (float)
        |> Json.Decode.Pipeline.required "wheel_diameter_left" (float)
        |> Json.Decode.Pipeline.required "wheel_diameter_right" (float)

decodeRobotConfigRobotHardwareImu : Decoder RobotConfigRobotHardwareImu
decodeRobotConfigRobotHardwareImu =
    Decode.succeed RobotConfigRobotHardwareImu
        |> Json.Decode.Pipeline.required "sampling_frequency" (int)
        |> Json.Decode.Pipeline.required "low_pass_cut_off_frequency" (int)
        |> Json.Decode.Pipeline.required "SCLK" (int)
        |> Json.Decode.Pipeline.required "MOSI" (int)
        |> Json.Decode.Pipeline.required "MISO" (int)
        |> Json.Decode.Pipeline.required "CSAG" (int)

decodeRobotConfigRobotHardwareMotor_left : Decoder RobotConfigRobotHardwareMotor_left
decodeRobotConfigRobotHardwareMotor_left =
    Decode.succeed RobotConfigRobotHardwareMotor_left
        |> Json.Decode.Pipeline.required "PWM_GPIO_FORWARD" (int)
        |> Json.Decode.Pipeline.required "PWM_GPIO_REVERSE" (int)

decodeRobotConfigRobotHardwareMotor_right : Decoder RobotConfigRobotHardwareMotor_right
decodeRobotConfigRobotHardwareMotor_right =
    Decode.succeed RobotConfigRobotHardwareMotor_right
        |> Json.Decode.Pipeline.required "PWM_GPIO_FORWARD" (int)
        |> Json.Decode.Pipeline.required "PWM_GPIO_REVERSE" (int)

decodeRobotConfigRobotHardwareLed : Decoder RobotConfigRobotHardwareLed
decodeRobotConfigRobotHardwareLed =
    Decode.succeed RobotConfigRobotHardwareLed
        |> Json.Decode.Pipeline.required "GPIO" (int)

decodeRobotConfigRobotHardware : Decoder RobotConfigRobotHardware
decodeRobotConfigRobotHardware =
    Decode.succeed RobotConfigRobotHardware
        |> Json.Decode.Pipeline.required "imu" (decodeRobotConfigRobotHardwareImu)
        |> Json.Decode.Pipeline.required "motor_left" (decodeRobotConfigRobotHardwareMotor_left)
        |> Json.Decode.Pipeline.required "motor_right" (decodeRobotConfigRobotHardwareMotor_right)
        |> Json.Decode.Pipeline.required "led" (decodeRobotConfigRobotHardwareLed)

decodeRobotConfigRobot : Decoder RobotConfigRobot
decodeRobotConfigRobot =
    Decode.succeed RobotConfigRobot
        |> Json.Decode.Pipeline.required "name" (string)
        |> Json.Decode.Pipeline.required "parameters" (decodeRobotConfigRobotParameters)
        |> Json.Decode.Pipeline.required "hardware" (decodeRobotConfigRobotHardware)

encodeRobotConfig : RobotConfig -> Json.Encode.Value
encodeRobotConfig record =
    Json.Encode.object
        [ ("robot",  encodeRobotConfigRobot <| record.robot)
        ]

encodeRobotConfigRobotParameters : RobotConfigRobotParameters -> Json.Encode.Value
encodeRobotConfigRobotParameters record =
    Json.Encode.object
        [ ("track",  Json.Encode.float <| record.track)
        , ("wheel_diameter_left",  Json.Encode.float <| record.wheel_diameter_left)
        , ("wheel_diameter_right",  Json.Encode.float <| record.wheel_diameter_right)
        ]

encodeRobotConfigRobotHardwareImu : RobotConfigRobotHardwareImu -> Json.Encode.Value
encodeRobotConfigRobotHardwareImu record =
    Json.Encode.object
        [ ("sampling_frequency",  Json.Encode.int <| record.sampling_frequency)
        , ("low_pass_cut_off_frequency",  Json.Encode.int <| record.low_pass_cut_off_frequency)
        , ("sCLK",  Json.Encode.int <| record.sCLK)
        , ("mOSI",  Json.Encode.int <| record.mOSI)
        , ("mISO",  Json.Encode.int <| record.mISO)
        , ("cSAG",  Json.Encode.int <| record.cSAG)
        ]

encodeRobotConfigRobotHardwareMotor_left : RobotConfigRobotHardwareMotor_left -> Json.Encode.Value
encodeRobotConfigRobotHardwareMotor_left record =
    Json.Encode.object
        [ ("pWM_GPIO_FORWARD",  Json.Encode.int <| record.pWM_GPIO_FORWARD)
        , ("pWM_GPIO_REVERSE",  Json.Encode.int <| record.pWM_GPIO_REVERSE)
        ]

encodeRobotConfigRobotHardwareMotor_right : RobotConfigRobotHardwareMotor_right -> Json.Encode.Value
encodeRobotConfigRobotHardwareMotor_right record =
    Json.Encode.object
        [ ("pWM_GPIO_FORWARD",  Json.Encode.int <| record.pWM_GPIO_FORWARD)
        , ("pWM_GPIO_REVERSE",  Json.Encode.int <| record.pWM_GPIO_REVERSE)
        ]

encodeRobotConfigRobotHardwareLed : RobotConfigRobotHardwareLed -> Json.Encode.Value
encodeRobotConfigRobotHardwareLed record =
    Json.Encode.object
        [ ("gPIO",  Json.Encode.int <| record.gPIO)
        ]

encodeRobotConfigRobotHardware : RobotConfigRobotHardware -> Json.Encode.Value
encodeRobotConfigRobotHardware record =
    Json.Encode.object
        [ ("imu",  encodeRobotConfigRobotHardwareImu <| record.imu)
        , ("motor_left",  encodeRobotConfigRobotHardwareMotor_left <| record.motor_left)
        , ("motor_right",  encodeRobotConfigRobotHardwareMotor_right <| record.motor_right)
        , ("led",  encodeRobotConfigRobotHardwareLed <| record.led)
        ]

encodeRobotConfigRobot : RobotConfigRobot -> Json.Encode.Value
encodeRobotConfigRobot record =
    Json.Encode.object
        [ ("name",  Json.Encode.string <| record.name)
        , ("parameters",  encodeRobotConfigRobotParameters <| record.parameters)
        , ("hardware",  encodeRobotConfigRobotHardware <| record.hardware)
        ]