cmake_minimum_required(VERSION 2.6)
PROJECT(cvQTcameraGL)

find_package( OpenCV REQUIRED )
set(CMAKE_AUTOMOC ON)
find_package( Qt5Widgets REQUIRED)
find_package( Qt5Core REQUIRED)
find_package( Qt5OpenGL REQUIRED)
find_package( GLUT REQUIRED)
find_package( GLEW   REQUIRED )
find_package( OpenGL REQUIRED)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

QT5_WRAP_CPP( WIDGET_MOC_SOURCES
              ${CMAKE_CURRENT_SOURCE_DIR}/src/widget.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/include/widget.h)


QT5_WRAP_CPP(MAIN_MOCsrcs
   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )

set(MAIN_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(TEXTFILE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textfile.cpp

)

set(WIDGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/widget.h

)

ADD_LIBRARY(textfile
  ${TEXTFILE_SOURCES}

)

ADD_LIBRARY(widget
  ${WIDGET_SOURCES}
  ${WIDGET_MOC_SOURCES}
)

ADD_EXECUTABLE(cvQTcameraGL
  ${MAIN_MOC_SOURCES}
  ${MAIN_SOURCES}
)

TARGET_LINK_LIBRARIES(widget
  ${OpenCV_LIBS} 
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
  ${GLEW_LIBRARY}
  textfile
)

TARGET_LINK_LIBRARIES(cvQTcameraGL
  widget
)

qt5_use_modules(widget Core Gui Widgets OpenGL)
qt5_use_modules(cvQTcameraGL Core Gui Widgets)