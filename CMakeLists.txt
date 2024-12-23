#https://github.com/meemknight/cmakeSetup
#https://github.com/meemknight/openglConfiguration

cmake_minimum_required(VERSION 3.16)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # We want to use the static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(mygame)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2)			# Window opener
add_subdirectory(thirdparty/glad)				# Opengl loader
add_subdirectory(thirdparty/stb_image)			# Loading images
add_subdirectory(thirdparty/stb_truetype)		# Loading ttf font files
add_subdirectory(thirdparty/raudio)				# Audio
add_subdirectory(thirdparty/glm)				# Math
add_subdirectory(thirdparty/imgui-docking)		# UI


# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


# Define MY_INCLUDES to be a list of all the include files for my game 
file(GLOB_RECURSE MY_INCLUDES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)

# This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
# Uncomment this line to setup the ASSETS_PATH macro to the final assets directory when you share the game
#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") 


target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


if(MSVC) # If using the VS compiler...

	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "/ZI")
	target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "/INCREMENTAL")

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	#remove console
	#set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	
	set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
	set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

	# Debug stuff: uncomment to get verbose output (ChatGPT helps a lot to decypher it)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zi /diagnostics:caret")
	#set(CMAKE_CXX_STANDARD 17)
	#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

endif()


target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_INCLUDES})


target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw 
	glad stb_image stb_truetype raudio imgui)

