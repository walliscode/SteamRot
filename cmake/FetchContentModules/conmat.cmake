include(FetchContent)

FetchContent_Declare(
  conmat
  GIT_REPOSITORY https://github.com/walliscode/conmat.git
  GIT_TAG main  # or specify a tag/commit
)

FetchContent_MakeAvailable(conmat)
