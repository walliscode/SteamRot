
add_library(EntityManager
	EntityManager.cpp
	EntityMemoryPool.cpp
	Archetype.cpp)


target_include_directories(EntityManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/resources/generated_headers
)

add_dependencies(EntityManager
	my_binary_data  # the flatc exectuable and any required flatbuffers headers need to be created first

)

target_link_libraries(EntityManager PUBLIC
	Component # nessary for using Component.cpp file for getHas()
	Component001
	flatbuffers
	Scene
	AssetManager
)

