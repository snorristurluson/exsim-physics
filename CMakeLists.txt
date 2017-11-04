cmake_minimum_required(VERSION 3.2)
include(ExternalProject)

ADD_DEFINITIONS(
        -std=c++11
)

INCLUDE_DIRECTORIES(
		3rdparty/rapidjson/include
)

IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(App_BasicExample PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)

configure_file(CMakeLists-bulletphysics.txt.in bulletphysics-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bulletphysics-download )
if(result)
    message(FATAL_ERROR "CMake step for bulletphysics failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bulletphysics-download )
if(result)
    message(FATAL_ERROR "Build step for bulletphysics failed: ${result}")
endif()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/bulletphysics-src/src)
LINK_DIRECTORIES(
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3Collision
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3Common
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3Dynamics
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3Geometry
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3OpenCL
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/Bullet3Serialize
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/BulletCollision
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/BulletDynamics
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/BulletInverseDynamics
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/BulletSoftBody
        ${CMAKE_BINARY_DIR}/bulletphysics-build/src/LinearMath
)

# Download and unpack googletest at configure time
configure_file(CMakeLists-googletest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

LINK_LIBRARIES(
        BulletDynamics BulletCollision LinearMath
)

ADD_EXECUTABLE(exsim-physics
        src/main.cpp
        src/Solarsystem.cpp
        src/Ship.cpp src/Ship.h src/Commands.h src/CommandHandler.cpp src/CommandHandler.h src/CommandParser.cpp src/CommandParser.h)


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
