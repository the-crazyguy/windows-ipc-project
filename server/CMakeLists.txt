cmake_minimum_required(VERSION 3.10.0)
project(ipc_server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(ipc_server main.cpp)

# Add libraries
add_library(DebugModule source/debug_module.cpp)
add_library(SmartHandle source/SmartHandle.cpp)

set(LIBRARIES_TO_LINK
    DebugModule
    SmartHandle
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIBRARIES_TO_LINK})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
