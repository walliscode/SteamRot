
add_library(Component
	Component.cpp
)

target_include_directories(Component PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
