#include <iostream>
#include <fstream>
#include <string>

#include "Consistency.h"
#include "Confidence.h"
#include <ComparableModels/ComparableModels.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <LidarScan/LidarScanner.h>
#include <LidarScan/ScanParamsClass.h>

#include <ModelFile/ModelFile.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

void CheckRequiredArgs(const po::variables_map &vm);

int main(int argc, char *argv[])
{
//	Tools::Log L("log.txt");
//	L.Enable();

  std::string WorldFilename, ModelFilename;

  //parse arguments
  po::options_description desc("Allowed options");
  desc.add_options()
                  ("help", "Help message.")
                  ("world", po::value<std::string>(&WorldFilename), "Set world file")
                  ("model", po::value<std::string>(&ModelFilename), "Set model file")
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

  ModelFile Model(ModelFilename);
  ModelFile World(WorldFilename);

  vgl_point_3d<double> ScannerLocation;
  bool IsScan = World.getScannerLocation(ScannerLocation);

  if(!IsScan)
  {
    std::cout << "The world file is not a scan!" << std::endl;
    exit(-1);
  }

  std::clog << "Scan location: " << ScannerLocation << std::endl;

  //cull the world points by seeing which world points are in the visual hull (visual frustrum) of the model
  vgl_box_3d<double> ModelBox = Model.GetBoundingBox();
  Model.WriteBoundingBox("BoudingBox.vtp");
  std::vector<vgl_point_3d<double> > WorldCoords = World.getCoords();
  std::vector<unsigned int> UsedWorldIndices = geom::IndicesInsideFrustrum(ModelBox, WorldCoords, ScannerLocation);
  std::vector<vgl_point_3d<double> > UsedWorldCoords;
  for(unsigned int i = 0; i < UsedWorldIndices.size(); i++)
  {
    UsedWorldCoords.push_back(World.getCoord(UsedWorldIndices[i]));
  }

  ModelFile WPoints;
  WPoints.setCoords(UsedWorldCoords);
  WPoints.Write("WorldPoints.vtp");

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
