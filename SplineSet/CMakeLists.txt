set(HEADERS SplineSet.h SplineSet_Eigen.h)

add_library(SplineSet SHARED SplineSet.cpp SplineSet_Eigen.cpp)

target_link_libraries(SplineSet yaml-cpp SplineEvaluatorCompilerOptions ROOT::Core ROOT::Hist Eigen)

target_include_directories(SplineSet PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS yaml-cpp
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/yaml-cpp
)

install(DIRECTORY ${yaml-cpp_SOURCE_DIR}/include/
  DESTINATION include/
)

set_target_properties(SplineSet PROPERTIES
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME SplineSet)

install(TARGETS SplineSet
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/SplineSet)

add_library(SplineSet::SplineSet ALIAS SplineSet)
