module Robotconfig exposing (..)

import Json.Decode as Decode exposing (Decoder, decodeString, float, int, string)
import Json.Decode.Pipeline exposing (required, optional, hardcoded)

-- The types

type alias DifferentialDriveParameters = 
  {
    track: Float
  , wheel_diameter_left: Float
  , wheel_diameter_right: Float 
  }

type alias Imu =  
  {
    sampling_frequency : Int
  , low_pass_cut_off_frequency: Int
  , sclk: Int
  , mosi: Int
  , miso: Int
  , csag: Int
  }

type alias Motor =  
  {
    pwm_gpio_forward: Int
  , pwm_gpio_reverse: Int
  }

type alias Led = {gpio: Int}

type alias Hardware = 
  {
    imu: Imu
  , motor_left: Motor
  , motor_right: Motor
  , led: Led  
  }

type alias RobotConfig = 
  {
    name: String
  , differential_drive_parameters: DifferentialDriveParameters
  , hardware: Hardware
  }

type alias Config = 
  {
    robot: RobotConfig
  }

-- Json Decode and Encode

-- Decode all the bits
decodeDifferentialDriveParameters : Decoder DifferentialDriveParameters
decodeDifferentialDriveParameters =
    Decode.succeed DifferentialDriveParameters
        |> required "track" float
        |> required "wheel_diameter_left" float
        |> required "wheel_diameter_right" float

decodeImu : Decoder Imu
decodeImu =
    Decode.succeed Imu
        |> required "sampling_frequency" int
        |> required "low_pass_cut_off_frequency" int
        |> required "SCLK" int
        |> required "MOSI" int
        |> required "MISO" int
        |> required "CSAG" int

decodeMotor : Decoder Motor
decodeMotor =
    Decode.succeed Motor
        |> required "PWM_GPIO_FORWARD" int
        |> required "PWM_GPIO_REVERSE" int

decodeLed : Decoder Led
decodeLed =
    Decode.succeed Led
        |> required "GPIO" int

decodeHardware : Decoder Hardware
decodeHardware =
    Decode.succeed Hardware
        |> required "imu" decodeImu
        |> required "motor_left" decodeMotor
        |> required "motor_right" decodeMotor
        |> required "led" decodeLed

decodeRobotConfig : Decoder RobotConfig
decodeRobotConfig =
    Decode.succeed RobotConfig
        |> required "name" string
        |> required "differential_drive_parameters" decodeDifferentialDriveParameters
        |> required "hardware" decodeHardware

decodeConfig : Decoder Config
decodeConfig =
    Decode.succeed Config
        |> required "robot" decodeRobotConfig




-- Encode