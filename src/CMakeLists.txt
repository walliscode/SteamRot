


# Add source to this project's executable.
add_executable(SteamRot "src.cpp" "src.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SteamRot PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
    add_custom_command(
        TARGET SteamRot
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:SteamRot>
        VERBATIM)
endif()

# TODO: Add tests and install targets if needed.
