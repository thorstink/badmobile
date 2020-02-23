module LivePlot exposing (main)

import Browser
import Html exposing (..)
import Task
import Time
import ImuViz 
import FakeImu exposing (ImuData, acc, gyr)


-- MAIN

main =
  Browser.element
    { init = init
    , view = view
    , update = update
    , subscriptions = subscriptions
    }



-- MODEL


type alias Model =
  { zone : Time.Zone
  , time : Time.Posix
  , t : Int
  , imuDatas : List ImuData
  }


init : () -> (Model, Cmd Msg)
init _ =
  ( Model Time.utc (Time.millisToPosix 0) 0 []
  , Task.perform AdjustTimeZone Time.here
  )



-- UPDATE


type Msg
  = Tick Time.Posix
  | AdjustTimeZone Time.Zone



update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
  case msg of
    Tick newTime ->
      ( { model | time = newTime 
                , t = model.t + 1
                , imuDatas = 
                  (
                    let 
                      new_sample = ImuData model.t (acc model.t 0.0) (acc model.t 0.2) (acc model.t 0.4) (gyr model.t 0.0) (gyr model.t 0.2) (gyr model.t 0.4)
                    in
                      case (List.tail model.imuDatas) of
                        Just tail ->  ( if List.length model.imuDatas > 50 then
                                          tail ++ [new_sample]
                                        else
                                          model.imuDatas ++ [new_sample]
                                      )
                        _         -> model.imuDatas ++ [new_sample]
                  )
        }
      , Cmd.none
      )

    AdjustTimeZone newZone ->
      ( { model | zone = newZone }
      , Cmd.none
      )



-- SUBSCRIPTIONS


subscriptions : Model -> Sub Msg
subscriptions model =
  Time.every 40 Tick

-- VIEW


view : Model -> Html Msg
view model =
  ImuViz.view model.imuDatas
