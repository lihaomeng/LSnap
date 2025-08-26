cmake_minimum_required(VERSION 3.10)

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
endif()

project(LSnapMain)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

set(EXCLUDE_DIRS ".git")

set(CURRENT_DIR ${CMAKE_SOURCE_DIR})

file(GLOB ALL_ITEMS RELATIVE ${CURRENT_DIR} ${CURRENT_DIR}/*)
message(STATUS "ALL_ITEMS: ${ALL_ITEMS}")

set(DIRECTORIES "")
foreach(ITEM ${ALL_ITEMS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${ITEM})
        set(EXCLUDE_FLAG OFF)
        foreach(EXCLUDE_DIR ${EXCLUDE_DIRS})
            if(${ITEM} STREQUAL ${EXCLUDE_DIR})
                set(EXCLUDE_FLAG ON)
                break()
            endif()
        endforeach()
        if(NOT EXCLUDE_FLAG)
            list(APPEND DIRECTORIES ${ITEM})
            add_subdirectory(${CURRENT_DIR}/${ITEM})
        endif()
    endif()
endforeach()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LSnap)
