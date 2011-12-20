#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

#include "Confidence.h"

#include <omp.h>

//#include <GL/glut.h>

#include <vgl/vgl_distance.h>

#include <VXLHelpers/VXLHelpers.h>

#include <Tools.h>

#include <LidarScan/LidarScanner.h>
#include <ModelFile/ModelFile.h>

#include <Integration/integration.h>

#include <Geometry/Color.h>
#include <Geometry/Transformation.h>
#include <Geometry/LineSegment.h>

#include <boost/progress.hpp>

Confidence::Confidence(const ModelFile &model, const ModelFile &world, const double spheresize) :
Model(model), World(world), SphereSize(spheresize), ShowProgress(true)
{}

double Confidence::CalculateConfidence()
{
  //inputs: a ModelFile which has been annotated with information contributions at each point
  //outputs: a Confidence value (0,1)

  if(Model.getTotalInformation() == 0.0)
  {
    std::cout << "File " << Model.getName() << " contains no informations! Quitting..." << std::endl;
    exit(-1);
  }

  std::clog << "Calculating Confidence" << endl << "---------------" << endl;
  double PreConfidence = 1.0 - Model.getRemainingInformation();
  std::clog << "Confidence before scan: " << PreConfidence << endl;

  DeductAllPointInformation();

  //if there is no remaining information, this means all the information has been "seen" so the confidence is high
  double RemainingInfo = Model.getRemainingInformation();
  if(RemainingInfo > 1.0)
  {
    RemainingInfo = 1.0;
  }

  double conf = 1.0 - RemainingInfo;

  std::clog << "RemainingInfo: " << RemainingInfo << std::endl;
  std::clog << "Confidence after scan: " << conf << std::endl;

  if(conf < 0.0 || conf > 1.0)
  {
    std::cout << "Confidence is out of range at " << conf << std::endl;
    exit(-1);
  }

  return conf;
}

void Confidence::DeductInformation(const vgl_point_3d<double> &ModelIntersection)
{
  //"use" point information for all model points within the sphere of influence of the intersection point

  //find the closest points on the model to the point that was the model intersection
  //vector<unsigned int> NeighborIndices = ModelTree.KNearestIndices(ModelIntersection, NumNeighbors);
  std::vector<unsigned int> NeighborIndices = Model.KDTree_.IndicesWithinRadius(SphereSize, ModelIntersection);

  for(unsigned int i = 0; i < NeighborIndices.size(); i++)
  {
    double dist = vgl_distance(Model.Points_[NeighborIndices[i]].getCoord(), ModelIntersection);
    //cerr << endl << "Deducting Info" << endl << "-------------" << endl;
    //cerr << "SphereSize: " << SphereSize << endl;

    //reduce the available point information based on the distance from the seen point
    //double Amount = Tools::ZeroMeanGaussian(dist, Model.getSampleSpacing());
    unsigned int CurrentPoint = NeighborIndices[i];

    double weight = Tools::ZeroMeanGaussian(dist, SphereSize);
    //cerr << "Weight: " << weight << endl;

    //cerr << "Before use, remaining: " << Model.Points_[CurrentPoint].getRemainingInformation() << endl;
    Model.Points_[CurrentPoint].UseInformationPercent(weight);
    //cerr << "After use, remaining: " << Model.Points_[CurrentPoint].getRemainingInformation() << endl;
  }
}

void Confidence::DeductAllPointInformation(void)
{
  Model.BuildKDTree();

  //boost::progress_display show_progress(World.NumPoints());
#pragma omp parallel for
  for(unsigned int i = 0; i < World.NumPoints(); i++)
  {
          DeductInformation(World.getCoord(i));
          //++show_progress;
  }
}

void Confidence::ResetInformation()
{
  for(unsigned int i = 0; i < Model.NumPoints(); i++)
  {
    Model.Points_[i].ResetInformation();
  }
}

std::vector<double> Confidence::getTotalInformation() const
{
  std::vector<double> Information(Model.Points_.size());
  for(unsigned int i = 0; i < Model.Points_.size(); i++)
  {
    Information[i] = Model.getTotalInformation(i);
  }

  return Information;
}

std::vector<double> Confidence::getRemainingInformation() const
{
  std::vector<double> Information(Model.Points_.size());
  for(unsigned int i = 0; i < Model.Points_.size(); i++)
  {
    Information[i] = Model.getRemainingInformation(i);
  }

  return Information;
}

double Confidence::SumTotalInformation() const
{
  std::vector<double> Information = getTotalInformation();

  double InfoSum = Tools::VectorSum(Information);
  return InfoSum;
}

double Confidence::SumRemainingInformation() const
{
  std::vector<double> Information = getRemainingInformation();

  return Tools::VectorSum(Information);
}
