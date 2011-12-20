#include <iostream>
#include <map>
#include <vector>

#include <LidarScan/LidarStrip.h>
#include <LidarScan/LidarPoint.h>
#include <LidarScan/LidarScan.h>
#include <LidarScan/LidarScanner.h>

#include <ModelFile/ModelFile.h>

void TestStripNormal(ModelFile &Scene);
void TestStripFast(ModelFile &Scene);

int main(int argc, char* argv[])
{
	string InputFile = argv[1];

	ModelFile Scene;
	Scene.Read(InputFile);
	
	//TestStripNormal(Scene);
	TestStripFast(Scene);
	
  return 0;
}

void TestStripFast(ModelFile &Scene)
{
	cout << "TestStripFast()" << endl;
	
	LidarScanner Scanner(true);
	Scanner.ScanParams.setLocation(vgl_point_3d<double>(0, 0, 10));
	
	LidarStrip TestStrip = Scanner.AcquireStrip(Scene,0);
	
	vector<vgl_point_3d<double> > Points = TestStrip.GetAllPoints();
	
	cout << "NumHits: " << TestStrip.NumHits() << endl;
	
	ModelFile Test;
	Test.setCoords(Points);
	
	Test.Write("TestStripFast.vtp");
	
}

void TestStripNormal(ModelFile &Scene)
{
	cout << "TestStripNormal()" << endl;
	
	LidarScanner Scanner(false);
	Scanner.ScanParams.setLocation(vgl_point_3d<double>(0, 0, 10));
	
	LidarStrip TestStrip = Scanner.AcquireStrip(Scene, 0);
	
	//cout << "Hits: " << TestStrip.NumHits() << endl;
	
	vector<vgl_point_3d<double> > Points = TestStrip.GetAllPoints();
	
	ModelFile Test;
	Test.setCoords(Points);
	
	Test.Write("TestStripNormal.vtp");
	
}