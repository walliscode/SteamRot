include(FetchContent)
FetchContent_Declare(flatbuffers
    GIT_REPOSITORY https://github.com/google/flatbuffers.git
    GIT_TAG v25.2.10

    )
FetchContent_MakeAvailable(flatbuffers)
