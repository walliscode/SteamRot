add_library(Assets
	Assets.cpp)

target_include_directories(Assets PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Assets PRIVATE 
    RESOURCES_PATH="${RESOURCES_PATH}")
