#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "LidarProbability.h"
#include <ComparableModels/ComparableModels.h>

#include <LidarScan/LidarScanner.h>

#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

using namespace std;

void TestTx(vector<ModelFile> &Worlds, vector<ModelFile> &OriginalModels, const vector<LidarScanner> &Scanners, const string &Outfile);

//example call:
//BunnyWithWallScan Bunny TestTx.txt ModelTx

int main(int argc, char *argv[])
{
	//parse arguments
	
	string WorldFilename = argv[1];
	string ModelFilename = argv[2];
	string WhichTest = argv[3];
	
	ModelFile World(WorldFilename);
	ModelFile Model(ModelFilename);
	
	//setup scanner
	LidarScanner Scanner;
	vgl_vector_3d<double> Translation(0.0, 0.0, 10.0);
	vnl_double_3x3 R = MakeRotation('x', PI/2.0);
	Transformation Trans(Translation, R);
	Scanner.ScanParams.setTransformation(Trans);

	if(WhichTest.compare("Tx") == 0)
	{
		//TestTx(World, Model, Scanner, ProbFilename, OutputPrefix);
	}
	/*
	else if(WhichTest.compare("Ty") == 0)
		TestTy(World, Model, Scanner, ProbFilename, OutputPrefix);
	else if(WhichTest.compare("Tz") == 0)
		TestTz(World, Model, Scanner, ProbFilename, OutputPrefix);
	else if(WhichTest.compare("Rx") == 0)
		TestRx(World, Model, Scanner, ProbFilename, OutputPrefix);
	else if(WhichTest.compare("Ry") == 0)
		TestRy(World, Model, Scanner, ProbFilename, OutputPrefix);
	else if(WhichTest.compare("Rz") == 0)
		TestRz(World, Model, Scanner, ProbFilename, OutputPrefix);
	*/
	return 0;
}


void TestTx(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	int counter = 0;
	
	
	for(double x = -.1; x <= .1; x+=.001)
	{
		cout << "x = " << x << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(x, 0.0, 0.0);
		Transformation Trans(Translation);
		Model.Transform(Trans);
		stringstream ModelFilename;

		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		
		//create the comparable models
		ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << x << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void TestTy(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	unsigned int counter = 0;
	
	for(double y = -.1; y <= .1; y+=.001)
	{
		cout << "y = " << y << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, y, 0.0);
		Transformation Trans(Translation);
		Model.Transform(Trans);
		stringstream ModelFilename;

		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		
		//create the comparable models
		ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << y << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void TestTz(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	int counter = 0;
	
	//for(double z = -1.0; z <= 1.0; z+=.1)
	for(double z = -.1; z <= .1; z+=.001)
	{
		cout << "z = " << z << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, 0.0, z);
		Transformation Trans(Translation);
		Model.Transform(Trans);
		stringstream ModelFilename;

		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		
		//create the comparable models
ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << z << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void TestRx(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	int counter = 0;
	
	for(double rx = -PI; rx <= PI; rx+=PI/400.0)
	{
		cout << "rx = " << rx << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, 0.0, 0.0);
		vnl_double_3x3 R = MakeRotation('x', rx);
		Transformation Trans(Translation, R);
		
		Model.Transform(Trans);
		stringstream ModelFilename;

		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		//create the comparable models
ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << rx << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void TestRy(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	int counter = 0;
	
	for(double ry = -PI; ry <= PI; ry+=PI/400.0)
	{
		cout << "ry = " << ry << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, 0.0, 0.0);
		vnl_double_3x3 R = MakeRotation('y', ry);
		Transformation Trans(Translation, R);
		
		Model.Transform(Trans);
		stringstream ModelFilename;
		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		//create the comparable models
ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << ry << " " << P << endl;
		counter++;
	}

	fout.close();
	
}

void TestRz(ModelFile &World, ModelFile &OriginalModel, const LidarScanner &Scanner, const string &OutFile)
{

	ofstream fout(OutFile.c_str());
	int counter = 0;
	
	for(double rz = -PI; rz <= PI; rz+=PI/400.0)
	{
		cout << "rz = " << rz << endl;
		ModelFile Model = OriginalModel;
		
		vgl_vector_3d<double> Translation(0.0, 0.0, 0.0);
		vnl_double_3x3 R = MakeRotation('z', rz);
		Transformation Trans(Translation, R);
		
		Model.Transform(Trans);
		stringstream ModelFilename;
		/*
		ModelFilename << OutputPrefix << ZeroPad(counter, 3) << ".vtp";
		Model.Write(ModelFilename.str(), true);
		*/
		//create the comparable models
ComparableModels Comparable(World, Model, Scanner);
		
		double P = CalculateProbability(Comparable, .01);
		
		fout << rz << " " << P << endl;
		counter++;
	}

	fout.close();
	
}


void MultipleScanProbability()
{
#if 0


	//open the prob file
	ofstream fout(ProbFilename.c_str(), ios::out);
	
	int NumWorldScans = 9;
	
	const int Num = 74;
	vector<double> Probs(Num, 0);
	
	for(int WorldIndex = 1; WorldIndex <= NumWorldScans; WorldIndex++)
	{
		PercentComplete(WorldIndex, NumWorldScans, 1);

		for(int PositionIndex = 0; PositionIndex < Num; PositionIndex++)
		{
			/*
			if(PositionIndex != 50)
				continue;
			*/
		
			stringstream PosIndexStream;
			PosIndexStream << setfill('0') << setw(5) << PositionIndex;
			
			//read world file
			stringstream WorldStream;
			WorldStream << WorldFilename << "_" << WorldIndex << "_" << PosIndexStream.str() << ".vtp";
			ModelFile World;
			World.Read(WorldStream.str(), false);
			
			//read model file
			stringstream ModelStream;
			ModelStream << ModelFilename << "_" << WorldIndex << "_" << PosIndexStream.str() << ".vtp";
			ModelFile Model;
			Model.Read(ModelStream.str(), false);
					
			
			vgl_point_3d<double> CenterOfMass = Model.CenterOfMass();
			
			double P = ProbModel(World, Model);
			//Probs[PositionIndex] += P;
			
		}//end PositionIndex loop
		
	}//end WorldIndex loop
	
	
	fout.close();
#endif
}