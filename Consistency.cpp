#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

#include "Consistency.h"
#include <ComparableModels/ComparableModels.h>

#include <omp.h>

#include <GL/glut.h>

#include <vgl/vgl_distance.h>

#include <SDL/SDL.h>
#include <SDLHelpers/SDLHelpers.h>

#include <VXLHelpers/VXLHelpers.h>

#include <Tools/Tools.h>

#include <LidarScan/LidarScanner.h>
#include <ModelFile/ModelFile.h>

#include <Integration/integration.h>

#include <Geometry/Color.h>
#include <Geometry/Transformation.h>
#include <Geometry/LineSegment.h>

using std::cout; using std::clog;

double Consistency::CalculateConsistency(void)
{
	/*
	inputs: 
	a pair of comparable models
	clutter and mismatch parameters
	*/

	double ModelDepth, WorldDepth;
		
	std::vector<double> Consistencies;
	
	if(Comparable.TotalPoints() == 0)
	{
		std::cout << "There are no comparable points!" << std::endl;
		//exit(-1);
		return 1.0;
	}
	else
	{
		for(unsigned int i = 0; i < Comparable.UsedWorldPoints.size(); i++)
		{
			vgl_point_3d<double> WorldCoord = Comparable.GetMatchingWorldCoord(i);
			vgl_point_3d<double> ModelCoord = Comparable.GetMatchingModelPoint(i);
			ModelDepth = vgl_distance(Comparable.Scanner.ScanParams.getLocation(), ModelCoord);
			WorldDepth = vgl_distance(Comparable.Scanner.ScanParams.getLocation(), WorldCoord);
			double consistency = Compute(WorldDepth, ModelDepth);
			Consistencies.push_back(consistency);
			//Comparable.World.Points_[Comparable.UsedWorldPoints[i]].ConsistencyDistance_ = WorldDepth - ModelDepth; //if this is negative, uninformative, else, free space violation

		}
		/*
		//count the miss points as consistent
		for(unsigned int i = 0; i < Comparable.MissPoints; i++)
		{
			Consistencies.push_back(1.0); //consistent
		}
		*/
	}

	double ProbSum = Tools::VectorSum(Consistencies);
	
	//double Ideal = IdealScore();
	double Ideal = Comparable.UsedWorldPoints.size();
	double FinalScore = ProbSum/Ideal;
	
	//std::clog << "There were " << ProbSum << " consistent points (including " << Comparable.MissPoints << " misses) out of " << Ideal << std::endl;
	std::clog << "There were " << ProbSum << " consistent points out of " << Ideal << std::endl;
	return FinalScore;
}


double Consistency::IdealScore(void)
{
	double ideal = static_cast<double>(Comparable.TotalPoints());
	return ideal;
}

double Consistency::Compute(const double WorldDepth, const double ModelDepth)
{
	if((WorldDepth - ModelDepth) > Mismatch)
		return 0.0;
	else
		return 1.0;
}
