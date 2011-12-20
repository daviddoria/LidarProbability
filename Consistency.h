#ifndef CONSISTENCY_H
#define CONSISTENCY_H

#include <ModelFile/ModelFile.h>
#include <LidarScan/LidarScanner.h>
#include <Geometry/Color.h>
#include <ComparableModels/ComparableModels.h>

class Prob
{
  // This class is for a single modelpoint-to-worldpoint comparison
public:
  double D, L, Mismatch;
  PointType Type;
  Color<unsigned char> PointColor;

  Prob(double d, double l, double m) : D(d), L(l), Mismatch(m)
  {
    if (D <= (L - Mismatch))//the world point is in front of where we think the model is, so we could have hit an occluding point with some probability
    {
      Type = UNINFORMATIVE;
      PointColor = Colors::Black();
    }
    else if(D >= (L + Mismatch))
    {
      Type = BAD; //bad
      PointColor = Colors::Red();
    }
    else
    {
      Type = GOOD; //good
      PointColor = Colors::Green();
    }

  }

  double Probability();

};

class Consistency
{
public:

  Consistency(const ComparableModels &comparable, const double mismatch);

private:
  double Mismatch;
  ComparableModels Comparable;
  bool ShowProgress;

  double Compute(const double WorldDepth, const double ModelDepth) const;
  double CalculateConsistency() const;
  double IdealScore();
};

#endif
