#include <iostream>
#include <string>

#include <LidarScan/LidarPoint.h>
#include <LidarScan/LidarStrip.h>
#include <LidarScan/LidarScan.h>
#include <LidarScan/LidarScanner.h>

#include <ModelFile/ModelFile.h>

#include <Geometry/Ray.h>
#include <Geometry/Geometry.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>

using std::cout;
namespace po = boost::program_options;
void CheckRequiredArgs(const po::variables_map vm);

void TestIntersection(ModelFile &Scene, const LidarScanner &Scanner);
void TestScan(ModelFile &Scene, const LidarScanner &Scanner, const std::string &Filename);


int main(int argc, char* argv[])
{
	std::string InputFile, OutputFile;
	double ScannerLocX, ScannerLocY, ScannerLocZ;
	double ForwardX, ForwardY, ForwardZ;
	unsigned int NumPoints;
	double ThetaMin, ThetaMax, PhiMin, PhiMax, minmax;
	bool UseOctree;
	
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("input", po::value<std::string>(&InputFile), "Set input file.")
			("output", po::value<std::string>(&OutputFile), "Set output file.")
			("ScannerLocX", po::value<double>(&ScannerLocX)->default_value(0.0), "Set Scanner X Location.")
			("ScannerLocY", po::value<double>(&ScannerLocY)->default_value(0.0), "Set Scanner Y Location.")
			("ScannerLocZ", po::value<double>(&ScannerLocZ)->default_value(0.0), "Set Scanner Z Location.")
			("ForwardX", po::value<double>(&ForwardX)->default_value(0.0), "Set forward (x)")
			("ForwardY", po::value<double>(&ForwardY)->default_value(0.0), "Set forward (y)")
			("ForwardZ", po::value<double>(&ForwardZ)->default_value(0.0), "Set forward (z)")
			("NumPoints", po::value<unsigned int>(&NumPoints)->default_value(50), "Set number of points on each side of the scan (total points is NumPoints^2).")
			("PhiMin", po::value<double>(&PhiMin)->default_value(-10.0), "Set min phi angle")
			("PhiMax", po::value<double>(&PhiMax)->default_value(10.0), "Set max phi angle")
			("ThetaMin", po::value<double>(&ThetaMin)->default_value(-10.0), "Set min theta angle")
			("ThetaMax", po::value<double>(&ThetaMax)->default_value(10.0), "Set max theta angle")
			("minmax", po::value<double>(&minmax)->default_value(60.0), "Set minmax in degrees (-minmax is thetamin and phimin, +minmax is thetamax and phimax).")
			("octree", po::value<bool>(&UseOctree)->default_value(true), "Use octree?")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}
	
	CheckRequiredArgs(vm);
	
	ModelFile Scene;
	Scene.Read(InputFile);
	Scene.Init();
		
	//setup scanner
	LidarScanner Scanner(true); //use octree
	Scene.BuildOctree();
	
	vgl_point_3d<double> Pos(ScannerLocX, ScannerLocY, ScannerLocZ);
	vgl_vector_3d<double> Dir(ForwardX, ForwardY, ForwardZ);
	Scanner.Orient(Pos, Dir);

	Scanner.ScanParams.setNumPhiPoints(NumPoints);
	Scanner.ScanParams.setNumThetaPoints(NumPoints);
	
	Scanner.ScanParams.setPhiMin(deg2rad(PhiMin));
	Scanner.ScanParams.setPhiMax(deg2rad(PhiMax));
	Scanner.ScanParams.setThetaMin(deg2rad(ThetaMin));
	Scanner.ScanParams.setThetaMax(deg2rad(ThetaMax));

	TestScan(Scene, Scanner, OutputFile);
	//Scene.DeleteTriangles();
	//TestScanImage(Scene, Scanner, OutputFile);
	//TestIntersection(Scene, Scanner);

	return 0;
}



void TestScan(ModelFile &Scene, const LidarScanner &Scanner, const std::string &Filename)
{
	LidarScan Scan = Scanner.AcquireScene(Scene);
	std::cout << "Hits: " << Scan.NumHits() << std::endl;
	
	vector<vgl_point_3d<double> > Points = Scan.GetAllPoints();
	
	ModelFile Test;
	Test.setCoords(Points);
	Test.setScannerLocation(Scanner.ScanParams.getLocation());
	
	Test.Write(Filename);

}

void TestIntersection(ModelFile &Scene, const LidarScanner &Scanner)
{
	LidarPoint Intersection;
	bool intersect = Scanner.AcquirePointOctree(Scene, vgl_vector_3d<double>(-1.0, 0.0, 0.0), Intersection);
	std::cout << "Intersect? " << intersect << std::endl;
	if(intersect)
		std::cout << Intersection << std::endl;
}

void CheckRequiredArgs(const po::variables_map vm)
{
	if(!vm.count("input"))
	{
		std::cout << "Please specify an input file." << std::endl;
		exit(-1);
	}
		
	if(!vm.count("output"))
	{
		std::cout << "Please specify an output file." << std::endl;
		exit(-1);
	}
}

#if 0
void Old()
{

	//position scanner
	vgl_point_3d<double> ScannerPos(ScannerX, ScannerY, ScannerZ);

	//setup scanner
	LidarScanner Scanner(UseOctree); //use octree
	Scene.BuildOctree();
	
	//set scanner orientation
	//vnl_vector_3d<double> DefaultForward = Scanner.DefaultForward();
	vgl_vector_3d<double> Translation = VXLHelpers::vgl_point_to_vgl_vector(ScannerPos);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2.0);

	vgl_point_3d<double> Origin(0.0, 0.0, 0.0);
	Ray ScannerForward(Origin, Scanner.DefaultForward());
	Ray NewDirection(ScannerPos, Origin - ScannerPos);
	vnl_double_3x3 R = VXLHelpers::Get3x3SubMatrix(geom::CameraTransform(ScannerForward, NewDirection));

	cout << "Translation: " << Translation << endl;
	cout << "Rotation: " << R << endl;
	Transformation Trans(Translation, R);
	
	Scanner.ScanParams.setTransformation(Trans);
	
	Scanner.ScanParams.setNumPhiPoints(NumPoints);
	Scanner.ScanParams.setNumThetaPoints(NumPoints);
	minmax = deg2rad(minmax);
	/*
	Scanner.ScanParams.setPhiMin(-minmax);
	Scanner.ScanParams.setPhiMax(minmax);
	Scanner.ScanParams.setThetaMin(-minmax);
	Scanner.ScanParams.setThetaMax(minmax);
	*/
	
	//hard coded params (for testing)
	Scanner.ScanParams.setPhiMin(deg2rad(-5));
	Scanner.ScanParams.setPhiMax(deg2rad(5));
	Scanner.ScanParams.setThetaMin(deg2rad(-10));
	Scanner.ScanParams.setThetaMax(deg2rad(10));
	Scanner.ScanParams.setNumPhiPoints(25);
	
}
#endif