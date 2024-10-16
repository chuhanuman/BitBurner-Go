cmake_minimum_required(VERSION 3.14)
project(HexChess)

list(APPEND CMAKE_PREFIX_PATH "C:\\libtorch")

find_package(Torch REQUIRED)

set(CMAKE_CXX_STANDARD 17)
    
set(CONSOLE_SOURCE_FILES
    src/game/GameStateConstants.h
    src/game/GameState.h src/game/GameState.cpp
    src/ai/GameStateData.h src/ai/GameStateData.cpp
    src/ai/Example.h src/ai/Example.cpp
    src/ai/Net.h
    src/ai/NeuralNetwork.h src/ai/NeuralNetwork.cpp
    src/ai/MCTSConstants.h
    src/ai/MCTS.h src/ai/MCTS.cpp
    src/ai/BasicMCTS.h src/ai/BasicMCTS.cpp
    src/ai/AdvancedMCTS.h src/ai/AdvancedMCTS.cpp
    src/utils.h src/utils.cpp
    src/console.cpp
)
add_executable(console ${CONSOLE_SOURCE_FILES})
target_link_libraries(console "${TORCH_LIBRARIES}")

set(READER_SOURCE_FILES
    src/game/GameStateConstants.h
    src/game/GameState.h src/game/GameState.cpp
    src/ai/GameStateData.h src/ai/GameStateData.cpp
    src/ai/Example.h src/ai/Example.cpp
    src/utils.h src/utils.cpp
    src/reader.cpp
)
add_executable(reader ${READER_SOURCE_FILES})

find_package(Boost REQUIRED)

set(MICROSERVICE_SOURCE_FILES
    src/utils.h src/utils.cpp
    src/game/GameState.h src/game/GameState.cpp
    src/ai/MCTS.h src/ai/MCTS.cpp
    src/boostUtils.h src/boostUtils.cpp
    src/Session.h src/Session.cpp
    src/Listener.h src/Listener.cpp
    src/microservice.cpp
)
add_executable(microservice ${MICROSERVICE_SOURCE_FILES})
target_include_directories(microservice PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(microservice ${Boost_LIBRARIES})
target_link_libraries(microservice "${TORCH_LIBRARIES}")

set(TRAINER_SOURCE_FILES
    src/game/GameStateConstants.h
    src/game/GameState.h src/game/GameState.cpp
    src/ai/GameStateData.h src/ai/GameStateData.cpp
    src/ai/Example.h src/ai/Example.cpp
    src/ai/Net.h
    src/ai/NeuralNetwork.h src/ai/NeuralNetwork.cpp
    src/ai/MCTSConstants.h
    src/ai/MCTS.h src/ai/MCTS.cpp
    src/ai/AdvancedMCTS.h src/ai/AdvancedMCTS.cpp
    src/utils.h src/utils.cpp
    src/trainer.cpp
)
add_executable(trainer ${TRAINER_SOURCE_FILES})
target_link_libraries(trainer "${TORCH_LIBRARIES}")

file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
add_custom_command(TARGET console POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS} $<TARGET_FILE_DIR:console>)
add_custom_command(TARGET microservice POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS} $<TARGET_FILE_DIR:microservice>)
add_custom_command(TARGET trainer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS} $<TARGET_FILE_DIR:trainer>)

file(GLOB OTHER_DLLS "dlls/*.dll")
add_custom_command(TARGET console POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OTHER_DLLS} $<TARGET_FILE_DIR:console>)
add_custom_command(TARGET microservice POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OTHER_DLLS} $<TARGET_FILE_DIR:microservice>)
add_custom_command(TARGET trainer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OTHER_DLLS} $<TARGET_FILE_DIR:trainer>)