#include <iostream>
#include <fstream>
#include <string>

#include "Confidence.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

void TestConfidence(ModelFile &Model, const ModelFile &World, const double SphereSize);

void SingleScan(ModelFile &Model, const ModelFile &World);
void TwoScans(ModelFile &Model, const ModelFile &World1, const ModelFile &World2);
void ColorSphereOfInfluence(ModelFile &Model, const ModelFile &World);

void CheckRequiredArgs(const po::variables_map &vm);

int main(int argc, char *argv[])
{
	Tools::Log L("log.txt");
	L.Enable();
	
	std::string WorldFilename, ModelFilename;
	double SphereSize;
	
	//parse arguments
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("world", po::value<std::string>(&WorldFilename), "Set world file")
			("model", po::value<std::string>(&ModelFilename), "Set model file")
			("sphere", po::value<double>(&SphereSize), "Sphere size.")
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
	
	ModelFile World;
	World.Read(WorldFilename);
	ModelFile Model;
	Model.Read(ModelFilename);
	
	if(!vm.count("sphere"))
	{
		SphereSize = Model.AverageBBEdge() / 20.0;
		std::cout << "Sphere size defaulted to " << SphereSize << std::endl;
		
	}

	
	TestConfidence(Model, World, SphereSize);
	
	//ColorSphereOfInfluence(World, Model);
	//SingleScan(World, Model);
	//TwoScans(World, Model);

	return 0;
}

void CheckRequiredArgs(const po::variables_map &vm)
{
	if(!vm.count("world"))
	{
		std::cout << "Please specify a world file." << std::endl;
		exit(-1);
	}

	if(!vm.count("model"))
	{
		std::cout << "Please specify a model file." << std::endl;
		exit(-1);
	}

}

void TestConfidence(ModelFile &Model, const ModelFile &World, const double SphereSize)
{
	std::cout << "TestConfidence" << std::endl << "------------------" << std::endl;
	Confidence Conf(Model, World, SphereSize);
	double conf = Conf.CalculateConfidence();
	std::cout << std::endl << "Confidence: " << conf << std::endl;
	
	Model.Write("Confidence.vtp");
}


void SingleScan(ModelFile &Model, const ModelFile &World)
{
	Confidence Conf(Model, World, .5);
	cout << "before: " << Model.getRemainingInformation(178) << endl;
	Conf.DeductAllPointInformation();
	cout << "after: " << Model.getRemainingInformation(178) << endl;
	
	cout << "Total Information: " << Model.SumTotalInformation() << endl;
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Model.Write("Colored.vtp");
	
}

void TwoScans(ModelFile &Model, const ModelFile &World1, const ModelFile &World2)
{
	Confidence Conf1(Model, World1, .5);
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Conf1.DeductAllPointInformation();
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Confidence Conf2(Model, World2, .5);
	Conf2.DeductAllPointInformation();
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Confidence Conf3(Model,World1,.5);
	Conf3.DeductAllPointInformation(); //this shouldn't change much, most of the information should already be used
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;

}

void ColorSphereOfInfluence(ModelFile &Model, const ModelFile &World)
{
	Confidence Conf(Model, World, 1.0); //sphere size
	cout << "Total Information: " << Model.SumTotalInformation() << endl;
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Conf.DeductInformation(World.getCoord(0));
	
	cout << "Total Information: " << Model.SumTotalInformation() << endl;
	cout << "Remaining Information: " << Model.SumRemainingInformation() << endl;
	
	Model.Write("Colored.vtp");
	
}