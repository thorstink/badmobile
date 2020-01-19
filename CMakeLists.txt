cmake_minimum_required(VERSION 3.10)
project(Badmobile VERSION 1.0.0)

# inspired from http://www.stablecoder.ca/2018/10/30/full-cmake-helper-suite.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# verbose and sanitizers
# https://stackoverflow.com/questions/45913603/for-gcc-in-the-fsanitize-option-how-to-use-address-and-leak-together
set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=${SANITIZE} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=${SANITIZE} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-psabi -Werror -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -Werror -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(code-coverage)
add_code_coverage() # Adds instrumentation to all targets

find_package(Eigen3 REQUIRED)
find_package(GTSAM REQUIRED)
find_package(Boost REQUIRED COMPONENTS fiber)
find_package(Seasocks REQUIRED)

find_library(GPIOD_LIBRARY NAMES libgpiodcxx.so)
if(NOT GPIOD_LIBRARY)
  message(FATAL_ERROR "gpiod library not found. Install https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/")
endif()

include_directories(${EIGEN3_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${SEASOCKS_INCLUDE_DIRS} lib)

set(dependencies
    ${Boost_LIBRARIES}
    ${GPIOD_LIBRARY}
    seasocks
    gtsam
)

if(BUILD_TESTING)
    # Enable testing for the project
    enable_testing()
    # because headers are somewhere else
    include_directories(src)
    # concat testfiles 
    set(TEST_DIR test)
    set(TEST_SOURCES
        ${TEST_DIR}/base.cc
        ${TEST_DIR}/state.cc
    )
    
    add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
    target_link_libraries(${PROJECT_NAME}_test ${dependencies})
    add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif()

#app
add_executable(${PROJECT_NAME} src/main.cc)
target_link_libraries(${PROJECT_NAME} ${dependencies})

#iopc
add_executable(${PROJECT_NAME}_iopoc src/iopoc.cc)
target_link_libraries(${PROJECT_NAME}_iopoc ${dependencies})

# target_code_coverage(main)

# install app
set(ui_files
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/index.html
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/badmobile.js
)

set(targets
  ${PROJECT_NAME}
)
# LD_PRELOAD=/path/to/preload.so
# set_property(TEST ${PROJECT_NAME}_test ${PROJECT_NAME} ${PROJECT_NAME}_iopoc
#    PROPERTY
#    ENVIRONMENT LD_PRELOAD=/path/to/preload.so)

install(FILES ${ui_files} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/ui)
install(TARGETS ${targets} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install)
