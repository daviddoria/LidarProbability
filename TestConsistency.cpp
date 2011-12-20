#include <iostream>
#include <fstream>
#include <string>

#include "Consistency.h"
//#include "Confidence.h"
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <LidarScan/LidarScanner.h>
#include <LidarScan/ScanParamsClass.h>

//#include <Integration/integration.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

void TestConsistency(ComparableModels Comparable, const double Mismatch);
void CheckRequiredArgs(const po::variables_map &vm);


int main(int argc, char *argv[])
{
  //Tools::Log L("log.txt");
  //L.Enable();

  std::string WorldFilename, ModelFilename;
  double Mismatch;
  //parse arguments
  po::options_description desc("Allowed options");
  desc.add_options()
                  ("help", "Help message.")
                  ("world", po::value<std::string>(&WorldFilename), "Set world file")
                  ("model", po::value<std::string>(&ModelFilename), "Set model file")
                  ("mismatch", po::value<double>(&Mismatch), "Mismatch allowance")
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

  ModelFile Model;
  Model.Read(ModelFilename);
  Model.Init();

  ModelFile World;
  World.Read(WorldFilename);
  World.Init();

  if(!vm.count("mismatch"))
  {
    Mismatch = Model.AverageBBEdge() / 15.0;
    std::cout << "Mismatch defaulted to " << Mismatch << std::endl;
  }

  vgl_point_3d<double> ScannerLocation;
  bool IsScan = World.getScannerLocation(ScannerLocation);

  if(!IsScan)
  {
    std::cout << "The world file is not a scan!" << std::endl;
    exit(-1);
  }

  std::clog << "Scan location: " << ScannerLocation << std::endl;

  ComparableModels Comparable(World, Model);

  TestConsistency(Comparable, Mismatch);

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

void TestConsistency(ComparableModels Comparable, const double Mismatch)
{
  std::cout << std::endl << "TestConsistency" << std::endl << "------------------" << std::endl;
  Comparable.Init();

  std::clog << Comparable << std::endl;

  Consistency C(Comparable, Mismatch);
  double consist = C.CalculateConsistency();
  //cout << Comparable << endl;
  Comparable.WriteMatchingPoints(Comparable.Model.getName() + "ConsistencyWorld.vtp", Comparable.Model.getName() + "ConsistencyModel.vtp");
  Comparable.WriteConnectingLines(Comparable.Model.getName() + "ConsistencyLines.vtp", Mismatch);

  std::cout << "Consistency: " << consist << std::endl;

}
