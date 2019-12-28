cmake_minimum_required(VERSION 3.10)
project(Badmobile VERSION 1.0.0)

find_package(Eigen3 REQUIRED)
find_package(GTSAM REQUIRED)
find_package(Boost REQUIRED COMPONENTS fiber)

include_directories(${EIGEN3_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set(dependencies
    ${Boost_LIBRARIES}
    gtsam
)


if(BUILD_TESTING)
    # Enable testing for the project
    enable_testing()
    # Get Catch
    find_package(Catch2 REQUIRED)
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
add_executable(main src/main.cc)
target_link_libraries(main ${dependencies})
