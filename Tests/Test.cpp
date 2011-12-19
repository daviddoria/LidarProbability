#include <iostream>
#include <fstream>
#include <string>

#include "LidarProbability.h"
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <LidarScan/LidarScanner.h>
#include <LidarScan/ScanParamsClass.h>

#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <valgrind/callgrind.h>

using namespace std;

void OnePointProbability();
void ScanProbability(const ModelFile &World, const ModelFile &Model);
void MultipleScanProbability();
void TestCreateComparableModels(const ModelFile &World, const ModelFile &OriginalModel);
void SimpleComparableModels();
void TestProbsOnLine(const ModelFile &World, const ModelFile &OriginalModel, const string &OutFile, const string &OutputPrefix);
void TestProbsOnGrid(const ModelFile &World, const ModelFile &OriginalModel, const string &OutFile, const string &OutputPrefix);

void TestCalculateProbsOnLine();

void EvaluateMultipleModels(const ModelFile &World, const string &InputPrefix, const string &OutputFile);

//BunnyWithWallScan.vtp Bunny.vtp LineSumProb.txt BunnyModelLine
int main(int argc, char *argv[])
{
	//parse arguments
	Tools::AssertNumArgs(argc, 3);
	string WorldFilename = argv[1];
	string ModelFilename = argv[2];
	string ProbFilename = argv[3];
	//string OutputPrefix = argv[4];
	
	//OnePointProbability();
	
	//TestCalculateProbsOnLine();

	ModelFile World(WorldFilename);
	//ModelFile Model(ModelFilename);
	
	//TestCreateComparableModels(World, Model);
	//ScanProbability(World, Model);
	
	//TestProbsOnLine(World, Model, ProbFilename, OutputPrefix);
	//TestProbsOnGrid(World, Model, ProbFilename, OutputPrefix);

	EvaluateMultipleModels(World, ModelFilename, ProbFilename);
	return 0;
}


void OnePointProbability()
{
	//Prob P(9, 10, .1, .1);
	Prob P(10.0, 11.0, .1);

	cout << P.Probability() << endl;
}

void ScanProbability(const ModelFile &World, const ModelFile &Model)
{	
	cout << "Test ScanProbability" << endl << "------------------" << endl;
	
	LidarScanner Scanner(true); //use octree
	
	ComparableModels Comparable(World, Model, Scanner);

	double p = CalculateProbability(Comparable, .2);
	
	cout << "Probability: " << p << endl;

	Comparable.World.Write("ColoredComparableWorld.vtp");
	Comparable.Model.Write("ColoredComparableModel.vtp");
}

void TestCreateComparableModels(const ModelFile &World, const ModelFile &Model)
{
	//LidarScanner Scanner(false);
	LidarScanner Scanner(true); //octree

	ComparableModels Comparable(World, Model, Scanner);

	Model.Write("ModelOctree.vtp");
	
	//write the compatable files
	Comparable.World.Write("CompatWorld.vtp");

	Comparable.Model.Write("CompatModel.vtp");

}

void SimpleComparableModels()
{
	vector<vgl_point_3d<double> > WorldPoints;
	vgl_point_3d<double> P0(-.5, -.5, -3.0);
	vgl_point_3d<double> P1(-.5, .5, -3.0);
	vgl_point_3d<double> P2(.5, -.5, -3.0);
	vgl_point_3d<double> P3(.5, .5, -3.0);
	vgl_point_3d<double> P4(4.0, 4.0, -3.0);
	vgl_point_3d<double> P5(-4.0, -4.0, 3.0);
	WorldPoints.push_back(P0);
	WorldPoints.push_back(P1);
	WorldPoints.push_back(P2);
	WorldPoints.push_back(P3);
	WorldPoints.push_back(P4);
	WorldPoints.push_back(P5);
	
	ModelFile World(WorldPoints);
	World.Write("/media/portable/Data/Cube/WorldPoints.vtp");
	
	ModelFile Model("/media/portable/Data/Cube/Cube.vtp");

	//create the comparable models
	ModelFile CompatWorld, CompatModel;

	LidarScanner Scanner(false);
	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	vnl_double_3x3 R = MakeRotation('x', PI/2.0);
	Transformation Trans(Translation, R);
	Scanner.ScanParams.setTransformation(Trans);
	
	ComparableModels Comparable(World, Model, Scanner);

	//write the compatable files
	Comparable.World.Write("/media/portable/Data/Cube/CompatWorld.vtp");

	Comparable.Model.Write("/media/portable/Data/Cube/CompatModel.vtp");

}

