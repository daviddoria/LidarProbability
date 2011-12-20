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

#include <SDL/SDL.h>
#include <SDLHelpers/SDLHelpers.h>

#include <cmath>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;

void KeepShell(ModelFile &Model, const vector<LidarScanner> &Scanners, const string &OutputFilename);
vector<LidarScanner> CreateScanners(const LidarScanner Scanner);

//usage
//KeepShell --input in.vtp --output out.vtp

int main(int argc, char* argv[])
{
	string InputFile, OutputFile;

	cout << "Options:" << endl;
	
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("input", po::value<string>(), "Set input file")
			("output", po::value<string>(), "Set output file")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	
	if (vm.count("help")) 
	{
		cout << desc << endl;
		return 1;
	}

	if (vm.count("input")) 
	{
		InputFile = vm["input"].as<string>();
		cout << "Input file: " << InputFile << endl;
	} 
	else 
	{
		cout << "Please specify an input file." << endl;
		return 1;
	}
	
	if (vm.count("output")) 
	{
		OutputFile = vm["output"].as<string>();
		cout << "Output file: " << OutputFile << endl;
	} 
	else 
	{
		cout << "Please specify an output file." << endl;
		return 1;
	}
	
	cout << endl << endl;

	/////////////////////////////////////////////////////
	
	//setup model
	ModelFile Model(InputFile);
	
	//setup scanner
	//LidarScanner Scanner(true); //use octree
	//Model.BuildOctree();
	
	LidarScanner Scanner(false); //do not use octree (easier to track triangles)
	
	unsigned int num = 200;
	Scanner.ScanParams.setNumPhiPoints(num);
	Scanner.ScanParams.setNumThetaPoints(num);
	
	double minmax = deg2rad(40.0);
	Scanner.ScanParams.setPhiMin(-minmax);
	Scanner.ScanParams.setPhiMax(minmax);
	Scanner.ScanParams.setThetaMin(-minmax);
	Scanner.ScanParams.setThetaMax(minmax);
	
	vector<LidarScanner> Scanners = CreateScanners(Scanner);
			
	//scan
	KeepShell(Model, Scanners, OutputFile);
	return 0;

}

vector<LidarScanner> CreateScanners(const LidarScanner Scanner)
{
	vector<LidarScanner> Scanners(6);
	vector<vgl_point_3d<double> > Points(6);
	double dist = 15.0;
	Points[0] = vgl_point_3d<double>(dist, 0.0, 0.0);
	Points[1] = vgl_point_3d<double>(0.0, dist, 0.0);
	Points[2] = vgl_point_3d<double>(0.0, 0.0, dist);
	Points[3] = vgl_point_3d<double>(-dist, 0.0, 0.0);
	Points[4] = vgl_point_3d<double>(0.0, -dist, 0.0);
	Points[5] = vgl_point_3d<double>(0.0, 0.0, -dist);
	
	Ray ScannerDefault(vgl_point_3d<double>(0.0, 0.0, 0.0), vgl_vector_3d<double>(0.0, 1.0, 0.0));
	
	for(unsigned int i = 0; i < 6; i++)
	{
		LidarScanner NewScanner = Scanner;
		
		vgl_vector_3d<double> Dir = vgl_point_3d<double>(0.0, 0.0, 0.0) - Points[i];
		Ray R2(Points[i], Dir);
		vnl_matrix<double> M = geom::CameraTransform(ScannerDefault, R2);
	
		vnl_double_3x3 R = Get3x3SubMatrix(M);

		//extract the first three entires of the last column (the translation vector)
		vgl_vector_3d<double> Translation(M.get(0,3), M.get(1,3), M.get(2,3));
	
		Transformation Trans(Translation, R);
		NewScanner.ScanParams.setTransformation(Trans);
		
		Scanners[i] = NewScanner;
	}

	return Scanners;
}

void KeepShell(ModelFile &Model, const vector<LidarScanner> &Scanners, const string &OutputFilename)
{
	vector<bool> KeepTri(Model.NumTriangles(), false);
	
	for(unsigned i = 0; i < Scanners.size(); i++)
	//for(unsigned i = 0; i < 1; i++)
	{
		LidarScanner Scanner = Scanners[i];
		LidarScan Scan = Scanner.AcquireScene(Model);
		for(unsigned int t = 0; t < Scanner.ScanParams.getNumThetaPoints(); t++)
		{
			for(unsigned int p = 0; p < Scanner.ScanParams.getNumPhiPoints(); p++)
			{
				LidarPoint LP = Scan.getPoint(t, p);
				if(LP.getHit() == true)
				{
					//cout << "Hit Tri: " << LP.HitTri_ << " (out of " << Model.NumTriangles() << ")" << endl;
					KeepTri[LP.HitTri_] = true;
				}
			}
		}
	}
	
	vector<vector<unsigned int> > TriList = Model.getVertexLists();
	
	vector<vector<unsigned int> > NewTriList;
	for(unsigned int i = 0; i < TriList.size(); i++)
	{
		if(KeepTri[i])
			NewTriList.push_back(TriList[i]);
	}
	
	ModelFile OutputModel = Model;
	OutputModel.setVertexLists(NewTriList);
	
	OutputModel.Write(OutputFilename);
}
