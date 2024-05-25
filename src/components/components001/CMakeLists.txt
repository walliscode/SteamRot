
add_library(Component
	Component.cpp
)

target_include_directories(Component PUBLIC # Public is necessary for main to have access to GameEngine.h
	${CMAKE_CURRENT_SOURCE_DIR}/include
)


#target_link_libraries( PUBLIC
#	sfml-graphics
#)
