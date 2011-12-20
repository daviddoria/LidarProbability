cmake_minimum_required(VERSION 2.6)

PROJECT(LidarProbability)

FIND_PACKAGE(VXL REQUIRED)
INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.38 COMPONENTS program_options required)

######################################
set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../Tools)

set(LidarScanDir ${CMAKE_CURRENT_SOURCE_DIR}/LidarScan/)
set(GeometryDir ${TOOLS}/Geometry/)

INCLUDE_DIRECTORIES(
${Boost_INCLUDE_DIRS}
${CMAKE_CURRENT_SOURCE_DIR}
${TOOLS}
)

LINK_DIRECTORIES(
${Boost_LIBRARY_DIRS}
)

set(Sources
Consistency.cpp
#Confidence.cpp

ComparableModels/ComparableModels.cpp

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

${TOOLS}/ModelFile/ModelFile.cpp

${LidarScanDir}/LidarPoint.cpp 
${LidarScanDir}/LidarStrip.cpp 
${LidarScanDir}/LidarScan.cpp 
${LidarScanDir}/LidarScanner.cpp
${LidarScanDir}/ScanParamsClass.cpp

#${PROJECTS}/ObjViewer/CameraClass.h
${TOOLS}/VXLHelpers/VXLHelpers.cpp
${TOOLS}/Integration/integration.cpp

${TOOLS}/Tools.cpp

${TOOLS}/VTKHelpers/Octree.cpp
${TOOLS}/KDTree/KDTree.cpp

)

set(VXLLibraries vnl vnl_algo vbl vbl_io vul vgl vgl_algo vil vsl)

set(Libraries
${VXLLibraries}
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
# ADD_EXECUTABLE(HeatMap HeatMap.cpp)
# TARGET_LINK_LIBRARIES(HeatMap ${Libraries} MyLibs)

###########################################################
# ADD_EXECUTABLE(UsedScenePoints UsedScenePoints.cpp)
# TARGET_LINK_LIBRARIES(UsedScenePoints ${Libraries} MyLibs)

###########################################################
# ADD_EXECUTABLE(WindowSlide WindowSlide.cpp ${Sources})
# TARGET_LINK_LIBRARIES(WindowSlide ${Libraries} MyLibs)
