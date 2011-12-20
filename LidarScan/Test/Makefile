PROJECTS = ../..

INCLUDES = -I$(PROJECTS)/GeometryLibrary -I$(PROJECTS)/FileFormats -I.

LIBS = -L$(PROJECTS)/GeometryLibrary -lgeom -lstdc++ -L/usr/local/lib -openmp

FILELIST = ../Pause.o ../LidarPoint.o ../LidarScanner.o ../LidarScan.o ../LidarStrip.o ../WriteFiles.o

EXTERNALFILES = $(PROJECTS)/FileFormats/ObjFile.o $(PROJECTS)/FileFormats/PtxFile.o $(PROJECTS)/FileFormats/VtkFile.o $(PROJECTS)/FileFormats/ConvertFormat.o

COMPILER = icc

FLAGS = -fPIC

TestScanner: TestScanner.cpp
	$(COMPILER) $(INCLUDES) $(FILELIST) $(EXTERNALFILES) $^ -o $@ $(LIBS) $(FLAGS)

clean:
	rm -f *.o
	rm TestScanner
