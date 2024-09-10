add_library(AssetManager
	AssetManager.cpp)

target_include_directories(AssetManager PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/resources/generated_headers
)

add_dependencies(AssetManager
	my_binary_data) # the flatc exectuable and any required flatbuffers headers need to be created first

target_link_libraries(AssetManager PRIVATE
	flatbuffers
	sfml-graphics
	utils)

target_compile_definitions(AssetManager PRIVATE 
    RESOURCES_PATH="${RESOURCES_PATH}")
