find_package(Qt6 COMPONENTS Core Gui OpenGL Widgets Xml QUIET)
if(NOT Qt6_FOUND)
  message("Qt6 not found. Install it and set Qt6_DIR accordingly")
  if (WIN32)
    message("  In Windows, Qt6_DIR should be something like C:/Qt/6.1/msvc2017_64_opengl/lib/cmake/Qt6")
  endif()
  return()
endif()

find_path(QGLVIEWER_INCLUDE_DIR qglviewer.h
    /usr/include/QGLViewer
    /opt/local/include/QGLViewer
    /usr/local/include/QGLViewer
    /sw/include/QGLViewer
    ENV QGLVIEWERROOT
  )

find_library(QGLVIEWER_LIBRARY_RELEASE
  NAMES qglviewer QGLViewer qglviewer-qt6 QGLViewer-qt6
  PATHS /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV QGLVIEWERROOT
        ENV LD_LIBRARY_PATH
        ENV LIBRARY_PATH
  PATH_SUFFIXES QGLViewer QGLViewer/release
)
find_library(QGLVIEWER_LIBRARY_DEBUG
  NAMES dqglviewer dQGLViewer dqglviewer-qt6 dQGLViewer-qt6 QGLViewerd2
  PATHS /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV QGLVIEWERROOT
        ENV LD_LIBRARY_PATH
        ENV LIBRARY_PATH
  PATH_SUFFIXES QGLViewer QGLViewer/debug
)

if(QGLVIEWER_LIBRARY_RELEASE)
  if(QGLVIEWER_LIBRARY_DEBUG)
    set(QGLVIEWER_LIBRARY optimized ${QGLVIEWER_LIBRARY_RELEASE} debug ${QGLVIEWER_LIBRARY_DEBUG})
  else()
    set(QGLVIEWER_LIBRARY ${QGLVIEWER_LIBRARY_RELEASE})
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QGLViewer DEFAULT_MSG
  QGLVIEWER_INCLUDE_DIR QGLVIEWER_LIBRARY)
