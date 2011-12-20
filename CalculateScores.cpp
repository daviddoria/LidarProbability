#include <iostream>
#include <fstream>
#include <string>

#include "Confidence.h"
#include "Consistency.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <ModelFile/ModelFile.h>

#include <vul/vul_file.h>

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
  std::string SceneFilename, ModelFilename;
  std::string LogFilename;
  double SphereSize;

  //parse arguments
  po::options_description desc("Allowed options");
  desc.add_options()
                  ("help", "Help message.")
                  ("scene", po::value<std::string>(&SceneFilename), "Set scene file")
                  ("model", po::value<std::string>(&ModelFilename), "Set model file")
                  ("sphere", po::value<double>(&SphereSize), "Sphere size.")
                  ("log", po::value<std::string>(&LogFilename), "Sphere size.")
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

  Tools::Log L;
  if(vm.count("log"))
  {
    L.setFilename(LogFilename);
    L.Enable();
  }

  ModelFile Scene;
  Scene.Read(SceneFilename);
  Scene.Init();

  ModelFile Model;
  Model.Read(ModelFilename);
  Model.Init();

  std::string Filename = vul_file::strip_extension(vul_file::strip_directory(ModelFilename));

  if(!vm.count("sphere"))
  {
    SphereSize = Model.AverageBBEdge() / 20.0;
    std::cout << "Sphere size defaulted to " << SphereSize << std::endl;
  }

  std::cout << "Calculating Confidence..." << std::endl;
  Confidence Conf(Model, Scene, SphereSize);
  double conf = Conf.CalculateConfidence();
  std::cout << std::endl << std::endl;
  Conf.Model.Write(Filename + "_Confidence.vtp");

  std::cout << "Getting comparable model..." << std::endl;
  ComparableModels Comparable(Scene, Model);
  Comparable.Init();
  std::cout << std::endl << std::endl;

  std::cout << "Calculating Consistency..." << std::endl;
  Consistency C(Comparable, SphereSize);

  double consist = C.CalculateConsistency();
  Comparable.WriteMatchingPoints(Filename + "_ConsistencyScene.vtp", Filename + "_ConsistencyModel.vtp");
  Comparable.WriteConnectingLines(Filename + "_ConsistencyLines.vtp");
  std::cout << std::endl << std::endl;


  std::stringstream ss;
  ss << Filename << "_conf_consist_scores.txt";
  std::ofstream fout(ss.str().c_str());

  fout << conf << std::endl << consist << std::endl;

  fout.close();

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

}
