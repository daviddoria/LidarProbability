#ifndef CONFIDENCE_H
#define CONFIDENCE_H

#include <ModelFile/ModelFile.h>
#include <LidarScan/LidarScanner.h>
#include <Geometry/Color.h>
#include <ComparableModels/ComparableModels.h>

class Confidence
{
public:

  double SphereSize;
  ModelFile Model, World;
  bool ShowProgress;

  Confidence(const ModelFile &model, const ModelFile &world, const double spheresize);

  void DeductInformation(const vgl_point_3d<double> &ModelIntersection);
  void DeductAllPointInformation();
  double CalculateConfidence();
  void ResetInformation();
  std::vector<double> getTotalInformation() const;
  std::vector<double> getRemainingInformation() const;

  double SumRemainingInformation() const;
  double SumTotalInformation() const;
};

#endif
