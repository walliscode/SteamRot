
add_library(Component
	Component.cpp
)

target_include_directories(Component PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(Component001
	CTransform.cpp 
	CMeta.cpp
	CText.cpp
)
target_include_directories(Component001 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Component001 PUBLIC
	sfml-graphics
)