module ImuViz exposing (main,view, replyImuDecoder)

import Axis
import Html
import Color exposing (Color)
import Path exposing (Path)
import FakeImu exposing (ImuData, generateImuData)
import Scale exposing (ContinuousScale, OrdinalScale)
import Scale.Color
import Shape
import Statistics
import Time
import TypedSvg exposing (g, svg, text_)
import TypedSvg.Attributes exposing (class, dy, fill, fontFamily, stroke, textAnchor, transform, viewBox)
import TypedSvg.Attributes.InPx exposing (fontSize, height, strokeWidth, x, y)
import TypedSvg.Core exposing (Svg, text)
import TypedSvg.Types exposing (AnchorAlignment(..), Paint(..), Transform(..), em)
import Json.Decode as D

-- DECODE

replyImuDecoder : D.Decoder ImuData
replyImuDecoder =
    D.map7 ImuData 
      (D.at [ "t" ] D.int)
      (D.at [ "ax" ] D.float)
      (D.at [ "ay" ] D.float)
      (D.at [ "az" ] D.float)
      (D.at [ "gx" ] D.float)
      (D.at [ "gy" ] D.float)
      (D.at [ "gz" ] D.float)

w : Float
w =
    900


h : Float
h =
    450


padding : Float
padding =
    60


series =
    [ { label = "Ax"
      , accessor = .ax
      }
    , { label = "Ay"
      , accessor = .ay
      }
    , { label = "Az"
      , accessor = .az
      }
    , { label = "Gx"
      , accessor = .gx
      }
    , { label = "Gy"
      , accessor = .gy
      }
    , { label = "Gz"
      , accessor = .gz
      }
    ]

accessors : List (ImuData -> Float)
accessors =
    List.map .accessor series


values : ImuData -> List Float
values i =
    List.map (\a -> a i) accessors


colorScale : OrdinalScale String Color
colorScale =
    List.map .label series
        |> Scale.ordinal Scale.Color.category10


color : String -> Color
color =
    Scale.convert colorScale >> Maybe.withDefault Color.black


view : List ImuData -> Svg msg
view model =
    let
        last =
            List.reverse model
                |> List.head
                |> Maybe.withDefault (ImuData 0 0.0 0.0 0.0 0.0 0.0 0.0)

        first =
            List.head model
                |> Maybe.withDefault (ImuData 0 0.0 0.0 0.0 0.0 0.0 0.0)

        xScale : ContinuousScale Float
        xScale =
            model
                |> List.map (.t >> toFloat)
                |> Statistics.extent
                |> Maybe.withDefault ( 1900, 1901 )
                |> Scale.linear ( 0, w - 2 * padding )

        yScale : ContinuousScale Float
        yScale =
            model
                |> List.map (values >> List.maximum >> Maybe.withDefault 0)
                |> List.maximum
                |> Maybe.withDefault 0
                |> (\b -> ( -b, b ))
                |> Scale.linear ( h - 2 * padding, 0 )
                |> Scale.nice 4

        lineGenerator : ( Int, Float ) -> Maybe ( Float, Float )
        lineGenerator ( x, y ) =
            Just ( Scale.convert xScale (toFloat x), Scale.convert yScale y)

        line : (ImuData -> Float) -> Path
        line accessor =
            List.map (\i -> ( .t i, accessor i )) model
                |> List.map lineGenerator
                |> Shape.line Shape.monotoneInXCurve
    in
    svg [ viewBox 0 0 w h ]
        [ g [ transform [ Translate (padding - 1) (h - padding) ] ]
            [ Axis.bottom [ Axis.tickCount 10 ] xScale ]
        , g [ transform [ Translate padding padding ], class [ "series" ] ]
            (List.map
                (\{ accessor, label } ->
                    Path.element (line accessor)
                        [ stroke <| Paint <| color label
                        , strokeWidth 3
                        , fill PaintNone
                        ]
                )
                series
            )
        , g [ fontFamily [ "sans-serif" ], fontSize 10 ]
            (List.map
                (\{ accessor, label } ->
                    g
                        [ transform
                            [ Translate (w - padding + 10) (padding + Scale.convert yScale (accessor last))
                            ]
                        ]
                        [ text_ [ fill (Paint (color label)) ] [ text label ] ]
                )
                series
            )
        , g [ transform [ Translate (w - padding) (padding + 20) ] ]
            [ text_ [ fontFamily [ "sans-serif" ], fontSize 20, textAnchor AnchorEnd ] [ text "ImuData" ]
            , text_ [ fontFamily [ "sans-serif" ], fontSize 10, textAnchor AnchorEnd, dy (em 1) ] [ text "Source: the Badmobile" ]
            ]
        ]

main =
  view (generateImuData 200 [])
