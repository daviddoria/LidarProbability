#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Consistency.h"
#include "Confidence.h"
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vbl/vbl_array_2d.h>

#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_array_2d.h>

#include <LidarScan/LidarScanner.h>
#include <LidarScan/ScanParamsClass.h>

#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	const unsigned int MAX_LISTS = 8;
	double SphereSize;
	std::vector<string> ModelFileList;

	std::vector<vector<std::string> > ScanFileLists(MAX_LISTS);
	//parse arguments
	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "Help message.")
			("SphereSize", po::value<double>(&SphereSize)->default_value(.5), "Influence sphere size.")
			("ModelFileList", po::value<vector<std::string> >(&ModelFileList)->multitoken(), "List of model files.")
			("ScanFileList0", po::value<vector<std::string> >(&ScanFileLists[0])->multitoken(), "List 0 of scan files.")
			("ScanFileList1", po::value<vector<std::string> >(&ScanFileLists[1])->multitoken(), "List 1 of scan files.")
			("ScanFileList2", po::value<vector<std::string> >(&ScanFileLists[2])->multitoken(), "List 2 of scan files.")
			("ScanFileList3", po::value<vector<std::string> >(&ScanFileLists[3])->multitoken(), "List 3 of scan files.")
			("ScanFileList4", po::value<vector<std::string> >(&ScanFileLists[4])->multitoken(), "List 4 of scan files.")
			("ScanFileList5", po::value<vector<std::string> >(&ScanFileLists[5])->multitoken(), "List 5 of scan files.")
			("ScanFileList6", po::value<vector<std::string> >(&ScanFileLists[6])->multitoken(), "List 6 of scan files.")
			("ScanFileList7", po::value<vector<std::string> >(&ScanFileLists[7])->multitoken(), "List 7 of scan files.")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm); //assign the variables (if they were specified)
	
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 1;
	}
	
	std::cout << "Model File List:" << std::endl;
	for(unsigned int i = 0; i < ModelFileList.size(); i++)
	{
		std::cout << ModelFileList[i] << std::endl;
	}

	unsigned int NumSceneObjects = ScanFileLists[0].size();
	for(unsigned int i = 0; i < MAX_LISTS; i++)
	{
		if(ScanFileLists[i].size() == 0)
		{
			ScanFileLists.resize(i);
			break;
		}

		if(ScanFileLists[i].size() != NumSceneObjects)
		{
			std::cout << "The SceneFileLists are not all the same size." << std::endl;
			std::cout << "ScanFileLists[" << i << "] is length " << ScanFileLists[i].size() << std::endl;
			std::cout << "NumSceneObjects is " << NumSceneObjects << std::endl;
			return 1;
		}
	}

	unsigned int NumScans = ScanFileLists.size();
	std::cout << "There are " << NumScans << "file lists with " << NumSceneObjects << " each." << std::endl;

	for(unsigned int list = 0; list < NumScans; list++)
	{
		std::cout << "Scan File List " << list << ":" << endl;
		for(unsigned int i = 0; i < ScanFileLists[list].size(); i++)
		{
			std::cout << ScanFileLists[list][i] << std::endl;
		}
	}
			
	unsigned int NumModels = ModelFileList.size();
	
	vbl_array_2d<double> ConfidenceMatrix(NumModels, NumSceneObjects, 0.0);

	std::vector<vbl_array_2d<double> > Individual(NumScans, ConfidenceMatrix); //initialize with the empty matrices of the correct size
	std::vector<vbl_array_2d<double> > RunningTotal(NumScans, ConfidenceMatrix); //initialize with the empty matrices of the correct size

	
	for(unsigned int model = 0; model < NumModels; model++) //go through all the models
	//unsigned int model = 0;
	{
		std::cout << std::endl << "****************" << std::endl << "Model " << model << " of " << NumModels << std::endl << std::endl;
		ModelFile RunningTotalModel(ModelFileList[model]);
		RunningTotalModel.BuildKDTree();
		
		ModelFile IndividualModel(ModelFileList[model]);
		IndividualModel.BuildKDTree();
		std::cout << "KDTree built." << endl;
				
		for(unsigned int scene = 0; scene < NumSceneObjects; scene++) //compare the current model to all of the model scans
		//unsigned int scene = 0;
		{
			RunningTotalModel.ResetInformation();
			
			for(unsigned int scan = 0; scan < NumScans; scan++) //use multiple scans of the same model (to increase confidence)
			{
				std::cout << "------------------" << std::endl << "Calculating Confidence " << model << "_" << scene << "_" << scan << std::endl
						<< "-----------------" << std::endl;
				
				IndividualModel.ResetInformation();
				
				ModelFile World(ScanFileLists[scan][scene]);

				//this find the cumulative confidence (after seeing scans 0 - scan)
				Confidence RunningTotalConf(RunningTotalModel, World, SphereSize);
				double RunningTotalConfidence = RunningTotalConf.CalculateConfidence();
				
				Confidence IndividualConf(IndividualModel, World, SphereSize);
				double IndividualConfidence = IndividualConf.CalculateConfidence();

				RunningTotal[scan](model, scene) = RunningTotalConfidence;
				Individual[scan](model, scene) = IndividualConfidence;
				
				//write the model with information used after this scan
				std::stringstream RunningTotalName;
				RunningTotalName << "Confidence_RunningTotal_" << model << "_" << scene << "_" << scan << ".vtp";
				RunningTotalModel.Write(RunningTotalName.str());
				
				std::stringstream IndividualName;
				IndividualName << "Confidence_Individual_" << model << "_" << scene << "_" << scan << ".vtp";
				IndividualModel.Write(IndividualName.str());
			}
		}
	}

	for(unsigned int i = 0; i < NumScans; i++)
	{
		//write individual confidence files
		stringstream ssIndividualFilename;
		ssIndividualFilename << "Confidence_Individual_" << i;
		string IndividualPNG = ssIndividualFilename.str() + ".png";
		VXLHelpers::Write2DArrayImage(Individual[i], IndividualPNG);
		std::cout << "Wrote " << IndividualPNG << std::endl;

		string IndividualTXT = ssIndividualFilename.str() + ".txt";
		Tools::OutputObject(Individual[i], IndividualTXT);
		std::cout << "Wrote " << IndividualTXT << std::endl;
		
		//write running total confidence files
		stringstream ssRunningTotalFilename;
		ssRunningTotalFilename << "Confidence_RunningTotal_" << i;
		string RunningTotalPNG = ssRunningTotalFilename.str() + ".png";
		VXLHelpers::Write2DArrayImage(RunningTotal[i], RunningTotalPNG);
		std::cout << "Wrote " << RunningTotalPNG << std::endl;
		
		std::string RunningTotalTXT = ssRunningTotalFilename.str() + ".txt";
		Tools::OutputObject(RunningTotal[i], RunningTotalTXT);
		std::cout << "Wrote " << RunningTotalTXT << std::endl;
		
		
	}

	return 0;
}
