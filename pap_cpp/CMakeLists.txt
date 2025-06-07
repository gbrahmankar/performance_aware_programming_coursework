cmake_minimum_required(VERSION 3.14)
project(pap_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE HEADERS
    "${ROOT_PATH}/include/*.h"
    "${ROOT_PATH}/include/*.hpp"
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${ROOT_PATH}/source/*.cpp"
)

add_executable(pap_cpp ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${ROOT_PATH}
)

source_group(TREE "${ROOT_PATH}/include" PREFIX "include" FILES ${HEADERS})
source_group(TREE "${ROOT_PATH}/source" PREFIX "source" FILES ${SOURCES})

add_custom_target(run_raddbg
  COMMAND ${ROOT_PATH}/../other/raddebugger/build/raddbg ${CMAKE_CURRENT_BINARY_DIR}/pap_cpp.sln
  DEPENDS pap_cpp 
)

add_custom_target(run_vs_sln
  COMMAND start "" "${CMAKE_CURRENT_BINARY_DIR}/pap_cpp.sln"
  DEPENDS pap_cpp 
)
