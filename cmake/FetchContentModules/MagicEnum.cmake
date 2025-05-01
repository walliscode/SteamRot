include(FetchContent)

include(FetchContent)
FetchContent_Declare(
  magic_enum  # name of the target
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)
