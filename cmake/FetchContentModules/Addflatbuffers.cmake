##### N.B. ######
# if you are building flatbufffers for the first time, then FLATBUFFERS_FOUND will be false.
# run the build and then run the cmake again. FLATBUFFERS_FOUND should be true.
# This should trigger it to convert .fbs files to .h files and json files to binar

Include(FetchContent)

FetchContent_Declare(
  flatbuffers
  GIT_REPOSITORY https://github.com/google/flatbuffers
  GIT_TAG        v24.3.25 # or a later release
)

FetchContent_MakeAvailable(flatbuffers)

# these variables are necessary as we are using fetch content to get flatbuffers.

list(APPEND CMAKE_MODULE_PATH ${flatbuffers_SOURCE_DIR}/CMake)
set(FLATBUFFERS_INCLUDE_DIR ${flatbuffers_SOURCE_DIR}/include) # allows find_path() in FindFlatBuffers.cmake to find the flatbuffers headers


find_package(FlatBuffers) # needs to be FlatBuffers, not flatbuffers as FindFlatBuffers.cmake is calling FlatBuffers

message(STATUS "FLATBUFFERS_FOUND: ${FLATBUFFERS_FOUND}")

if(FLATBUFFERS_FOUND)
	# set variables


	if(COMMAND build_flatbuffers)
	  message(STATUS "##### build_flatbuffers found, converting .fbs to headers and .json to binaries #####")

	else()
	  message(STATUS "The build_flatbuffers function is not defined.")
	endif()

endif()


