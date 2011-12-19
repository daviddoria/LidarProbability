cmake_minimum_required(VERSION 2.6)

PROJECT(LidarProbability)

FIND_PACKAGE(VXL REQUIRED)
INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.38 COMPONENTS program_options required)

######################################
set(PROJECTS /media/portable/Projects/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -DGRAPHICS")

set(FileFormatsDir ${PROJECTS}/FileFormats/)
set(DisplayDir ${PROJECTS}/Geometry/Display)
set(LidarScanDir ${PROJECTS}/LidarScan/)
set(GeometryDir ${PROJECTS}/Geometry/)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES}
${PROJECTS}
${Boost_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${LINK_DIRECTORIES} 
${Boost_LIBRARY_DIRS}
)

set(Sources
Consistency.cpp
#Confidence.cpp

${PROJECTS}/ComparableModels/ComparableModels.cpp

${GeometryDir}/Angles.cpp 
${GeometryDir}/Color.cpp 
${GeometryDir}/Ray.cpp
${GeometryDir}/Edge.cpp
${GeometryDir}/Triangle.cpp
${GeometryDir}/Transformation.cpp
${GeometryDir}/LineSegment.cpp
${GeometryDir}/OrientedPoint.cpp
${GeometryDir}/Geometry.cpp
${GeometryDir}/Helpers.cpp

${PROJECTS}/ModelFile/ModelFile.cpp

${LidarScanDir}/LidarPoint.cpp 
${LidarScanDir}/LidarStrip.cpp 
${LidarScanDir}/LidarScan.cpp 
${LidarScanDir}/LidarScanner.cpp
${PROJECTS}/LidarScan/ScanParamsClass.cpp

${PROJECTS}/ObjViewer/CameraClass.h
${PROJECTS}/VXLHelpers/VXLHelpers.cpp
${PROJECTS}/Integration/integration.cpp

${PROJECTS}/Tools/Tools.cpp

${PROJECTS}/VTKHelpers/Octree.cpp
${PROJECTS}/KDTree/KDTree.cpp

)


set(Libraries
vnl vnl_algo vbl vbl_io vul vgl vgl_algo vil vsl
${VTK_LIBRARIES}
boost_program_options-mt
)

ADD_LIBRARY(MyLibs ${Sources})
TARGET_LINK_LIBRARIES(MyLibs ${Libraries})

###########################################################
# ADD_EXECUTABLE(ColorMatches ColorMatches.cpp)
# TARGET_LINK_LIBRARIES(ColorMatches ${Libraries} MyLibs)

###########################################################
# ADD_EXECUTABLE(CalculateScores CalculateScores.cpp)
# TARGET_LINK_LIBRARIES(CalculateScores ${Libraries} MyLibs)

# ###########################################################
# ADD_EXECUTABLE(TestConfidence TestConfidence.cpp)
# TARGET_LINK_LIBRARIES(TestConfidence ${Libraries} MyLibs)
# # 
# ###########################################################
# ADD_EXECUTABLE(TestConsistency TestConsistency.cpp)
# TARGET_LINK_LIBRARIES(TestConsistency ${Libraries} MyLibs)

# ###########################################################
# ADD_EXECUTABLE(ConfidenceProbMatrix ConfidenceProbMatrix.cpp)
# TARGET_LINK_LIBRARIES(ConfidenceProbMatrix ${Libraries} MyLibs)
# 
# ###########################################################
# ADD_EXECUTABLE(ConsistencyProbMatrix ConsistencyProbMatrix.cpp)
# TARGET_LINK_LIBRARIES(ConsistencyProbMatrix ${Libraries} MyLibs)

# ###########################################################
# ADD_EXECUTABLE(TestSingleVariableMotion TestSingleVariableMotion.cpp)
# TARGET_LINK_LIBRARIES(TestSingleVariableMotion ${Libraries} MyLibs)
#  
# ###########################################################
# ADD_EXECUTABLE(TestMultipleScanners TestMultipleScanners.cpp)
# TARGET_LINK_LIBRARIES(TestMultipleScanners ${Libraries} MyLibs)
# 
# ###########################################################
# ADD_EXECUTABLE(WriteMultiplePositions WriteMultiplePositions.cpp)
# TARGET_LINK_LIBRARIES(WriteMultiplePositions ${Libraries} MyLibs)

###########################################################

# ADD_EXECUTABLE(CombineWindows CombineWindows.cpp)
# TARGET_LINK_LIBRARIES(CombineWindows ${Libraries} MyLibs)

###########################################################

# ADD_EXECUTABLE(TestLineBetweenPoints TestLineBetweenPoints.cpp)
# TARGET_LINK_LIBRARIES(TestLineBetweenPoints ${Libraries} MyLibs)

###########################################################
ADD_EXECUTABLE(HeatMap HeatMap.cpp)
TARGET_LINK_LIBRARIES(HeatMap ${Libraries} MyLibs)

###########################################################
# ADD_EXECUTABLE(UsedScenePoints UsedScenePoints.cpp)
# TARGET_LINK_LIBRARIES(UsedScenePoints ${Libraries} MyLibs)

###########################################################
# ADD_EXECUTABLE(WindowSlide WindowSlide.cpp ${Sources})
# TARGET_LINK_LIBRARIES(WindowSlide ${Libraries} MyLibs)
