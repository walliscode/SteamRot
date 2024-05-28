
add_library(EntityManager
	Entity.cpp
	EntityManager.cpp 
	EntityMemoryPool.cpp
)

target_include_directories(EntityManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(EntityManager PUBLIC
	Component001
)