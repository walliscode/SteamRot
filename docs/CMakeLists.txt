# build docs target for doxygen documentation

find_package(Doxygen
  REQUIRED dot)

set(DOXYGEN_HAVE_DOT YES)

if(DOXYGEN_FOUND)
  doxygen_add_docs(
    docs
    ${PROEJCT_SOURCE_DIR}
    COMMENT "Generating documentation for SteamRot"
  )
endif()
