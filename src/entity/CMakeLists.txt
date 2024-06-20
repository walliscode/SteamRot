
add_library(EntityManager
	EntityManager.cpp
	EntityMemoryPool.cpp
	Archetype.cpp)


target_include_directories(EntityManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/resources/generated_headers
)

target_link_libraries(EntityManager PUBLIC
	Component # nessary for using Component.cpp file for getHas()
	Component001
	flatbuffers
	utils
	Scene
	Assets
)

