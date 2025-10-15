project(
    tuipod
    VERSION 0.1
    LANGUAGES CXX
)

add_executable(
    ${PROJECT_NAME}
    main.cpp

    albumMenu.cpp
    albumMenu.h

    artistMenu.cpp
    artistMenu.h

    binaryButton.cpp
    binaryButton.h

    mainMenu.cpp
    mainMenu.h

    nowPlaying.cpp
    nowPlaying.h

    playbackControls.cpp
    playbackControls.h

    pressedButton.cpp
    pressedButton.h

    queue.cpp
    queue.h

    seekbar.cpp
    seekbar.h

    trackMenu.cpp
    trackMenu.h

    uiLoop.cpp
    uiLoop.h
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    clipod
    ftxui::screen
    ftxui::dom
    ftxui::component
    cxxopts
    ctrlc
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "tuipod")
