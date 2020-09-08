cmake_minimum_required(VERSION 3.5)

project(conf_PTZ LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    # -Wsign-conversion
    # -Wsign-promo
)

add_link_options(
    -no-pie
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5SerialPort  REQUIRED)
find_package(Qt5Network  REQUIRED)

qt5_add_resources(RCC_SOURCES resources/favicon.qrc)

set(SOURCES_PATH src)
set(SOURCE_LISTS ${SOURCES_PATH}/main.cpp
    ${SOURCES_PATH}/mainwindow.cpp
    ${SOURCES_PATH}/serial_info.cpp
    ${SOURCES_PATH}/icommunication.cpp
    ${SOURCES_PATH}/serial_com.cpp
    ${SOURCES_PATH}/udp_com.cpp
    ${SOURCES_PATH}/tcp_com.cpp
    ${SOURCES_PATH}/communication_front.cpp
    ${SOURCES_PATH}/communication_window.cpp
    ${SOURCES_PATH}/setting_window.cpp
    ${SOURCES_PATH}/control_window.cpp
    ${SOURCES_PATH}/pelcod_protocol.cpp
)

file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp src/*.h)

add_custom_target (
    format
    COMMAND clang-format
    -style=file
    -i
    ${ALL_SOURCE_FILES}
)

include_directories(src)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_LISTS} ${RCC_SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::SerialPort Qt5::Network)

if (WIN32)
    execute_process(COMMAND python semver.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/scripts)
else()
    execute_process(COMMAND python3 semver.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/scripts)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-Wl,-rpath,./"
)
