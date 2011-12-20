cmake_minimum_required(VERSION 2.6)

PROJECT(Lidar)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

FIND_PACKAGE(VXL REQUIRED)
INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.38 COMPONENTS program_options required)

#################################

set(PROJECTS /media/portable/Projects/src)

set(GeometryDir ${PROJECTS}/Geometry/)

set(Sources
LidarPoint.cpp
LidarStrip.cpp
LidarScan.cpp
LidarScanner.cpp
ScanParamsClass.cpp

${GeometryDir}/Angles.cpp 
${GeometryDir}/Edge.cpp 
${GeometryDir}/Color.cpp 
${GeometryDir}/Ray.cpp 
${GeometryDir}/Triangle.cpp
${GeometryDir}/OrientedPoint.cpp
${GeometryDir}/Geometry.cpp
${GeometryDir}/Helpers.cpp

${PROJECTS}/VXLHelpers/VXLHelpers.cpp
${PROJECTS}/Tools/Tools.cpp

#${PROJECTS}/Octree/Octree.cpp
${PROJECTS}/VTKHelpers/Octree.cpp
${PROJECTS}/KDTree/KDTree.cpp

${PROJECTS}/ModelFile/ModelFile.cpp

)

set(Libraries
vcl vul vgl vgl_algo vnl vil
${VTK_LIBRARIES}
)

INCLUDE_DIRECTORIES(${PROJECTS} ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

add_subdirectory(Test)

ADD_LIBRARY(libLidarScan ${Sources} )
TARGET_LINK_LIBRARIES(libLidarScan ${Libraries} )


###################################################

ADD_EXECUTABLE(SingleScan SingleScan.cpp)
TARGET_LINK_LIBRARIES(SingleScan libLidarScan boost_program_options-mt)

###################################################
ADD_EXECUTABLE(VisualizeScanFrustrum VisualizeScanFrustrum.cpp)
TARGET_LINK_LIBRARIES(VisualizeScanFrustrum libLidarScan boost_program_options-mt)


###################################################
# 
# ADD_EXECUTABLE(KeepShell KeepShell.cpp)
# TARGET_LINK_LIBRARIES(KeepShell MyLibs boost_program_options-mt)