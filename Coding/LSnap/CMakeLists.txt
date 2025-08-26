cmake_minimum_required(VERSION 3.10)
get_filename_component(CURRENT_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE) 
get_filename_component(CURRENT_DIR_NAME "${CURRENT_DIR_PATH}" NAME) 
project(LSnap)

set(CMAKE_CXX_STANDARD 11) 

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(UI_FILES 
    "lsnapview/lsnapdrawpanel.ui"
    "lsnapview/lsnapselectionactionbar.ui"
)

set(RCC_FILES 
    "res/icons.qrc"
)

set(HEADER_FILES 
    "lsnapapplicationcore.h"
    "globalhotkey.h"
    "lsnapview/lsnapoverlaywindow.h"
    "lsnapview/lsnapselectionactionbar.h"
    "lsnapview/lsnapdrawpanel.h"
    "lsnapview/lsnapdrawinglayer.h"
    "lsnapview/lsnapstickerwindow/stickerwindow.h"
    "lsnapview/lsnapstickerwindow/lsnapstickerwindowbase.h"
    "lsnapview/lsnapstickerwindow/lsnappicstickerwindow.h"
)

set(CPP_FILES 
    "main.cpp"
    "lsnapapplicationcore.cpp"
    "globalhotkey.cpp"
    "lsnapview/lsnapoverlaywindow.cpp"
    "lsnapview/lsnapselectionactionbar.cpp"
    "lsnapview/lsnapdrawpanel.cpp"
    "lsnapview/lsnapdrawinglayer.cpp"
    "lsnapview/lsnapstickerwindow/stickerwindow.cpp"
    "lsnapview/lsnapstickerwindow/lsnapstickerwindowbase.cpp"
    "lsnapview/lsnapstickerwindow/lsnappicstickerwindow.cpp"
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})
source_group("Form Files" FILES ${UI_FILES})
source_group("Resource Files" FILES ${RCC_FILES})
add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${CPP_FILES} ${UI_FILES} ${RCC_FILES})

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    LSnapGIF 
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/LSnapGIF")