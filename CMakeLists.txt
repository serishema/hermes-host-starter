cmake_minimum_required(VERSION 3.18)
project (hermes-host-starter C CXX)


set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# this has to be done because the hermes build system expected that jsi.lib would be a static library
# but building it as a static library on windows doesn't work
if (WIN32)
    install(FILES "${PROJECT_SOURCE_DIR}/third_party/hermes/build/jsi/jsi.lib" DESTINATION "${PROJECT_BINARY_DIR}")
endif()

add_executable(hermes-host-starter src/main.cpp src/nativeobject.cpp)


# hermes requires C++17. It will not work correctly with eairlier language standards
set_property(TARGET hermes-host-starter PROPERTY CXX_STANDARD 17)
set_property(TARGET hermes-host-starter PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET hermes-host-starter PROPERTY CXX_EXTENSIONS Off)
target_include_directories(hermes-host-starter PUBLIC "${PROJECT_SOURCE_DIR}/lib/hermes/include")
target_link_directories(hermes-host-starter PUBLIC "${PROJECT_SOURCE_DIR}/lib/hermes/lib")
if (WIN32)
target_link_libraries(hermes-host-starter hermes jsi)
else()
#on linux jsi is statically linked into libhermies.so 
#the developers of hermes intended that this happen on windows too, but it doesn't
target_link_libraries(hermes-host-starter hermes)
endif()

if (WIN32) 
	install(FILES "${PROJECT_SOURCE_DIR}/lib/hermes/bin/hermes.dll" DESTINATION "${PROJECT_BINARY_DIR}")
    install(FILES "${PROJECT_SOURCE_DIR}/third_party/hermes/build/jsi/jsi.dll" DESTINATION "${PROJECT_BINARY_DIR}")
else()
    install(FILES "${PROJECT_SOURCE_DIR}/lib/bin/hermes.so" DESTINATION "${PROJECT_BINARY_DIR}")
endif() 
