#include <iostream>
#include <fstream>
#include <string>

#include "LidarProbability.h"
#include <ComparableModels/ComparableModels.h>

#include <LidarScan/LidarScanner.h>

#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

using namespace std;

void TestTranslation(const ModelFile &World, const ModelFile &OriginalModel, const LidarScanner &Scanner, const char Which);
void TestRotation(const ModelFile &World, const ModelFile &OriginalModel, const LidarScanner &Scanner, const char Which);

//example call:
//BunnyWithWallScan.vtp Bunny.vtp Tx

int main(int argc, char *argv[])
{
	//parse arguments
	Tools::AssertNumArgs(argc, 3);
	
	string WorldFilename = argv[1];
	string ModelFilename = argv[2];
	string WhichTest = argv[3];
	
	ModelFile World(WorldFilename);
	ModelFile Model(ModelFilename);
	
	//setup scanner
	LidarScanner Scanner(true); //true = use octree
	/*
	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	vnl_double_3x3 R = MakeRotation('x', PI/2.0);
	Transformation Trans(Translation, R);
	Scanner.ScanParams.setTransformation(Trans);
	*/
	
	if(WhichTest.compare("Tx") == 0)
		TestTranslation(World, Model, Scanner, 'x');
	else if(WhichTest.compare("Ty") == 0)
		TestTranslation(World, Model, Scanner, 'y');
	else if(WhichTest.compare("Tz") == 0)
		TestTranslation(World, Model, Scanner, 'z');
	else if(WhichTest.compare("Rx") == 0)
		TestRotation(World, Model, Scanner, 'x');
	else if(WhichTest.compare("Ry") == 0)
		TestRotation(World, Model, Scanner, 'y');
	else if(WhichTest.compare("Rz") == 0)
		TestRotation(World, Model, Scanner, 'z');
	
	return 0;
}


void TestTranslation(const ModelFile &World, const ModelFile &OriginalModel, const LidarScanner &Scanner, const char Which)
{
	stringstream ProbFileStream;
	ProbFileStream << "ProbT" << Which << ".txt";
	string ProbFile = ProbFileStream.str();
	ofstream fout(ProbFile.c_str());
	
	int counter = 0;
	
	//for(double var = -1.0; var <= 1.0; var += .1) //coarse
	for(double var = -0.2; var <= 0.2; var += .01) //fine
	//double var = -.2;
	{
		cout << "var = " << var << endl;
		ModelFile Model = OriginalModel;
		
		
		vgl_vector_3d<double> Translation;
		
		if(Which == 'x')
			Translation = vgl_vector_3d<double> (var, 0.0, 0.0);
		else if(Which == 'y')
			Translation = vgl_vector_3d<double> (0.0, var, 0.0);
		else if(Which == 'z')
			Translation = vgl_vector_3d<double> (0.0, 0.0, var);
		else
			assert(0);
		
		Transformation Trans(Translation);
		Model.Transform(Trans);
		stringstream ModelFilename;
		Model.Write("Model.vtp");
		
		ModelFilename << "T" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str());
		
		//create the comparable models
		ModelFile CompatWorld, CompatModel;
		ComparableModels Comparable(World, Model, Scanner);
		
		Comparable.Model.Write("CompatModel.vtp");
		Comparable.World.Write("CompatWorld.vtp");

		
		double P = CalculateProbability(Comparable, .1);
		
		Comparable.Model.Write("CompatModelColored.vtp");
		Comparable.World.Write("CompatWorldColored.vtp");
		
		stringstream ModelPointsFilename;
		stringstream WorldPointsFilename;
		ModelPointsFilename << "ModelPointsT" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		WorldPointsFilename << "WorldPointsT" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		Comparable.Model.Write(ModelPointsFilename.str());
		Comparable.World.Write(WorldPointsFilename.str());

		
		fout << var << " " << P << endl;
		counter++;
	}

	fout.close();
	
}

void TestRotation(const ModelFile &World, const ModelFile &OriginalModel, const LidarScanner &Scanner, const char Which)
{
	stringstream ProbFileStream;
	ProbFileStream << "ProbR" << Which << ".txt";
	string ProbFile = ProbFileStream.str();
	ofstream fout(ProbFile.c_str());
	
	int counter = 0;
	
	//double step = PI/20.0;
	//for(double angle = -PI; angle <= PI; angle += PI/20.0)
	for(double angle = -PI/4.0; angle <= PI/4.0; angle += PI/100.0)
	{
		cout << "angle = " << angle << endl;
		ModelFile Model = OriginalModel;
		cout << "Center of mass: " << Model.getCenterOfMass() << endl;
		
		vnl_double_3x3 R = MakeRotation(Which, angle);
		Model.RotateAroundCenter(R);
		//Transformation Trans(R);
		//Model.Transform(Trans);
		
		
		stringstream ModelFilename;
		ModelFilename << "R" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str());
		
		//create the comparable models
		ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .1);
		
		stringstream ModelPointsFilename;
		stringstream WorldPointsFilename;
		ModelPointsFilename << "ModelPointsR" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		WorldPointsFilename << "WorldPointsR" << Which << Tools::ZeroPad(counter, 3) << ".vtp";
		Comparable.Model.Write(ModelPointsFilename.str());
		Comparable.World.Write(WorldPointsFilename.str());

		fout << angle << " " << P << endl;
		counter++;
	}

	fout.close();
	
}

