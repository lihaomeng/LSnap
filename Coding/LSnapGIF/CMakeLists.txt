cmake_minimum_required(VERSION 3.10)
project(LSnapGIF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED Concurrent)

add_library(${PROJECT_NAME} SHARED
    "gifrecorder.h"
    "gifsaveworker.h"
    "gifrecorder.cpp"
    "gifsaveworker.cpp"
    "lsnapgif_export.h"
    "lsnapgifpanel.h"
    "lsnapgifpanel.cpp"
    "lsnapgifpanel.ui"
)

# target_sources(${PROJECT_NAME} PRIVATE
#     "gifrecorder.h"
#     "gifsaveworker.h"
#     "gifrecorder.cpp"
#     "gifsaveworker.cpp"
#     "lsnapgif_export.h"
# )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Concurrent
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    #GIFRECORDER_DLL
    #GIFSAVEWORKER_DLL
    LSNAPGIF_DLL
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "LSnapGIF"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)