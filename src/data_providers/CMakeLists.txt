add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  FlatbuffersAssetDataProvider.cpp
  FlatbuffersFragmentDataProvider.cpp
  FlatbuffersSceneManagerDataProvider.cpp
  FlatbuffersSubscriberViewer.cpp
  provider_factory.cpp
)

target_include_directories(data_providers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_providers
  PUBLIC
  data_handlers
  components
  flatbuffers_headers
  events
)
