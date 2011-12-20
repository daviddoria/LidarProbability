#include <iostream>
#include <vector>

#include "LidarPoint.h"
#include "LidarStrip.h"
#include "LidarScan.h"
#include "LidarScanner.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <ModelFile/ModelFile.h>

#include <Geometry/Color.h>

#include <cmath>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void PerformScan(ModelFile &Scene, const LidarScanner &Scanner, const std::string &OutputFilename);
void CheckRequiredArgs(const po::variables_map vm);
//usage

//SingleScan --input 4.vtp --output 4scan.vtp --ScannerLocX 10 --ScannerLocY -20 --ScannerLocZ 2

int main(int argc, char* argv[])
{
	std::cout << "Enter main." << std::endl;
	
	std::string InputFile, OutputFile;
	double ScannerLocX, ScannerLocY, ScannerLocZ;
	double ForwardX, ForwardY, ForwardZ;
	unsigned int NumPoints;
	double ThetaMin, ThetaMax, PhiMin, PhiMax, minmax;

	std::cout << "Options:" << std::endl;
	
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("input", po::value<std::string>(&InputFile), "Set input file")
			("output", po::value<std::string>(&OutputFile), "Set output file")
			("ScannerLocX", po::value<double>(&ScannerLocX)->default_value(0.0), "Set scanner location (x)")
			("ScannerLocY", po::value<double>(&ScannerLocY)->default_value(0.0), "Set scanner location (y)")
			("ScannerLocZ", po::value<double>(&ScannerLocZ)->default_value(0.0), "Set scanner location (z)")
			("ForwardX", po::value<double>(&ForwardX)->default_value(0.0), "Set forward (x)")
			("ForwardY", po::value<double>(&ForwardY)->default_value(0.0), "Set forward (y)")
			("ForwardZ", po::value<double>(&ForwardZ)->default_value(0.0), "Set forward (z)")
			("NumPoints", po::value<unsigned int>(&NumPoints)->default_value(100), "Set number of points (one side of the grid, assuming a square grid)")
			("PhiMin", po::value<double>(&PhiMin)->default_value(-10.0), "Set min phi angle")
			("PhiMax", po::value<double>(&PhiMax)->default_value(10.0), "Set max phi angle")
			("ThetaMin", po::value<double>(&ThetaMin)->default_value(-10.0), "Set min theta angle")
			("ThetaMax", po::value<double>(&ThetaMax)->default_value(10.0), "Set max theta angle")
			("minmax", po::value<double>(&minmax), "Set max theta angle")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm); //assign the variables (if they were specified)
	
	if(vm.count("help")) 
	{
		std::cout << desc << std::endl;
		return 1;
	}

	CheckRequiredArgs(vm);
	
	std::cout << std::endl << std::endl;

	if(!vm.count("minmax"))
	{
		//convert degrees to radians
		PhiMin = deg2rad(PhiMin);
		PhiMax = deg2rad(PhiMax);
		ThetaMin = deg2rad(ThetaMin);
		ThetaMax = deg2rad(ThetaMax);
	}
	else
	{
		PhiMin = deg2rad(-minmax);
		PhiMax = deg2rad(minmax);
		ThetaMin = deg2rad(-minmax);
		ThetaMax = deg2rad(minmax);
	}
	/////////////////////////////////////////////////////
	
	//setup model
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
	
	Scanner.ScanParams.setPhiMin(PhiMin);
	Scanner.ScanParams.setPhiMax(PhiMax);
	Scanner.ScanParams.setThetaMin(ThetaMin);
	Scanner.ScanParams.setThetaMax(ThetaMax);
	
	//scan
	PerformScan(Scene, Scanner, OutputFile);
	return 0;

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


void PerformScan(ModelFile &Scene, const LidarScanner &Scanner, const std::string &OutputFilename)
{
	LidarScan Scan = Scanner.AcquireScene(Scene);
		
	ModelFile Model;
	Scan.CreateModelFile(Model);
	Model.Write(OutputFilename);
	Scan.WritePTX("Scan.ptx");
	Scan.WriteBinaryHitImage("HitImage.png");
	Scan.WriteImage("DepthImage.png");
	std::cout << "Hits: " << Scan.NumHits() << std::endl;
	
}
