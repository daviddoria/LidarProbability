#include <iostream>
#include <string>

#include "../LidarPoint.h"
#include "../LidarScanner.h"
#include "../LidarScan.h"

#include <ModelFile/ModelFile.h>

void SinglePointNormalGreen(ModelFile &Scene);
void SinglePointNormalDirectionGreen(ModelFile &Scene);

void SinglePointFastGreen(ModelFile &Scene);
void SinglePointFastDirectionGreen(ModelFile &Scene);

void ThreePointsFastGreen(ModelFile &Scene);
void ThreePointsNormalGreen(ModelFile &Scene);

int main(int argc, char* argv[])
{
	std::string InputFile = argv[1];

	//ModelFile Scene(InputFile);

	//SinglePointNormalGreen(Scene);
	//SinglePointNormalDirectionGreen(Scene);
	
	//SinglePointFastGreen(Scene);
	//SinglePointFastDirectionGreen(Scene);

	//ThreePointsNormalGreen(Scene);
	//ThreePointsFastGreen(Scene);

	return 0;
}

void ThreePointsFastGreen(ModelFile &Scene)
{
//BROKEN!!!
	std::cout << "ThreePointsFastGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(true);

	vgl_vector_3d<double> Translation(0, 0, 10);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);
	
	bool bValid = false;
	LidarPoint Pcenter;
	bValid = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(0, 0), Pcenter);
	
	if(bValid)
		cout << "Center: " << Pcenter << endl;
	else
		cout << "No intersection!" << endl;
	
	LidarPoint Pupright;
	bValid = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(.05, .05), Pupright);
	cout << "Up Right Point: " << Pupright << endl;
	
	LidarPoint Pdownleft;
	bValid = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(-.05, -.05), Pdownleft);
	std::cout << "Down Left Point: " << Pdownleft << std::endl;
	
	// Output to file
	std::vector<vgl_point_3d<double> > Points;
	Points.push_back(Pcenter.getCoord());
	Points.push_back(Pupright.getCoord());
	Points.push_back(Pdownleft.getCoord());

	std::vector<Color<unsigned char> > ColorList;
	ColorList.push_back(Colors::Red());
	ColorList.push_back(Colors::Green());
	ColorList.push_back(Colors::Blue());
	
	ModelFile Test;
	Test.setCoords(Points);
	Test.setColors(ColorList);
	Test.Write("TestThreePointsFast.vtp");
	
}

void ThreePointsNormalGreen(ModelFile &Scene)
{
//BROKEn!!!
	std::cout << "ThreePointsNormalGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(false);

	vgl_vector_3d<double> Translation(0, 0, 10);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);

	std::cout << "Scanner: " << Scanner << std::endl;
	bool bIntersect = false;
	LidarPoint Pcenter;
	bIntersect = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(0, 0), Pcenter);

	std::cout << "Center Point: " << Pcenter << std::endl;

	LidarPoint Pupright;
	bIntersect = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(.05, .05), Pupright);
	std::cout << "Up Right Point: " << Pupright << std::endl;
	
	LidarPoint Pdownleft;
	bIntersect = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(-.05, -.05), Pdownleft);
	std::cout << "Down Left Point: " << Pdownleft << std::endl;

	std::vector<vgl_point_3d<double> > Points;	
	Points.push_back(Pcenter.getCoord());
	Points.push_back(Pupright.getCoord());
	Points.push_back(Pdownleft.getCoord());

	//output
	std::vector<Color<unsigned char> > ColorList;
	ColorList.push_back(Colors::Red());
	ColorList.push_back(Colors::Green());
	ColorList.push_back(Colors::Blue());
	
	ModelFile Test;
	Test.setCoords(Points);
	Test.setColors(ColorList);
	
	Test.Write("TestThreePointsNormal.vtp");
	
}

void SinglePointFastGreen(ModelFile &Scene)
{
//BROKEN!!!
	std::cout << "SinglePointFastGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(true);

	vgl_vector_3d<double> Translation(0, 0, 10);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);

	//ModelFile ColoredScene = Scene.Colored();
	
	bool bValid = false;
	LidarPoint Pcenter;
	bValid = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(0, 0), Pcenter);
	
	std::cout << "Center: " << Pcenter << std::endl;

	// Output to file
	std::vector<vgl_point_3d<double> > Points;
	Points.push_back(Pcenter.getCoord());

	ModelFile Test;
	Test.setCoords(Points);
	Test.Write("TestSinglePointFast.vtp");
}


void SinglePointFastDirectionGreen(ModelFile &Scene)
{
//BROKEN!!!
	std::cout << "SinglePointFastGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(true);

	vgl_vector_3d<double> Translation(0, 0, 10);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);

	//ModelFile ColoredScene = Scene.Colored();
	
	bool bValid = false;
	LidarPoint Pcenter;
	bValid = Scanner.AcquirePoint(Scene, vgl_vector_3d<double>(0, 0, -1), Pcenter);
	
	std::cout << "Center: " << Pcenter << std::endl;

	// Output to file
	vector<vgl_point_3d<double> > Points;
	Points.push_back(Pcenter.getCoord());

	ModelFile Test;
	Test.setCoords(Points);
	Test.Write("TestPointFastDirectionGreen.vtp");
}

void SinglePointNormalGreen(ModelFile &Scene)
{
//BROKEN!!!
	std::cout << "SinglePointNormalGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(false);

	vgl_vector_3d<double> Translation(0, 0, 10);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);

	std::cout << Scanner << std::endl;
	bool bIntersect = false;
	
	LidarPoint Pcenter;
	bIntersect = Scanner.AcquirePoint(Scene, Scanner.SetThetaPhi(0, 0), Pcenter);

	std::cout << "Center Point: " << Pcenter << std::endl;
	
	std::vector<vgl_point_3d<double> > Points;

	if(Pcenter.getHit())
	{
		Points.push_back(Pcenter.getCoord());
		ModelFile Test;
		Test.setCoords(Points);
		Test.Write("TestSinglePointNormal.vtp");
	}
	else
	{
		std::cout << "No intersection!" << std::endl;
	}
}


void SinglePointNormalDirectionGreen(ModelFile &Scene)
{
//BROKEN!!!
	std::cout << "SinglePointNormalDirectionGreen()" << std::endl
			<< "--------------------------" << std::endl;
	LidarScanner Scanner(false);

	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(Translation, R);
	//Scanner.ScanParams.setTransformation(Trans);

	std::cout << Scanner << std::endl;
	bool bIntersect = false;
	
	LidarPoint Pcenter;
	bIntersect = Scanner.AcquirePoint(Scene, vgl_vector_3d<double>(0.0, 0.0, -1.0), Pcenter);
	
	std::cout << "Center Point: " << Pcenter << std::endl;
	
	std::vector<vgl_point_3d<double> > Points;

	if(Pcenter.getHit())
	{
		Points.push_back(Pcenter.getCoord());
		ModelFile Test;
		Test.setCoords(Points);
		Test.Write("TestSinglePointNormalDirection.vtp");
	}
	else
	{
		std::cout << "No intersection!" << std::endl;
	}
}

