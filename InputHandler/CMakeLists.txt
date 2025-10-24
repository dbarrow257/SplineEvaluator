set(HEADERS InputHandler.h InputHandler_SingleRootDir.h)

add_library(InputHandler SHARED InputHandler.cpp InputHandler_SingleRootDir.cpp)

target_link_libraries(InputHandler yaml-cpp SplineEvaluatorCompilerOptions ROOT::Core)

target_include_directories(InputHandler PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(InputHandler PROPERTIES
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME InputHandler)

install(TARGETS InputHandler
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/InputHandler)

add_library(InputHandler::InputHandler ALIAS InputHandler)
