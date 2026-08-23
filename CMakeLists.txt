cmake_minimum_required(VERSION 3.20)
project(Mesopotamia C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB SOURCES "src/*.cpp")
set(RESOURCES "")

if(WIN32)
    list(APPEND RESOURCES "src/resource.rc")
endif()

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(${PROJECT_NAME} ${SOURCES})
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index")
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${RESOURCES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(EMSCRIPTEN)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_SDL_IMAGE=2"
        "-sUSE_SDL_MIXER=2"
    )
    
    target_link_options(${PROJECT_NAME} PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_SDL_IMAGE=2"
        "-sUSE_SDL_MIXER=2"
        "-sSDL2_IMAGE_FORMATS=[\"png\"]"
        "-sSDL2_MIXER_FORMATS=[\"mp3\"]"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@assets"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sEXIT_RUNTIME=1"
    )
else()
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)

	target_link_libraries(${PROJECT_NAME} PRIVATE 
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2-static>
        $<TARGET_NAME_IF_EXISTS:SDL2_image::SDL2_image>
        $<TARGET_NAME_IF_EXISTS:SDL2_image::SDL2_image-static>
        $<TARGET_NAME_IF_EXISTS:SDL2_mixer::SDL2_mixer>
        $<TARGET_NAME_IF_EXISTS:SDL2_mixer::SDL2_mixer-static>
    )

	target_link_options(${PROJECT_NAME} PRIVATE
		"-static"
		"-static-libgcc" 
		"-static-libstdc++"
		"-s"
		"-Wl,--gc-sections"
	)
endif()