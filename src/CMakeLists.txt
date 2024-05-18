

# Add source to exe
add_executable(SteamRot "main.cpp" "main.h")

if(WIN32)
    add_custom_command(
        TARGET SteamRot
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:SteamRot>
        VERBATIM)
endif()

# TODO: Add tests and install targets if needed.
