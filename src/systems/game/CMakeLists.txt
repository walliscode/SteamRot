

add_library(GameEngine
	GameEngine.cpp
)

target_include_directories(GameEngine PUBLIC # Public is necessary for main to have access to GameEngine.h
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(GameEngine PUBLIC
	sfml-graphics
	SceneManager
)
