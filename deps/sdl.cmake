include(FetchContent)

FetchContent_Declare(sdl
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.2.26
)

find_package(SDL3 QUIET)
find_package(SDL3_ttf QUIET)

if (SDL3_FOUND AND SDL3_ttf_FOUND)
	message(STATUS "[shiny] Using system SDL")
else ()
	message(STATUS "[shiny] Downloading SDL")
	set(SDL_AUDIO OFF)
	set(SDL_CAMERA OFF)
	set(SDL_DIALOG ON)
	set(SDL_GPU OFF)
	set(SDL_HAPTIC OFF)
	set(SDL_HIDAPI OFF)
	set(SDL_JOYSTICK OFF)
	set(SDL_POWER OFF)
	set(SDL_RENDER ON)
	set(SDL_SENSOR OFF)
	set(SDL_VIDEO ON)
	FetchContent_MakeAvailable(sdl)
	FetchContent_MakeAvailable(sdl_ttf)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
	SDL3_ttf::SDL3_ttf
)
