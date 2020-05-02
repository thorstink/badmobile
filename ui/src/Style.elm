module Style exposing (container, row, column, full)

import Html
import Html.Attributes exposing (style)

full : List (Html.Attribute msg)
full =
    [ style "background-color" "orange"
    , style "width" "100vw"
    -- , style "height" "100vh"
    ]

container : List (Html.Attribute msg)
container =
    [ style "background-color" "rebeccapurple"
    , style "color" "white"
    , style "font-family" "sans-serif"
    , style "display" "flex"
    , style "overflow-wrap" "break-word"
    , style "align-items" "center"
    , style "justify-content" "flex-start"
    , style "box-sizing" "border-box"
    , style "flex-direction" "column"
    ]

row : List (Html.Attribute msg)
row =
    [ style "background-color" "blue"
    , style "width" "100vw"
    ]

column : List (Html.Attribute msg)
column =
    [ style "background-color" "white"
    , style "color" "black"
    , style "float" "left"
    , style "width" "50%"
    ]
    