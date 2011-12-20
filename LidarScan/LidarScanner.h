#ifndef LIDARSCANNER_H
#define LIDARSCANNER_H

/* 
Coordinate System:

The scanner coordinate system is as follows:
z = up
y = forward
therefore, x = right

Theta:
The angle in the "XY" (forward-right) plane, measured from +y (Forward). It's range is -pi to pi. -pi/2 is left, pi/2 is right. This is obtained by rotating around the "up" axis.

Phi:
The elevation angle, in the YZ (forward-up) plane, measured from +y. It's range is -pi/2 (down) to pi/2 (up). This is obtained by rotating around the "right" axis (AFTER the new right axis is obtained by setting Theta).
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <map>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <Geometry/Triangle.h>
#include <Geometry/Circle.h>
#include <Geometry/Ray.h>
#include <Geometry/Color.h>

#include <ModelFile/ModelFile.h>

#include <VTKHelpers/Octree.h>

#include "LidarPoint.h"
#include "LidarStrip.h"
#include "LidarScan.h"
#include "ScanParamsClass.h"

class LidarScanner
{
	private:

		bool UseOctree_;

		std::vector<vector<vgl_vector_3d<double> > > MakeLinearGrid();
		std::vector<vector<vgl_vector_3d<double> > > MakeSphericalGrid();

	public:
		
		ScanParamsClass ScanParams;
		
		/////////// Constructors /////////////;
		LidarScanner(const bool octree)
		{
			UseOctree_ = octree;
			Init();
		}
		
		LidarScanner() {}
				
		LidarScanner(const double PhiMinMax, const double ThetaMinMax);
		LidarScanner(const Transformation &T, const int ThetaPoints, const int PhiPoints, const double ThetaMin, const double ThetaMax, const double PhiMin, const double PhiMax);
		void Init();

		void Orient(const vgl_point_3d<double> &Pos, const vgl_vector_3d<double> &Dir);

  
		///////////// Accessors ////////////
		vgl_point_3d<double> getPosition(void) const {return ScanParams.getLocation();}
		static vgl_vector_3d<double> DefaultForward(void) {return vgl_vector_3d<double>(0.0, 1.0, 0.0);}
		//////////////Functions///////////
		vgl_vector_3d<double> SetThetaPhi(const double Theta, const double Phi) const;

		bool AcquirePoint(const ModelFile &Scene, const double theta, const double phi, LidarPoint &Intersection) const;
		bool AcquirePoint(const ModelFile &Scene, const vgl_vector_3d<double> &Direction, LidarPoint &Intersection) const;
		bool AcquirePointOctree(const ModelFile &Model, const vgl_vector_3d<double> &Direction, LidarPoint &Intersection) const;
		LidarStrip AcquireStrip(const ModelFile &Scene, const double theta) const;
		LidarScan AcquireScene(const ModelFile &Scene) const;
		
		double CalculateDepth(const vgl_point_3d<double> &Point) const;
		double CalculateHeight(const vgl_point_3d<double> &Point) const;
		
		LidarScan Strips2Scan(const std::vector<LidarStrip> &Strips) const;
		
};

std::ostream & operator << (std::ostream &output, const LidarScanner &Scanner);

#endif

