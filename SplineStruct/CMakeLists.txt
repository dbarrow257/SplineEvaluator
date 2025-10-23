set(HEADERS SplineSet.h SplineSet_Eigen.h)

add_library(SplineStruct SHARED SplineSet.cpp SplineSet_Eigen.cpp)

target_link_libraries(SplineStruct yaml-cpp SplineEvaluatorCompilerOptions ROOT::Core ROOT::Hist Eigen)

target_include_directories(SplineStruct PUBLIC
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

set_target_properties(SplineStruct PROPERTIES
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME SplineStruct)

install(TARGETS SplineStruct
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/SplineStruct)

add_library(SplineStruct::SplineStruct ALIAS SplineStruct)
