

add_executable(steamrot_tests 
	"systems/game/GameEngine.test.cpp"
)

target_link_libraries(steamrot_tests
	PRIVATE
		Catch2::Catch2WithMain
		GameEngine
		EntityManager
)
include(CTest)
add_test(NAME steamrot_tests COMMAND steamrot_tests)	
include(Catch)

#catch_discover_tests(steamrot_tests)
