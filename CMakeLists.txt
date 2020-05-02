cmake_minimum_required(VERSION 3.10)
project(Badmobile VERSION 1.0.0)

# inspired from http://www.stablecoder.ca/2018/10/30/full-cmake-helper-suite.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-psabi -Werror -Wall -Wextra -march=native -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -Werror -Wall -Wextra -march=native -O2")

# verbose and sanitizers
# https://stackoverflow.com/questions/45913603/for-gcc-in-the-fsanitize-option-how-to-use-address-and-leak-together
# run with the sanitizer on pi:
#set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=address ${CMAKE_SHARED_LINKER_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=address ${CMAKE_EXE_LINKER_FLAGS}")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(code-coverage)
add_code_coverage() # Adds instrumentation to all targets

find_package(Eigen3 REQUIRED)
find_package(GTSAM REQUIRED)
find_package(Boost REQUIRED COMPONENTS fiber)
find_package(Seasocks REQUIRED)
find_package(pigpio REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${SEASOCKS_INCLUDE_DIRS} ${PIGPIO_INCLUDE_DIRS} lib src/msg)

#bno055 imu library
add_library(bno055 
  # src/bno055/BNO055_driver/bno055.c
  src/bno055/i2c_bno055.c
)

set(dependencies
  ${Boost_LIBRARIES}
  pigpio
  Seasocks::seasocks
  gtsam
  bno055
)

#app
add_executable(${PROJECT_NAME}_app src/main.cc)
target_link_libraries(${PROJECT_NAME}_app ${dependencies})

#simulated app
add_executable(${PROJECT_NAME}_simulate_app src/simulate_app.cc)
target_link_libraries(${PROJECT_NAME}_simulate_app ${dependencies})

#imu
add_executable(${PROJECT_NAME}_lsm9ds1 src/lsm9ds1.cc)
target_link_libraries(${PROJECT_NAME}_lsm9ds1 ${dependencies})

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
      ${TEST_DIR}/bno055.cc
  )
  
  add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_test ${dependencies} bno055)
  add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif()

# target_code_coverage(main)

# install app
set(ui_files
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/index.html
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/badmobile.js
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/badmobile.min.js
)

set(targets
  ${PROJECT_NAME}_app
  ${PROJECT_NAME}_simulate_app
  ${PROJECT_NAME}_iopoc
  ${PROJECT_NAME}_motor
)

install(FILES ${ui_files} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/ui)
install(TARGETS ${targets} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install)
