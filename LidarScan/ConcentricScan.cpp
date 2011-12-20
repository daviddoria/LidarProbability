#include <iostream>
#include <string>
#include <sstream>
#include <ObjFile.h>
#include "ScanScene.h"
#include "LidarScan.h"
#include "LidarScanner.h"

#include <omp.h> //multiprocessor support

using namespace std;

bool CircularScan(ObjFile &Scene, LidarScan SP, double radius, int NumAlpha);

//call with
// ./ConcentricScan rect.obj 300 300 -20 20 -10 10 2 20 360
int main(int argc, char* argv[])
{
  //get params from command line
  string ModelFile = argv[1];

  //setup scan                                                                  
  int ThetaPoints = atoi(argv[2]);
  int PhiPoints = atoi(argv[3]);
  double ThetaStart = atoi(argv[4]);
  double ThetaEnd = atoi(argv[5]);
  double PhiStart = atoi(argv[6]);
  double PhiEnd = atoi(argv[7]);
  
  ObjFile Scene(ModelFile);
  LidarScan Scan(ThetaPoints, PhiPoints, ThetaStart, ThetaEnd, PhiStart, PhiEnd);

  bool success =  CircularScan(Scene, Scan, 15, 360);

  return 0;
}


bool CircularScan(ObjFile &Scene, LidarScan SP, double radius, int NumAlpha)
{
  int verbosity = 1;

  //calculate camera positions around circle
  vector<geom_Point3> Locations;
  vector<double> Angles;
  vector<geom_Vector3> Forwards;
	
  double PoseStep = 360 / NumAlpha;

  double height = 2;
  geom_Point3 CenterOfCircle(0, height, 0);
  geom_Vector3 Up(0,1,0); //looking straight forward until further notice

  //start scanner at (0,height,radius) and facing (0,0,-1)
  for(int CurrentPose = 0; CurrentPose < NumAlpha; CurrentPose++)
    {
      double CurrentAngle = CurrentPose*PoseStep;
      Angles.push_back(CurrentAngle);
      Locations.push_back(geom_Point3(radius * sin(deg2rad(CurrentAngle)), height, radius * cos(deg2rad(CurrentAngle))));
      Forwards.push_back(VectorBetweenPoints(Locations.at(CurrentPose), CenterOfCircle));
    }

  int StartPosition = 0; //start at the bottom of the circle

  #pragma omp parallel for
  for(int PoseCounter = StartPosition; PoseCounter < NumAlpha; PoseCounter++)
    {

      //should have done this with a copy constructor
      LidarScan Scan(Scan.getThetaPoints(), Scan.getPhiPoints(), deg2rad(Scan.getThetaStart()), 
		     deg2rad(Scan.getThetaEnd()), deg2rad(Scan.getPhiStart()), deg2rad(Scan.getPhiEnd()));

      LidarScanner Scanner(Forwards.at(PoseCounter), Up, Locations.at(PoseCounter));

      cout << "Scanning from Position " << PoseCounter << endl;

      ScanScene(Scan, Scanner, Scene);
      
      string FileName;
      string ObjectName = "truck";
      string ext = ".lid";
      stringstream ss;
      ss << PoseCounter;
      FileName = ObjectName + ss.str() + ext;
      cout << "Writing " << FileName << "..." << endl;

      WriteLidFile(FileName, Scan, Scanner);
    }

      return true; //there were no problems
}
