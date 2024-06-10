
add_library(EntityMemoryPool
	EntityMemoryPool.cpp
	Archetype.cpp
)

target_include_directories(EntityMemoryPool PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(EntityMemoryPool PUBLIC
	Component001
)