cmake_minimum_required(VERSION 3.16)
project(execution_graph)

# Global compiler options
# Note that by default, Debug build will have:
#  /Od   [optimization disabled] 
#  /RTC1 [runtime checks for uninit variables and stack usage]
#  /MDd  [use debug runtime]
# Release build will have:
#  /O2   [optimization for max speed]
#  /Ob2  [full inline expansion]
#  /MD   [use release runtime]
add_compile_options(
	/W4							# Warning level 4
	/WX							# Enable warnings as errors
	/Gy							# Enable function level linking
	/Gw							# Optimize Global Data
	/GR-						# Disable Run-Time Type Information
	/GS-						# Disable Security Check
	/bigobj						# Increase number of sections in object files
	/Oi							# Enable intrinsics
	#$<$<CONFIG:Debug>:/Ob1>		# Enable only __inline (\Ob1) for Debug Build
	/fp:fast					# Enable fast floating point arithmetics by default
	/permissive-
)

# Global linker options
add_link_options(
	/WX				# Enable warnings as errors
)

file(GLOB_RECURSE TARGET_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "src/*")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Sources" FILES ${TARGET_SOURCES})
add_executable(execution_graph ${TARGET_SOURCES})

find_package(cpp-taskflow)
find_package(rttr)
target_link_libraries(execution_graph PRIVATE cpp-taskflow::cpp-taskflow rttr::rttr)

target_compile_options(execution_graph PRIVATE /wd4201 /wd4267)
