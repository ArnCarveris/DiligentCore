cmake_minimum_required (VERSION 3.6)

if(PLATFORM_WIN32)
	add_subdirectory(glew)
endif()

if(PLATFORM_ANDROID)
	add_subdirectory(Android)
endif()
