cmake_minimum_required(VERSION 2.6)
PROJECT(InteractiveBestPatches)

# Where to copy executables when 'make install' is run
SET(INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) #fix the dynamic linking error (RPATH) that would occur without this

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_UI(UISrcs InteractiveBestPatchesWidget.ui)
QT4_WRAP_CPP(MOCSrcs InteractiveBestPatchesWidget.h
#MyGraphicsItem.h
ClickableLabel.h)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

add_library(BestPatches
Helpers.cpp
Patch.cpp
SelfPatchCompare.cpp)

ADD_EXECUTABLE(InteractiveBestPatches
ClickableLabel.cpp
CustomImageStyle.cxx
CustomTrackballStyle.cxx
InteractiveBestPatchesWidget.cpp
InteractiveBestPatches.cpp 
#MyGraphicsItem.cpp
SwitchBetweenStyle.cxx
${UISrcs} ${MOCSrcs})
TARGET_LINK_LIBRARIES(InteractiveBestPatches BestPatches ${VTK_LIBRARIES} QVTK ${ITK_LIBRARIES} ${QT_LIBRARIES})
INSTALL( TARGETS InteractiveBestPatches RUNTIME DESTINATION ${INSTALL_DIR} )


ADD_EXECUTABLE(ExamplePatchDifference ExamplePatchDifference.cpp)
TARGET_LINK_LIBRARIES(ExamplePatchDifference BestPatches ${VTK_LIBRARIES} QVTK ${ITK_LIBRARIES} ${QT_LIBRARIES})
