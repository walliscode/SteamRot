

add_executable(steamrot_tests 
	"systems/game/GameEngine.test.cpp"
	"entity/EntityMemoryPool.test.cpp" 
	"entity/EntityManager.test.cpp"
    "systems/scenes/Scene.test.cpp" 
	"components/assets/Assets.test.cpp")

target_include_directories(steamrot_tests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(steamrot_tests
	PRIVATE
		Catch2::Catch2WithMain
		GameEngine
		EntityManager
		Scene
)
include(CTest)
include(Catch)
catch_discover_tests(steamrot_tests)
