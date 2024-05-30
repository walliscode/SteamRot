

add_executable(steamrot_tests 
	"systems/game/GameEngine.test.cpp"
 "systems/entity/EntityPool.test.cpp")

target_link_libraries(steamrot_tests
	PRIVATE
		Catch2::Catch2WithMain
		GameEngine
		Entity
)
include(CTest)
include(Catch)
catch_discover_tests(steamrot_tests)
