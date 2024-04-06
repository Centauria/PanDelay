cmake_minimum_required(VERSION 3.27)
project(PanDelay C)

set(CMAKE_C_STANDARD 11)

set(FFMPEG_DIR /usr/local/Cellar/ffmpeg/6.0)
include_directories(${FFMPEG_DIR}/include/)
link_directories(${FFMPEG_DIR}/lib/)

include_directories(PanDelay)

add_executable(PanDelay
    PanDelay/ff.c
    PanDelay/ff.h
    PanDelay/main.c
    PanDelay/PanDelay.c
    PanDelay/PanDelay.h)
target_link_libraries(PanDelay
    avcodec
    avformat
    avutil)
