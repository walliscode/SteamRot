add_library(configuration
  FlatbuffersUserPreferencesProvider.cpp
  FlatbuffersSaveDataProvider.cpp
)

target_include_directories(configuration PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(configuration PUBLIC
  types
  data_providers

)
