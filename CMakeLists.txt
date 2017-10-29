cmake_minimum_required(VERSION 3.2)

ADD_DEFINITIONS(
        -std=c++11
)

INCLUDE_DIRECTORIES(
        /usr/local/Cellar/bullet/2.87/include/bullet/
)

LINK_LIBRARIES(
 BulletDynamics BulletCollision LinearMath
)

ADD_EXECUTABLE(exsim-physics
        src/main.cpp
        src/Solarsystem.cpp
        src/Ship.cpp src/Ship.h src/Commands.h src/CommandHandler.cpp src/CommandHandler.h src/CommandParser.cpp src/CommandParser.h)


IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)

# Pull in rapidjson. Note that this also pulls in googletest that
# we use as well. Pulling that in explicitly causes conflicts and
# the easiest workaround seems to be to rely on the googletest
# coming in from rapidjson.

configure_file(CMakeLists-rapidjson.txt.in rapidjson-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/rapidjson-download )
if(result)
    message(FATAL_ERROR "CMake step for rapidjson failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/rapidjson-download )
if(result)
    message(FATAL_ERROR "Build step for rapidjson failed: ${result}")
endif()

add_subdirectory(${CMAKE_BINARY_DIR}/rapidjson-src
        ${CMAKE_BINARY_DIR}/rapidjson-build
        EXCLUDE_FROM_ALL)

INCLUDE_DIRECTORIES(
        ${CMAKE_BINARY_DIR}/rapidjson-src/include
)

add_executable(solarsystem_test
        src/Solarsystem_test.cpp
)
target_link_libraries(solarsystem_test gtest_main)

add_executable(ship_test
        src/Ship_test.cpp
        src/Ship.cpp
    )
target_link_libraries(ship_test gtest_main)

add_executable(commandparser_test
        src/CommandParser_test.cpp
        src/CommandParser.cpp
        src/CommandParser_test.cpp)
target_link_libraries(commandparser_test gtest_main)
