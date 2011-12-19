#include <iostream>
#include <fstream>
#include <string>

//#include "Confidence.h"
#include "Consistency.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <ModelFile/ModelFile.h>

#include <vul/vul_file.h>

#include <Tools/Tools.h>

#include <boost/progress.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

void CheckRequiredArgs(const po::variables_map &vm);

int main(int argc, char *argv[])
{
	Tools::Log L("log.txt");
	L.Enable();
	
	std::string SceneFilename, TriModelFilename, PointModelFilename;
	std::string ConfMapFile, ConsistMapFile;
	double SphereSize;
	
	//parse arguments
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("scene", po::value<std::string>(&SceneFilename), "Set scene file")
			("TriModel", po::value<std::string>(&TriModelFilename), "Set triangulated model file")
			("PointModel", po::value<std::string>(&PointModelFilename), "Set uniform point model file")
			("sphere", po::value<double>(&SphereSize), "Sphere size.")
			("ConfMap", po::value<std::string>(&ConfMapFile), "Confidence map file.")
			("ConsistMap", po::value<std::string>(&ConsistMapFile), "Consistency map file.")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help"))
	{
		std::cout << desc << std::endl;
		exit(-1);
	}

	CheckRequiredArgs(vm);
	
	ModelFile Scene;
	Scene.Read(SceneFilename);
	ModelFile TriModel;
	TriModel.Read(TriModelFilename);
	ModelFile PointModel;
	PointModel.Read(PointModelFilename);
	
	std::ofstream ConfMapOut(ConfMapFile.c_str());
	std::ofstream ConsistMapOut(ConsistMapFile.c_str());
		
	vgl_point_3d<double> Origin = PointModel.getCenterOfMass();
	

	//for confidence
	int GridSizeX = 60;
	int GridSizeY = 80;
	double StepX = .2;
	double StepY = .2;
	
	
	//for consistency
/*
#ifdef TEST
	GridSizeX = 2;
	GridSizeY = 2;
	StepX = .4;
	StepY = .4;
#endif
*/
	
	std::vector<vgl_point_3d<double> > ConfSurfacePoints;
	std::vector<vgl_point_3d<double> > ConsistSurfacePoints;
	
	//std::cout << "-GridSizeX/2 = " << -GridSizeX/2 << std::endl;
	
	int TotalPoints = GridSizeX*GridSizeY;
	std::cout << "Total points: " << TotalPoints << std::endl;
	unsigned int counter = 0;
	boost::progress_display show_progress(TotalPoints);
	for(int x = -GridSizeX/2; x <= GridSizeX/2; x++)
	{
		for(int y = -GridSizeX/2; y < GridSizeY/2; y++)
		{	
			//std::cout << "x: " << x << " y: " << y << std::endl;
			/*
			double xpos = static_cast<double>(x) * StepX;
			double ypos = static_cast<double>(y) * StepY;
			PointModel.Translate(vgl_vector_3d<double>(xpos, ypos, 0.0));
			*/
			double delx = static_cast<double>(x) * StepX;
			double dely = static_cast<double>(y) * StepY;
			vgl_point_3d<double> Position(Origin.x() + delx, Origin.y() + dely, Origin.z());
			
			//PointModel.SetCenterOfMass(Position);
			TriModel.SetCenterOfMass(Position);
			
			/*
			std::stringstream ss;
			ss << "Tri_" << x << "_" << y << ".vtp";
			TriModel.Write(ss.str());
			*/
			
			/*
			//Confidence
			double Confidence = CalculateConfidence(PointModel, Scene, SphereSize);
			ConfMapOut << Position.x() << " " << Position.y() << " " << Confidence << std::endl;
			ConfSurfacePoints.push_back(vgl_point_3d<double>(Position.x(), Position.y(), Confidence));
			*/
			
			//Consistency
			std::clog << "Getting comparable model..." << std::endl;
			
			ComparableModels Comparable(Scene, TriModel);
			Comparable.Init();
			
			//std::cout << "Calculating Consistency..." << std::endl;
			Consistency C(Comparable, SphereSize);
			
			//C.ShowProgress = false;
			double consist = C.CalculateConsistency();
			ConfMapOut << Position.x() << " " << Position.y() << " " << consist << std::endl;
			ConsistSurfacePoints.push_back(vgl_point_3d<double>(Position.x(), Position.y(), consist));
			//Comparable.WriteMatchingPoints("ConsistencyScene.vtp", "ConsistencyModel.vtp");
			//Comparable.WriteConnectingLines("ConsistencyLines.vtp");
			ConsistMapOut << Position.x() << " " << Position.y() << " " << consist << std::endl;
			//std::cout << "end loop" << std::endl;
			++show_progress;
		} //end y loop
	} //end x loop
	
	ConfMapOut.close();
	ConsistMapOut.close();
	
	/*
	ModelFile ConfSurface;
	ConfSurface.setCoords(ConfSurfacePoints);
	ConfSurface.Write("ConfSurface.vtp");
	*/
	ModelFile ConsistSurface;
	ConsistSurface.setCoords(ConsistSurfacePoints);
	ConsistSurface.Write("ConsistSurface.vtp");
	return 0;
}

void CheckRequiredArgs(const po::variables_map &vm)
{
	if(!vm.count("scene"))
	{
		std::cout << "Please specify a scene file." << std::endl;
		exit(-1);
	}

	if(!vm.count("TriModel"))
	{
		std::cout << "Please specify a triangulated model file." << std::endl;
		exit(-1);
	}

	if(!vm.count("PointModel"))
	{
		std::cout << "Please specify a uniformly spaced vertex model file." << std::endl;
		exit(-1);
	}
	
	if(!vm.count("sphere"))
	{
		std::cout << "Please specify a model file." << std::endl;
		exit(-1);
	}

	if(!vm.count("ConfMap"))
	{
		std::cout << "Please specify a confidence map file." << std::endl;
		exit(-1);
	}
	
	if(!vm.count("ConsistMap"))
	{
		std::cout << "Please specify a consistency map file." << std::endl;
		exit(-1);
	}
}
