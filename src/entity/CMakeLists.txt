
add_library(EntityManager
	EntityManager.cpp
	EntityMemoryPool.cpp
	Archetype.cpp


target_include_directories(EntityManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(EntityManager PUBLIC
	Component # nessary for using Component.cpp file for getHas()
	Component001
)