add_executable(test_scene_change
scene_change.test.cpp
)

target_link_libraries(test_scene_change
PRIVATE
Catch2::Catch2WithMain
data_handlers
systems
)


catch_discover_tests(test_scene_change)
