cmake_minimum_required (VERSION 3.1)
project(cppSimulator)

set(CMAKE_CXX_STANDARD 11)


FIND_PACKAGE(PythonLibs)
if (PYTHONLIBS_FOUND)
    MESSAGE("Python LIB: " ${PYTHON_LIBRARIES})
    MESSAGE("Python INC: " ${PYTHON_INCLUDE_DIRS})
    link_libraries(${PYTHON_LIBRARIES})
else (NOT PYTHONLIBS_FOUND)
    MESSAGE("can't find python lib")
endif (PYTHONLIBS_FOUND)

#header only lib, just include headers
set(RAPIDJSON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include")

link_directories(${PYTHON_LIBRARIES})
include_directories(${PYTHON_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIRS})

include_directories(include)

file(GLOB HEADERS include/*.h)
file(GLOB HEADERS include/Ability/*.h)
file(GLOB HEADERS include/FeatureRepresentation/*.h)

aux_source_directory(src SRCS)
aux_source_directory(src/Ability SRCS)
aux_source_directory(src/FeatureRepresentation SRCS)
add_library(cppSimulator SHARED ${SRCS} ${HEADERS})


add_executable(testSimulator ${SRCS} ${HEADERS} ./test/test_main.cpp)

set_target_properties(testSimulator
    PROPERTIES 
    EXECUTABLE_OUTPUT_PATH ..
)

if (WIN32)
    set(LIB_SUFFIX ".pyd")
endif (WIN32)

if (UNIX)
    set(LIB_SUFFIX ".so")
endif(UNIX)

set_target_properties(cppSimulator
    PROPERTIES 
    PREFIX ""
    SUFFIX ${LIB_SUFFIX}
    LIBRARY_OUTPUT_DIRECTORY ..
)

if (WIN32)
    add_custom_command(TARGET cppSimulator POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE_DIR:cppSimulator>/cppSimulator${LIB_SUFFIX}"
    "${CMAKE_SOURCE_DIR}/cppSimulator${LIB_SUFFIX}"              
    )
endif (WIN32)
