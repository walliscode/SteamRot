# as flatc.exe is in bin folder, flatbuffers cmakefile needs to be pointed at it
set(FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_BINARY_DIR}/flatbuffers-build/flatc.exe)
message("FLATBUFFERS_FLATC_EXECUTABLE: ${FLATBUFFERS_FLATC_EXECUTABLE}")


########## Generating Data Files ##########
# This next bit loops through a curated list (DATA_FILES) of data files that we want to generate binary and header files)


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
    "mainMenu"
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
    "load_fonts"
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
    DEPENDS 
    ${GENERATED_FILES}
    flatc
    )
