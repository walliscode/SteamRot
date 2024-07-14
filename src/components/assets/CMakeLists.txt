add_library(Assets
	Assets.cpp)

target_include_directories(Assets PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/resources/generated_headers
)

add_dependencies(Assets
	my_binary_data) # the flatc exectuable and any required flatbuffers headers need to be created first

target_link_libraries(Assets PRIVATE
	flatbuffers
	sfml-graphics
	utils)

target_compile_definitions(Assets PRIVATE 
    RESOURCES_PATH="${RESOURCES_PATH}")
