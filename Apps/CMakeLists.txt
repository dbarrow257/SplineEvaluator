add_custom_target(Apps)

foreach(app
        ExampleSpline
      )

      add_executable(${app} ${app}.cpp)
      add_dependencies(Apps ${app})
      target_link_libraries(${app} SplineEvaluator::All)
      install(TARGETS ${app} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endforeach(app)
