
add_library(Entity
	Entity.cpp
)

target_include_directories(Entity PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_library(EntityManager
	EntityManager.cpp
)
target_include_directories(EntityManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_library(EntityMemoryPool
	EntityMemoryPool.cpp
)
target_include_directories(EntityMemoryPool PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(EntityMemoryPool PUBLIC
	Component001
)