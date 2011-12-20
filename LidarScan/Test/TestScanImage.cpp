#include <iostream>
#include <string>

#include <LidarScan/LidarPoint.h>
#include <LidarScan/LidarStrip.h>
#include <LidarScan/LidarScan.h>
#include <LidarScan/LidarScanner.h>

#include <ModelFile/ModelFile.h>

#include <Geometry/Ray.h>
#include <Geometry/Geometry.h>

int main(int argc, char* argv[])
{
	std::string InputFile = "Test.vtp", OutputFile = "Output.vtp", OutputImage = "Output.png";
	vgl_point_3d<double> ScannerLocation(0.0, 0.0, 0.0);
	vgl_vector_3d<double> ScannerDir(0.0, 1.0, 0.0);
	
	unsigned int NumPoints = 50;
	double ThetaMin = -10.0, ThetaMax = 10.0, PhiMin = -10.0, PhiMax = 10.0;
	
	ModelFile Scene;
	Scene.Read(InputFile);
	Scene.Init();
	Scene.BuildOctree();
	
	//setup scanner
	LidarScanner Scanner(true); //use octree
	Scanner.ScanParams.setNumPhiPoints(NumPoints);
	Scanner.ScanParams.setNumThetaPoints(NumPoints);
	Scanner.ScanParams.setPhiMin(deg2rad(PhiMin));
	Scanner.ScanParams.setPhiMax(deg2rad(PhiMax));
	Scanner.ScanParams.setThetaMin(deg2rad(ThetaMin));
	Scanner.ScanParams.setThetaMax(deg2rad(ThetaMax));
	
	Scanner.Orient(ScannerLocation, ScannerDir);
		
	LidarScan Scan = Scanner.AcquireScene(Scene);
		
	Scan.WriteImage(OutputImage);

	return 0;
}