void TestCalculateProbsOnLine()
{
	//world 10, model 11 to 9
	for(double z = 11.0; z >= 9.0; z-=.1)
	{
		
		Prob P(10.0, z, .3); //(world, model, clutter, mismatch)
		cout << "z = " << z << " P: " << P.Probability() << " log(P): " << log10(P.Probability()) << endl;
	}
	
}


void TestProbsOnLine(const ModelFile &World, const ModelFile &OriginalModel, const string &OutFile, const string &OutputPrefix)
{
//BunnyWithWallScan.vtp Bunny.vtp LineProb_C=0_M=.01.txt BunnyModelLine
	ofstream fout(OutFile.c_str());

	vector<vgl_point_3d<double> > Points;

	LidarScanner Scanner(false);
	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	vnl_double_3x3 R = MakeRotation('x', PI/2.0);
	Transformation Trans(Translation, R);
	Scanner.ScanParams.setTransformation(Trans);

	int counter = 0;
	//for(double z = -1.0; z <= 1.0; z+=.1)
	for(double z = -.1; z <= .1; z+=.01)
	{
		cout << "z = " << z << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, 0.0, z);
		Transformation Trans(Translation);
		Model.Transform(Trans);
		stringstream ModelFilename;
		
		ModelFilename << OutputPrefix << Tools::ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str());
		
		//create the comparable models
		ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		//cout << "z= " << z << " P: " << P << endl;
		//cout << P << endl;
		fout << z << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void TestProbsOnGrid(const ModelFile &World, const ModelFile &OriginalModel, const string &OutFile, const string &OutputPrefix)
{
//BunnyWithWallScan.vtp Bunny.vtp GridSumProb,M=.01.txt BunnyModelGrid
	ofstream fout(OutFile.c_str());

	LidarScanner Scanner(false);
	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	vnl_double_3x3 R = MakeRotation('x', PI/2.0);
	Transformation Trans(Translation, R);
	Scanner.ScanParams.setTransformation(Trans);

	int counter = 0;
	for(double x = -1.0; x<=1.0; x+=.1)
	{
		cout << x << endl;
		for(double z = -1.0; z <= 1.0; z+=.1)
		{
			ModelFile Model = OriginalModel;
			
			vgl_vector_3d<double> Translation(x, 0.0, z);
			Transformation Trans(Translation);
			Model.Transform(Trans);
			stringstream ModelFilename;
			
			ModelFilename << OutputPrefix << Tools::ZeroPad(counter, 3) << ".vtp";
			Model.Write(ModelFilename.str());
			
			//create the comparable models
			ComparableModels Comparable(World, Model, Scanner);
			
			double P = CalculateProbability(Comparable, .01);
			//cout << "z= " << z << " P: " << P << endl;
			//cout << P << endl;
			fout << x << " " << z << " " << P << endl;
			counter++;
		}
	}
	fout.close();
	
}

void EvaluateMultipleModels(const ModelFile &World, const string &InputPrefix, const string &OutputFile)
{
	cout << "Evaluate Multiple Models" << endl << "------------------" << endl;
	ofstream fout(OutputFile.c_str());
	
	LidarScanner Scanner(true); //fast
	vgl_point_3d<double> Pos (42.4165, 44.72, 3.93524);
	Scanner.ScanParams.setLocation(Pos);

	stringstream FilenameStream;
	stringstream ComparableFilestream;
	for(unsigned int i = 1; i <= 6; i++)
	{
		FilenameStream.str("");
		FilenameStream << InputPrefix << i << ".vtp";
		ModelFile Model(FilenameStream.str());
		
		ComparableModels Comparable(World, Model, Scanner);
		Comparable.ModelNumber = i;

		double Mismatch = 0.1;
		double Ideal = IdealScore(Comparable, Mismatch);
		double p = CalculateProbability(Comparable, Mismatch);
		
		fout << p << " " << p/Ideal << " " << Comparable.GoodPoints << " " << Comparable.BadPoints << " " << Comparable.UninformativePoints << endl;

		ComparableFilestream.str("");
		ComparableFilestream << "ComparableWorld" << i << ".vtp";
		Comparable.World.Write(ComparableFilestream.str());

		ComparableFilestream.str("");
		ComparableFilestream << "ComparableModel" << i << ".vtp";
		Comparable.Model.Write(ComparableFilestream.str());

	}
}