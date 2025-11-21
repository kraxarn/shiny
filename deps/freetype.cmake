include(FetchContent)

FetchContent_Declare(freetype
	GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
	GIT_TAG VER-2-14-1
)

find_package(Freetype QUIET)

if (Freetype_FOUND)
	message(STATUS "Using system Freetype")
else ()
	message(STATUS "Downloading Freetype")
	set(FT_DISABLE_ZLIB ON)
	set(FT_DISABLE_BZIP2 ON)
	set(FT_DISABLE_PNG ON)
	set(FT_DISABLE_BROTLI ON)
	set(FT_DISABLE_HARFBUZZ ON)
	set(FT_REQUIRE_HARFBUZZ OFF)
	FetchContent_MakeAvailable(freetype)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
	Freetype::Freetype
)