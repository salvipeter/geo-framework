cmake_minimum_required(VERSION 3.2)
project(geo-framework)

option(WITH_JET_FITTING "Use jet fitting for normals" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenGL_GL_PREFERENCE LEGACY)

if(WITH_JET_FITTING)
  find_package(Eigen3 3.3 REQUIRED NO_MODULE)
  find_package(CGAL REQUIRED NO_MODULE)
endif(WITH_JET_FITTING)
find_package(OpenGL REQUIRED)
find_package(OpenMesh REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core OpenGL OpenGLWidgets Widgets Xml)
find_package(QGLViewer REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -fPIC -Wno-reorder")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(geo_HEADERS # headers processed by MOC
  viewer.hh
  window.hh
  )

set(CMAKE_AUTORCC ON)
qt6_wrap_cpp(geo_HEADERS_MOC ${geo_HEADERS})

add_definitions(${Qt6Widgets_DEFINITIONS})

if(WITH_JET_FITTING)
  add_definitions(-DUSE_JET_FITTING)
endif()

include_directories(
  .
  ${CGAL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${OpenMesh_INCLUDE_DIR}
  ${Qt6OpenGL_INCLUDE_DIRS}
  ${Qt6Widgets_INCLUDE_DIRS}
  ${Qt6Xml_INCLUDE_DIRS}
  ${QGLVIEWER_INCLUDE_DIR}
  )

add_executable(geo-framework
  ${geo_HEADERS_MOC}
  textures.qrc
  bezier.cc
  main.cc
  mesh.cc
  object.cc
  viewer.cc
  visualization.cc
  window.cc
  )

if(WITH_JET_FITTING)
  target_sources(geo-framework PRIVATE jet-wrapper.cc)
endif(WITH_JET_FITTING)

if(UNIX)
  add_custom_command(OUTPUT tags
    COMMAND etags *.hh *.cc
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
  target_sources(geo-framework PRIVATE tags)
endif(UNIX)  

target_link_libraries(geo-framework PRIVATE
  ${OPENGL_LIBRARIES}
  ${OPENMESH_LIBRARIES}
  Qt6::Core
  Qt6::OpenGL
  Qt6::OpenGLWidgets
  Qt6::Widgets
  Qt6::Xml
  ${QGLVIEWER_LIBRARY}
  )
