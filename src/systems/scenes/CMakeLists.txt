add_library(Scene
Scene.cpp)

target_include_directories(Scene PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Scene PUBLIC
	EntityManager)