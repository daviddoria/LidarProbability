ENABLE_TESTING()

ADD_EXECUTABLE(TestScan TestScan.cpp)
TARGET_LINK_LIBRARIES(TestScan libLidarScan boost_program_options-mt)
ADD_TEST(TestScan TestScan)

############################################
ADD_EXECUTABLE(TestScanner TestScanner.cpp)
TARGET_LINK_LIBRARIES(TestScanner libLidarScan)
ADD_TEST(TestScan TestScan)

############################################
ADD_EXECUTABLE(TestStrip TestStrip.cpp)
TARGET_LINK_LIBRARIES(TestStrip libLidarScan)
ADD_TEST(TestStrip TestStrip)

###########################################
ADD_EXECUTABLE(TestPoint TestPoint.cpp)
TARGET_LINK_LIBRARIES(TestPoint libLidarScan)
ADD_TEST(TestPoint TestPoint)

###########################################
ADD_EXECUTABLE(TestScanImage TestScanImage.cpp)
TARGET_LINK_LIBRARIES(TestScanImage libLidarScan)
ADD_TEST(TestScanImage TestScanImage)
