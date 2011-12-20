#include "LidarStrip.h"

#include <Tools.h>

//////////// Constructors /////////////

LidarStrip::LidarStrip(unsigned int NumPoints)
{
  Points_.clear();
  Points_.resize(NumPoints);

  LidarPoint EmptyPoint;

  for(unsigned int i = 0; i < NumPoints; i++)
  {
    Points_[i] = EmptyPoint;
  }
}

///////////Accessors/////////////
LidarPoint LidarStrip::getPoint(unsigned int ind) const
{
  if( (ind < NumPoints()) && (ind >= 0) )
  {
    return Points_[ind];
  }
  else
  {
    cout << "Requested index: " << ind << endl;
    cout << "NumPoints: " << NumPoints() << endl;
    assert(0);
    LidarPoint Empty;
    return Empty;
  }
}

//////////Mutators////////////
void LidarStrip::setPoint(unsigned int ind, LidarPoint Point)
{
  if ((ind < NumPoints()) && (ind >= 0))
  {
    Points_[ind] = Point;
  }
  else
  {
    cout << "LidarStrip::setPoint index out of range" << endl;
    cout << "Requested ind: " << ind << endl;
    cout << "NumPoints: " << NumPoints() << endl;
  }

}

///////////Functions////////////////


void LidarStrip::WriteToStripFile(std::string &Filename)
{
  std::ofstream output(Filename.c_str(), std::ios::app);

  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    output << Points_[i] << std::endl;
  }
}

unsigned int LidarStrip::NumHits() const
{
  unsigned int hits = 0;

  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    if (Points_[i].getHit() )
    {
      hits++;
    }
  }
  return hits;
}

bool LidarStrip::IsEmpty() const
{
  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    if (Points_[i].getHit() == true )
    {
      return false;
    }
  }

  return true;
}

std::vector<double> LidarStrip::getAllDepths() const
{
  std::vector<double> Depths(NumPoints());

  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    Depths[i] = Points_[i].getDepth();
  }
  return Depths;
}

std::vector<vgl_point_3d<double> > LidarStrip::GetAllPoints() const
{
  std::vector<vgl_point_3d<double> > ValidPoints;

  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    if(Points_[i].getHit())
    {
      ValidPoints.push_back(Points_[i].getCoord());
    }
  }
  return ValidPoints;
}

//////// Output Functions //////////
void LidarStrip::OutputHitPointsXYZ()
{
  std::cout << "Hit Points (X,Y,Z)" << std::endl
                  << "--------------" << std::endl;
  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    if(Points_[i].getHit() == true)
    {
      std::cout << Points_[i].getCoord() << std::endl;
    }
  }
}
  
void LidarStrip::OutputHitPointsDH()
{
  std::cout << "Hit Points (Depth, Height)" << std::endl
                  << "--------------" << std::endl;
  for(unsigned int i = 0; i < NumPoints(); i++)
  {
    if(Points_[i].getHit() == true)
    {
      std::cout << Points_[i].getDepth() << " " << Points_[i].getHeight() << std::endl;
    }
  }
}


///////// External Operators ///////////
std::ostream & operator << (std::ostream &output, const LidarStrip &S)
{
  if(S.getWriteMode() == "screen")
  {
    for(unsigned int i = 0; i < S.NumPoints(); i++)
    {
      output << "Point: " << i << std::endl
                      << "-------" << std::endl
                      << S.getPoint(i) << std::endl;
    }
  }
  else if(S.getWriteMode() == "file")
  {
    for(unsigned int i = 0; i < S.NumPoints(); i++)
    {
            output << S.getPoint(i) << std::endl;
    }
  }

  return output;
}
