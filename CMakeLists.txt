cmake_minimum_required(VERSION 3.21)
project(spotify_dl_gui)

set(CMAKE_CXX_STANDARD 17)

add_executable(spotify_dl_gui src/main.cpp src/window.cpp src/window.h src/downloader.cpp src/downloader.h src/exec-stream.cpp src/exec-stream.h)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${GTKMM_LIBRARIES})