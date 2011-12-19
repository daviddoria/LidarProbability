#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

#include <LidarScan/LidarScanner.h>
#include <ModelFile/ModelFile.h>
#include <Geometry/Transformation.h>
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include "Consistency.h"
#include "Confidence.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

/*
run with
./WindowSlide --world orig/vcc_south1.vtp --model orig/window.vtp --transforms orig/transforms.xform --consistfile consist.txt --conffile conf.txt
*/

void CheckRequiredArgs(const po::variables_map &vm);

int main(int argc, char *argv[])
{
	std::string TransformFilename, WorldFilename, ModelFilename;
	std::string ConsistencyFilename, ConfidenceFilename;
	double Mismatch;
	
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("world", po::value<std::string>(&WorldFilename), "Set world file")
			("model", po::value<std::string>(&ModelFilename), "Set model file")
			("transforms", po::value<std::string>(&TransformFilename), "Set transforms file")
			("consistfile", po::value<std::string>(&ConsistencyFilename)->default_value("consist.txt"), "Set consistency output file")
			("conffile", po::value<std::string>(&ConfidenceFilename)->default_value("conf.txt"), "Set confidence output file")
			("mismatch", po::value<double>(&Mismatch)->default_value(0.5), "Mismatch allowance")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help"))
	{
		cout << desc << endl;
		exit(-1);
	}
	CheckRequiredArgs(vm);
	
	std::vector<Transformation> Xforms = ReadAllTransformations(TransformFilename);
	
	//read world file
	ModelFile WorldVtp;
	WorldVtp.Read(WorldFilename);
	
	//read model file
	ModelFile ModelVtp;
	ModelVtp.Read(ModelFilename);
	
	//std::vector<vgl_point_3d<double> > PlotPositions;
	std::vector<double> ConfidenceValues(Xforms.size());
	std::vector<double> ConsistencyValues(Xforms.size());
	
	ModelFile CurrentModel;
	
	std::vector<vgl_point_3d<double> > CentersOfMass;
			
	std::ofstream ConsistValuesOut("ConsistValues.txt");
	std::ofstream ConfValuesOut("ConfValues.txt");
	std::ofstream COMOut("COM.txt");
	
	for(unsigned int PositionIndex = 0; PositionIndex < Xforms.size(); PositionIndex++)
	{
		std::cout << "Position " << PositionIndex << "\n";
		
		CurrentModel = ModelVtp;
		CurrentModel.Rotate(Xforms[PositionIndex].getRotation());
		CurrentModel.Translate(Xforms[PositionIndex].getTranslation());
		std::cout << "Building octree...\n";
		CurrentModel.BuildOctree();
		
		//write the model at the current position
		std::stringstream ssCurrentName;
		ssCurrentName << "slide_" << Tools::ZeroPad(PositionIndex, 3) << ".vtp";
		CurrentModel.Write(ssCurrentName.str());
		
/*
		///////////////// Confidence /////////////
		std::cout << "\nConf:\n\n";
		Confidence Conf(CurrentModel, WorldVtp, Mismatch);
		double conf = Conf.CalculateConfidence();
		ConfidenceValues[PositionIndex] = conf;
		ConfValuesOut << conf << std::endl;
		//write the colored model at the current position
		std::stringstream ssConfidence;
		ssConfidence << "slide_confidence_" << Tools::ZeroPad(PositionIndex, 3) << ".vtp";
		Conf.Model.Write(ssConfidence.str());
*/	
		///////////////// Consistency /////////////
		//create comparable models
		std::cout << "Creating comparable models...\n";
		ComparableModels Comparable(WorldVtp, CurrentModel);
		Comparable.Init();
		
		std::cout << "\nConst:\n\n";
		Consistency C(Comparable, Mismatch);
		double consist = C.CalculateConsistency();
		ConsistencyValues[PositionIndex] = consist;
		ConsistValuesOut << consist << std::endl;
		
		//write the colored model at the current position
		std::stringstream ssConsistency;
		ssConsistency << "slide_consistency_" << Tools::ZeroPad(PositionIndex, 3) << ".vtp";

		ModelFile ColoredModel = Comparable.ColorOriginalModel(Mismatch);
		ColoredModel.Write(ssConsistency.str());
		
		//keep track of position
		vgl_point_3d<double> CenterOfMass = CurrentModel.getCenterOfMass();
		CentersOfMass.push_back(CenterOfMass);
		COMOut << CenterOfMass.x() << " " << CenterOfMass.y() << " " << CenterOfMass.z() << std::endl;
	
	}//end PositionIndex loop
	
	ConsistValuesOut.close();
	ConfValuesOut.close();
	COMOut.close();
	
	//for line plot
	std::ofstream ConsistOut(ConsistencyFilename.c_str());
	std::ofstream ConfOut(ConfidenceFilename.c_str());
	
	std::vector<vgl_point_3d<double> > ConsistencyLine;
	std::vector<vgl_point_3d<double> > ConfidenceLine;
	
	double scale = 2.0;
	for(unsigned int i = 0; i < CentersOfMass.size(); i++)
	{
		vgl_point_3d<double> ConfidencePoint = CentersOfMass[i];
		ConfidencePoint += vgl_vector_3d<double> (0.0, 0.0, scale*ConfidenceValues[i]);
		ConfidenceLine.push_back(ConfidencePoint);
		ConfOut << ConfidencePoint.x() << " " << ConfidencePoint.y() << " " << ConfidencePoint.z() << endl;
		
		vgl_point_3d<double> ConsistencyPoint = CentersOfMass[i];
		ConsistencyPoint += vgl_vector_3d<double> (0.0, 0.0, scale*ConsistencyValues[i]);
		ConsistencyLine.push_back(ConsistencyPoint);
		ConsistOut << ConsistencyPoint.x() << " " << ConsistencyPoint.y() << " " << ConsistencyPoint.z() << endl;
	}
	
	ConsistOut.close();
	ConfOut.close();

	return 0;
}

void CheckRequiredArgs(const po::variables_map &vm)
{
	if(!vm.count("transforms"))
	{
		std::cout << "Please specify a world file." << std::endl;
		exit(-1);
	}

	if(!vm.count("world"))
	{
		std::cout << "Please specify a model file." << std::endl;
		exit(-1);
	}
	
	if(!vm.count("model"))
	{
		std::cout << "Please specify a model file." << std::endl;
		exit(-1);
	}
}

/*
void ReadEricFile(string filename, vector<geom_Point3> &points)
{
	FILE *file = fopen(filename.c_str(), "rb");
	while(!feof(file))
	{
		double point[3];
		fread(point, sizeof(double)*3, 1, file);
		points.push_back(geom_Point3(point[0], point[1], point[2]));
	}
	fclose(file);
}
*/