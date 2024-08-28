cmake_minimum_required(VERSION 3.14)
project(HexChess)

set(CMAKE_CXX_STANDARD 17)
    
set(CONSOLE_SOURCE_FILES
    src/GameState.h src/GameState.cpp
    src/MCTS.h src/MCTS.cpp
    src/console.cpp
)
add_executable(console ${CONSOLE_SOURCE_FILES})

find_package(Boost REQUIRED)

set(MICROSERVICE_SOURCE_FILES
    src/GameState.h src/GameState.cpp
    src/MCTS.h src/MCTS.cpp
    src/boostUtils.h src/boostUtils.cpp
    src/Session.h src/Session.cpp
    src/Listener.h src/Listener.cpp
    src/microservice.cpp
)
add_executable(microservice ${MICROSERVICE_SOURCE_FILES})
target_include_directories(microservice PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(microservice ${Boost_LIBRARIES})