cmake_minimum_required(VERSION 3.11)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-g")

project(metalTest)

find_package(PkgConfig REQUIRED)
find_package(JPEG REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp-extensions
)

link_directories(/usr/local/lib)

set(src
    src/metalapp/shaderset.cpp
    src/metalapp/texture.cpp
    src/metalapp/vertex.cpp
    src/metalapp/camera.cpp
    src/metalapp/textdraw.cpp
    src/metalapp/simple2d.cpp
    src/metalapp/simple3d.cpp
    src/metalapp/impl.cpp
    src/metalapp/app.cpp
    src/gamepad.mm
    src/testloop.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${src})

#target_compile_options(padLink PUBLIC ${SDL2_CFLAGS_OTHER})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    "-framework Metal"
    "-framework Foundation"
    "-framework Cocoa"
    "-framework CoreGraphics"
    "-framework CoreText"
    "-framework MetalKit"
    "-framework GameController"
    JPEG::JPEG
    ${libs})
