#================================================================================================
#Project details

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

#KS: Store some handy cmake functions
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/SplineEvaluatorUtils.cmake)

set(SPLINEEVALUATOR_VERSION 1.0.0)

project(SPLINEEVALUATOR VERSION ${SPLINEEVALUATOR_VERSION} LANGUAGES CXX)

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
    "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

#================================================================================================

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

########################################################

#Custom variable sanitisation
add_library(SplineEvaluatorCompilerOptions INTERFACE)

if(NOT DEFINED SplineEvaluator_Compiler_Flags)
  set(SplineEvaluator_Compiler_Flags "")
endif()

separate_arguments(SplineEvaluator_Compiler_Flags)

cmessage(STATUS "Set compiler options: ${SplineEvaluator_Compiler_Flags}")
add_compile_options(${SplineEvaluator_Compiler_Flags})

#================================================================================================
#Setup Deps

set(YAML_CPP_VERSION 0.8.0)
CPMAddPackage(
    NAME yaml-cpp
    VERSION ${YAML_CPP_VERSION}
    GITHUB_REPOSITORY "jbeder/yaml-cpp"
    GIT_TAG "${YAML_CPP_VERSION}"
    OPTIONS
      "YAML_CPP_INSTALL ON"
      "YAML_CPP_BUILD_TESTS OFF"
      "YAML_CPP_BUILD_CONTRIB OFF"
      "YAML_BUILD_SHARED_LIBS ON"
)

if(NOT TARGET yaml-cpp::yaml-cpp)
  cmessage(FATAL_ERROR "SplineEvaluator Expected dependency target: yaml-cpp::yaml-cpp")
endif()

find_package(ROOT 6.18 REQUIRED)

CPMAddPackage(
  NAME Eigen
  VERSION 3.4.0
  URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
  DOWNLOAD_ONLY YES
)
if(Eigen_ADDED)
  add_library(Eigen INTERFACE IMPORTED)
  target_include_directories(Eigen INTERFACE ${Eigen_SOURCE_DIR})
endif()

#================================================================================================
#Subdirectories to add

add_subdirectory(SplineStruct)
add_subdirectory(Apps)

#================================================================================================

set(LIBDEST ${CMAKE_INSTALL_PREFIX}/lib)

configure_file(cmake/Templates/setup.SplineEvaluator.sh.in "${PROJECT_BINARY_DIR}/setup.SplineEvaluator.sh" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/setup.SplineEvaluator.sh" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set_target_properties(SplineEvaluatorCompilerOptions PROPERTIES EXPORT_NAME CompilerOptions)
install(TARGETS SplineEvaluatorCompilerOptions
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/)

#================================================================================================

#This is to export the target properties of SPLINEEVALUATOR
#Anything that links to "SPLINEEVALUATOR" will get all of these target properties
add_library(SplineEvaluator INTERFACE)
target_link_libraries(SplineEvaluator INTERFACE SplineStruct SplineEvaluatorCompilerOptions)
set_target_properties(SplineEvaluator PROPERTIES EXPORT_NAME All)

install(TARGETS SplineEvaluator
  EXPORT SplineEvaluator-targets
  LIBRARY DESTINATION lib/)

install(EXPORT SplineEvaluator-targets
  FILE SplineEvaluatorTargets.cmake
  NAMESPACE SplineEvaluator::
  DESTINATION ${CMAKE_INSTALL_PREFIX}/
)

add_library(SplineEvaluator::All ALIAS SplineEvaluator)

############################  Install  ####################################
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/SplineEvaluatorConfig.cmake.in ${CMAKE_BINARY_DIR}/SplineEvaluatorConfig.cmake
  INSTALL_DESTINATION
  /this/is/ignored/for/some/reason/thanks/kitware
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(${CMAKE_BINARY_DIR}/SplineEvaluatorConfigVersion.cmake
  VERSION ${SplineEvaluator_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES
  ${CMAKE_BINARY_DIR}/SplineEvaluatorConfig.cmake
  ${CMAKE_BINARY_DIR}/SplineEvaluatorConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/SplineEvaluator)

include(SplineEvaluator-config)
