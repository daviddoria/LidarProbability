#include <iostream>
#include <fstream>
#include <string>

#include "Consistency.h"
//#include "Confidence.h"
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vbl/vbl_array_2d.h>

#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_array_2d.h>

//#include <LidarScan/LidarScanner.h>
//#include <LidarScan/ScanParamsClass.h>

//#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{

  const unsigned int MAX_LISTS = 8;

  std::vector<std::string> ModelFileList;

  std::vector<std::vector<std::string> > ScanFileLists(MAX_LISTS);

  double Mismatch;

  //parse arguments
  po::options_description desc("Allowed options");
  desc.add_options()
                  ("help", "Help message.")
                  ("mismatch", po::value<double>(&Mismatch)->default_value(1.0), "Set mismatch allowance.")
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

  std::cout << "Mismatch: " << Mismatch << std::endl;

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
      std::cout << "ScanFileLists[" << i << "] is " << ScanFileLists[i].size() << std::endl;
      std::cout << "NumSceneObjects is " << NumSceneObjects << std::endl;
      return 1;
    }
  }

  unsigned int NumScans = ScanFileLists.size();
  std::cout << "There are " << NumScans << "file lists with " << NumSceneObjects << " each." << std::endl;

  for(unsigned int list = 0; list < NumScans; list++)
  {
    std::cout << "Scan File List " << list << ":" << std::endl;
    for(unsigned int i = 0; i < ScanFileLists[list].size(); i++)
    {
      std::cout << ScanFileLists[list][i] << std::endl;
    }
  }

  unsigned int NumModels = ModelFileList.size();


  vbl_array_2d<double> EmptyMatrix(NumModels, NumSceneObjects, 0.0);
  std::vector<vbl_array_2d<double> > Individual(NumScans, EmptyMatrix); //initialize with the empty matrices of the correct size
  std::vector<vbl_array_2d<double> > RunningTotal(NumScans, EmptyMatrix);

  for(unsigned int model = 0; model < NumModels; model++)
  //unsigned int model = 0;
  {
    std::cout << std::endl << "****************" << std::endl << "Model " << model << " of " << NumModels << std::endl << std::endl;
    ModelFile Model(ModelFileList[model]);
    Model.BuildOctree();
    //cout << "octree built." << endl;

    for(unsigned int scene = 0; scene < NumSceneObjects; scene++) //check consistency of each location we are alleging is a car
    //unsigned int scene = 0;
    {
      double TotalIdeal = 0.0;
      double TotalConsistency = 0.0;
      for(unsigned int scan = 0; scan < NumScans; scan++) //check consistency multiple scans of the same model
      {
        std::cout << "------------------" << std::endl << "Calculating Consistency " << model << "_" << scene << "_" << scan << std::endl
                        << "-----------------" << std::endl;
        double consist = 0.0;
        double Ideal = 0.0;
        /*
        stringstream ssConsistencyRunningTotal;
        ssConsistencyRunningTotal << "Consistency_RunningTotal_" << model << "_" << scene << "_" << scan << ".vtp";

        stringstream ssConsistencyIndividual;
        ssConsistencyIndividual << "Consistency_Individual_" << model << "_" << scene << "_" << scan << ".vtp";
        */
        std::stringstream ssConsistency;
        ssConsistency << "Consistency_" << model << "_" << scene << "_" << scan << ".vtp";

        //if(!Tools::FileExists(ssConsistency.str())) //we are assuming if one is there, the other is also there
        //{
            ModelFile World(ScanFileLists[scan][scene]);

            ComparableModels Comparable(World, Model);

            Consistency C(Comparable, Mismatch);
            consist = C.CalculateConsistency();

            //write the world with comparison info
            Comparable.World.Write(ssConsistency.str());

            Ideal = C.IdealScore();
            /*
          }
        else //the file already exists
        {
          vector<float> CD = GetFloatArrayFromPolyData(ssConsistency.str(), "ConsistencyDistances");
          vector<double> ConsistencyDistances(CD.size());
          unsigned int count = 0;
          for(unsigned int i = 0; i < CD.size(); i++)
          {
            ConsistencyDistances[i] = static_cast<double>(CD[i]);
            if(fabs(CD[i] > .1))
            {
              count++;
            }
          }
          std::cout << "There were " << count << " odd points." << endl;

          std::cout << "This is not implemented." << std::endl; exit(-1);
          //Consistency = CalculateTotalConsistency(ConsistencyDistances, Mismatch);
          //Ideal = IdealScore(ConsistencyDistances.size(), Mismatch);
        }
        */

        TotalConsistency += consist;

        TotalIdeal += Ideal;
        //cout << "calculated score." << endl;
        double SingleValue = consist/Ideal;
        Individual[scan](model, scene) = SingleValue;
        double RunningTotalCurrentValue = TotalConsistency/TotalIdeal;
        RunningTotal[scan](model, scene) = RunningTotalCurrentValue;
        //cout << "matrix value (" << model << ", " << scene << ") set to " << Val << "." << endl;

      } // end scan for

    } // end scene for

  } // end model for

  for(unsigned int i = 0; i < NumScans; i++)
  {
    //Individuals
    std::stringstream ssIndividualFilename;
    ssIndividualFilename << "Consistency_Individual_" << i;
    string IndividualPNG = ssIndividualFilename.str() + ".png";
    VXLHelpers::Write2DArrayImage(Individual[i], IndividualPNG);
    std::cout << "Wrote " << IndividualPNG << std::endl;

    std::string IndividualTXT = ssIndividualFilename.str() + ".txt";
    Tools::OutputObject(Individual[i], IndividualTXT);
    std::cout << "Wrote " << IndividualTXT << std::endl;

    //Running totals
    std::stringstream ssRunningTotalFilename;
    ssRunningTotalFilename << "Consistency_RunningTotal_" << i;
    std::string RunningTotalPNG = ssRunningTotalFilename.str() + ".png";
    VXLHelpers::Write2DArrayImage(RunningTotal[i], RunningTotalPNG);
    std::cout << "Wrote " << RunningTotalPNG << std::endl;

    std::string RunningTotalTXT = ssRunningTotalFilename.str() + ".txt";
    Tools::OutputObject(RunningTotal[i], RunningTotalTXT);
    std::cout << "Wrote " << RunningTotalTXT << std::endl;
  }

  return 0;
}
