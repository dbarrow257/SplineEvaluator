set(HEADERS SetHandler.h SetHandler.h)

add_library(SetHandler SHARED SetHandler.cpp SetHandler.cpp)
target_link_libraries(SetHandler InputHandler SplineSet)

target_include_directories(SetHandler PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(SetHandler PROPERTIES
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME SetHandler)

install(TARGETS SetHandler
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/SetHandler)

add_library(SetHandler::SetHandler ALIAS SetHandler)
