cmake_minimum_required(VERSION 3.10)
project(Badmobile VERSION 1.0.0)

find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS fiber)

include_directories(${EIGEN3_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set(dependencies
${Boost_LIBRARIES}
)

# if(BUILD_TESTING)
#     find_package(Catch2 REQUIRED)
#     include_directories(src)
#     set(TEST_DIR test)
#     set(TEST_SOURCES
#         ${TEST_DIR}/base.cc
#         ${TEST_DIR}/state.cc
#     )
    
#     add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
#     target_link_libraries(${PROJECT_NAME}_test ${dependencies})
#     add_test(test ${PROJECT_NAME}_test)
# endif()

#app
add_executable(main src/main.cc)
target_link_libraries(main ${dependencies})
