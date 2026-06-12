add_library(spatial_analysis
    grab_analysis.cpp

)

target_include_directories(spatial_analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(spatial_analysis PUBLIC
      types

)
