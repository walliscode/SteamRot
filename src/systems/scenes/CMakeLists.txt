add_library(Scene
Scene.cpp
Container.cpp
ContainerManager.cpp
SceneMainMenu.cpp)

target_include_directories(Scene PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Scene PUBLIC
	EntityManager
	GameEngine
	Actions
	utils
	
	)

message("FB_BINARIES_PATH  = ${FB_BINARIES_PATH}")

target_compile_definitions(Scene PRIVATE
    FB_BINARIES_PATH="${FB_BINARIES_PATH}")

add_library(SceneManager SceneManager.cpp)

target_include_directories(SceneManager PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SceneManager PUBLIC
	Scene
	sfml-graphics
	nlohmann_json
	AssetManager
	)