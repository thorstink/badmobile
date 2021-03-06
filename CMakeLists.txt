cmake_minimum_required(VERSION 3.10)
project(Badmobile VERSION 1.0.0)

# inspired from http://www.stablecoder.ca/2018/10/30/full-cmake-helper-suite.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-psabi -Wno-unused-parameter -Werror -Wall -Wextra -march=native -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -Wno-unused-parameter -Werror -Wall -Wextra -march=native -O2")

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
find_package(Seasocks REQUIRED)
find_package(pigpio REQUIRED)
find_package(fmt REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${SEASOCKS_INCLUDE_DIRS} ${PIGPIO_INCLUDE_DIRS} ${FMT_INCLUDE_DIRS} lib src/msg)

set(dependencies
  fmt
  pigpio
  Seasocks::seasocks
  gtsam
)

#app
add_executable(${PROJECT_NAME}_app src/main.cc)
target_link_libraries(${PROJECT_NAME}_app ${dependencies})

#simulated app
add_executable(${PROJECT_NAME}_simulate_app src/simulate_app.cc)
target_link_libraries(${PROJECT_NAME}_simulate_app ${dependencies})

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
      ${TEST_DIR}/config.cc
      ${TEST_DIR}/lowpass.cc
  )
  
  add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_test ${dependencies})
  add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif()

# target_code_coverage(main)

# install app
set(ui_files
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/index.html
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/badmobile.js
  ${CMAKE_CURRENT_SOURCE_DIR}/ui/badmobile.min.js
)

set(configuration_files
  ${CMAKE_CURRENT_SOURCE_DIR}/config/application_config.json
)

set(targets
  ${PROJECT_NAME}_app
  ${PROJECT_NAME}_simulate_app
)

install(FILES ${ui_files} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/ui)
install(FILES ${configuration_files} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/config)
install(TARGETS ${targets} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install)
