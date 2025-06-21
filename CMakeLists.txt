cmake_minimum_required(VERSION 3.28)
# project(CMakeSFMLProject LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    URL file://${CMAKE_SOURCE_DIR}/3rd/SFML-2.6.1.zip
    )
Fetchcontent_MakeAvailable(SFML)

# add_executable(main_src/main_2.cc)  
# target_compile_features(main PRIVATE cxx_std_17)
# target_link_libraries(main PRIVATE sfml-graphics sfml-window sfml-system sfml-audio sfml-network)


project(SnakeGame LANGUAGES CXX)
set(SOURCES
    main.cpp
    Fruit.cpp
    Game.cpp
    GameOverScreen.cpp
    GameScreen.cpp
    MenuScreen.cpp
    Screen.cpp
    Snake.cpp
    SnakeNode.cpp
    EnemySnake.cpp
)
add_executable(SnakeGame ${SOURCES})

target_link_libraries(SnakeGame PRIVATE sfml-graphics sfml-window sfml-system sfml-audio sfml-network)
