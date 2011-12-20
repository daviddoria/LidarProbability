#include <iostream>
#include <fstream>
#include <string>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <ModelFile/ModelFile.h>
#include <Geometry/Color.h>

#include <vul/vul_file.h>

#include <Tools/Tools.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

void CheckRequiredArgs(const po::variables_map &vm);

int main(int argc, char *argv[])
{
  std::string SceneFilename, ModelFilename;
  double mismatch;

  //parse arguments
  po::options_description desc("Allowed options");
  desc.add_options()
                  ("help", "Help message.")
                  ("scene", po::value<std::string>(&SceneFilename), "Set scene file")
                  ("model", po::value<std::string>(&ModelFilename), "Set model file")
                  ("mismatch", po::value<double>(&mismatch), "Set mismatch.")
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

  ModelFile Scene;
  Scene.Read(SceneFilename);
  Scene.Init();

  ModelFile Model;
  Model.Read(ModelFilename);
  Model.Init();

  if(Scene.NumPoints() != Model.NumPoints() )
  {
    std::cout << "Number of points do not match (points should already be correspondences.)!" << std::endl;
    exit(-1);
  }

  vgl_point_3d<double> ScannerLocation;
  bool IsScan = Scene.getScannerLocation(ScannerLocation);
  std::vector<Color<unsigned char> > Colors;

  for(unsigned int i = 0; i < Scene.NumPoints(); i++)
  {
    double s = (Scene.getCoord(i) - ScannerLocation).length();
    double m = (Model.getCoord(i) - ScannerLocation).length();
    if(s > (m + mismatch))
    {
      Colors.push_back(Colors::Red());
    }
    else
    {
      Colors.push_back(Colors::Green());
    }
  }

  ModelFile SceneColored;
  SceneColored.setCoords(Scene.getCoords());
  SceneColored.setColors(Colors);
  SceneColored.Write("SceneColored.vtp");

  ModelFile ModelColored;
  ModelColored.setCoords(Model.getCoords());
  ModelColored.setColors(Colors);
  ModelColored.Write("ModelColored.vtp");


  return 0;
}

void CheckRequiredArgs(const po::variables_map &vm)
{
  if(!vm.count("scene"))
  {
    std::cout << "Please specify a scene file." << std::endl;
    exit(-1);
  }

  if(!vm.count("model"))
  {
    std::cout << "Please specify a model file." << std::endl;
    exit(-1);
  }

  if(!vm.count("mismatch"))
  {
    std::cout << "Please specify a mismatch allowance." << std::endl;
    exit(-1);
  }

}
