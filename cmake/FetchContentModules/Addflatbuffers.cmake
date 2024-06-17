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

########## Generating Data Files ##########
# This next bit loops through a curated list (DATA_FILES) of data files that we want to generate binary and header files)


if(FLATBUFFERS_FOUND) # only runs if flatbuffers has been built

message("###### Generating Data Files ######")

######### Straight to headers (no binary)#########

set(HEADERS
    "component"
)

foreach(NAME ${HEADERS})
set(FBS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/schemas/${NAME}.fbs)
set(HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/generated_headers/${NAME}_generated.h)
if(EXISTS ${FBS_FILE})
	# output success message if they exist
	message(STATUS "Found FlatBuffers schema for ${NAME}")
	# Generate the header file
	add_custom_command(
	OUTPUT ${HEADER_FILE}
	COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE} -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/generated_headers -c ${FBS_FILE}
	DEPENDS ${FBS_FILE}
	COMMENT "Generating FlatBuffers header file from ${FBS_FILE}"
	)
	list(APPEND GENERATED_FILES ${HEADER_FILE})
    endif()
endforeach()

######### Scene Entity Configurations with shared schema #########

set(SCENE_CONFIG
    "SceneTest"
)
foreach(NAME ${SCENE_CONFIG})
set(FBS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/schemas/component.fbs")
set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/jsons/${NAME}.json)
set(BINARY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/binaries/${NAME}.bin)

if(EXISTS ${FBS_FILE} AND EXISTS ${JSON_FILE})
    # output success message if they exist
    message(STATUS "Found FlatBuffers schema and JSON file for ${NAME}")
    # Generate the binary file
    add_custom_command(
    OUTPUT ${BINARY_FILE}
    COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE} -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/binaries -b ${FBS_FILE} ${JSON_FILE}
    DEPENDS ${JSON_FILE}
    COMMENT "Generating binary FlatBuffers file from ${JSON_FILE}"
    )
    list(APPEND GENERATED_FILES ${BINARY_FILE})
    
endif()
endforeach()


######### Curated List of Data Files #########
set(DATA_FILES 
    "basic_data"
)
#############################################

foreach(NAME ${DATA_FILES})
set(FBS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/schemas/${NAME}.fbs)
set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/jsons/${NAME}.json)
set(BINARY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/binaries/${NAME}.bin)
set(HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/generated_headers/${NAME}_generated.h)

if(EXISTS ${FBS_FILE} AND EXISTS ${JSON_FILE})
    # output success message if they exist
    message(STATUS "Found FlatBuffers schema and JSON file for ${NAME}")
    # Generate the binary file
    add_custom_command(
    OUTPUT ${BINARY_FILE}
    COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE} -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/binaries -b ${FBS_FILE} ${JSON_FILE}
    DEPENDS ${JSON_FILE}
    COMMENT "Generating binary FlatBuffers file from ${JSON_FILE}"
    )
    list(APPEND GENERATED_FILES ${BINARY_FILE})

    # Generate the header file
    add_custom_command(
    OUTPUT ${HEADER_FILE}
    COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE} -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/generated_headers -c ${FBS_FILE}
    DEPENDS ${FBS_FILE}
    COMMENT "Generating FlatBuffers header file from ${FBS_FILE}"
    )

    list(APPEND GENERATED_FILES ${HEADER_FILE})
    
endif()
endforeach()

# by generating a custom target, we can ensure that the generated files are built. (https://cmake.org/cmake/help/book/mastering-cmake/chapter/Custom%20Commands.html#:~:text=add_custom_command%20has%20two%20main%20signatures,to%20add%20the%20custom%20command.)
add_custom_target(my_binary_data ALL 
    DEPENDS ${GENERATED_FILES})

endif()