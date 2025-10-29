project(
    clipod
    VERSION 0.1
    LANGUAGES CXX
)

set(HPATH ${CMAKE_SOURCE_DIR}/include/clipod)

add_library(
    ${PROJECT_NAME}

    ${HPATH}/album.h
    ${HPATH}/artist.h
    ${HPATH}/database.h
    ${HPATH}/databaseUtil.h
    ${HPATH}/decoder.h
    ${HPATH}/playback.h
    ${HPATH}/queue.h
    ${HPATH}/soundBuffer.h
    ${HPATH}/timer.h
    ${HPATH}/track.h

    album.cpp
    artist.cpp
    database.cpp
    databaseUtil.cpp
    decoder.cpp
    playback.cpp
    queue.cpp
    soundBuffer.cpp
    timer.cpp
    track.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${taglib_SOURCE_DIR}/taglib
    ${taglib_SOURCE_DIR}/taglib/toolkit
    ${taglib_SOURCE_DIR}/taglib/ape
    ${taglib_SOURCE_DIR}/taglib/mpeg
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v1
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2/frames
    ${taglib_SOURCE_DIR}/bindings/c/
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${FFMPEG_LIBRARY_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    Pal::Sigslot
    spdlog::spdlog
    SQLiteCpp
    modern-uuid::modern-uuid-static
    OpenAL
    tag
    avdevice
    avformat
    avfilter
    avcodec
    swresample
    swscale
    avutil
)
