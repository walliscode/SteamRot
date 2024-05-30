
add_library(Entity
	Entity.cpp
 "include/EntityPool.h")

target_include_directories(Entity PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Entity PUBLIC
	Component
)