
add_subdirectory(systems)
add_subdirectory(components)

add_executable(SteamRot main.cpp )

target_link_libraries(SteamRot PRIVATE
    GameEngine
)

# this dll file is needed for the audio module, so currently not needed but will be left in for now to prevent us searching for it when time comes for audio
if(WIN32)
    add_custom_command(
        TARGET SteamRot
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:SteamRot>
        VERBATIM)
endif()



