FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# Let Qt find it's MOCed files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI(LidarProbabilityUISrcs LidarProbabilityWidget.ui)
QT4_WRAP_CPP(LidarProbabilityMOCSrcs LidarProbabilityWidget.h)# PoissonEditingComputationThread.h ComputationThread.h)

ADD_EXECUTABLE(LidarProbability LidarProbability.cpp LidarProbabilityWidget.cpp ${LidarProbabilityUISrcs} ${LidarProbabilityMOCSrcs})
TARGET_LINK_LIBRARIES(LidarProbability ${ITK_LIBRARIES} ${QT_LIBRARIES} QVTK)
#INSTALL( TARGETS LidarProbability RUNTIME DESTINATION ${INSTALL_DIR} )
