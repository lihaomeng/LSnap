cmake_minimum_required(VERSION 3.16)

project(LSnap VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        globalhotkey.cpp
        globalhotkey.h
        overlaywindow.h
        overlaywindow.cpp
        stickerwindow.h
        stickerwindow.cpp
        lview/selectionactionbar.h
        lview/selectionactionbar.cpp
        lview/gifpanel.h
        lview/gifpanel.cpp
        lview/drawpanel.h
        recorder/gifrecorder.h
        recorder/gifrecorder.cpp
        #recorder/gifsaveworker.cpp
        #recorder/gifsaveworker.h
        thirdparty/gif.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LSnap
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(LSnap SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(LSnap
            ${PROJECT_SOURCES}
        )
    endif()
endif()
target_include_directories(LSnap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(LSnap PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    target_link_libraries(LSnap PRIVATE user32)
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LSnap)
endif()
set_target_properties(LSnap PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LSnap
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LSnap)
endif()
