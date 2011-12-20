#include "LidarScanner.h"

#include <map>
#include <vector>

#include <Geometry/Color.h>

#include <VXLHelpers/VXLHelpers.h>

#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_distance.h>

///////////////Constructors//////////////////

LidarScanner::LidarScanner(const double PhiMinMax, const double ThetaMinMax)
{
	ScanParams.setThetaMin(deg2rad(-ThetaMinMax));
	ScanParams.setThetaMax(deg2rad(ThetaMinMax));
	ScanParams.setPhiMin(deg2rad(-PhiMinMax));
	ScanParams.setPhiMax(deg2rad(PhiMinMax));

	Init();
}

LidarScanner::LidarScanner(const Transformation &Trans, const int NumThetaPoints, const int NumPhiPoints, const double ThetaMin, const double ThetaMax, const double PhiMin, const double PhiMax)
{
	ScanParams.setTransformation(Trans);

	ScanParams.setNumThetaPoints(NumThetaPoints);
	ScanParams.setNumPhiPoints(NumPhiPoints);
	ScanParams.setThetaMin(ThetaMin);
	ScanParams.setThetaMax(ThetaMax);
	ScanParams.setPhiMin(PhiMin);
	ScanParams.setPhiMax(PhiMax);
	
	Init();
}

void LidarScanner::Init()
{

}

//////////// Mutators ///////////


//////////// Functions //////////////

bool LidarScanner::AcquirePoint(const ModelFile &Scene, const double theta, const double phi, LidarPoint &Intersection) const
{
	//expects theta and phi in radians
	//this function simply finds the direction and then calls AcquirePoint(Obj, dir, BFC, verb)
	vgl_vector_3d<double> Direction = SetThetaPhi(theta, phi);

	bool inter = AcquirePoint(Scene, Direction, Intersection);
	
	return inter;
}


bool LidarScanner::AcquirePoint(const ModelFile &Scene, const vgl_vector_3d<double> &Direction, LidarPoint &Intersection) const
{
	if(UseOctree_)
	{
		if(Scene.Octree_.Built_ == false)
		{
			std::cout << "You must build the octree before intersecting it!" << std::endl;
			exit(-1);
		}
		
		return AcquirePointOctree(Scene, Direction, Intersection);
	}
		
	bool bIntersect = false;
	
	double MinDist = numeric_limits<double>::infinity();
	bIntersect = false; //unless set to true later
	
	Ray R(ScanParams.getLocation(), Direction);
	
	vgl_box_3d<double> Box = Scene.GetBoundingBox();
	
	assert(!Box.is_empty());
	
	vgl_point_3d<double> BoxIntersection;
	vgl_point_3d<double> TriIntersection;
	vgl_point_3d<double> ClosestIntersection;

	bool bIntersectBox = R.IntersectBox(Box);

	unsigned int HitTri = 0;
	
	if(bIntersectBox) //if the ray intersects the bounding box
	{
		// !!! Parallelize this !!!
		for (unsigned int triangle = 0; triangle < Scene.NumTriangles(); triangle++)
		{
			Triangle Tri = Scene.getTriangle(triangle);
			
			double dist;
			bool bIntersectTriangle = R.IntersectTriangle(Tri, dist, TriIntersection);
			
			if ( bIntersectTriangle && (dist < MinDist) && (dist > 0) )
			{
				MinDist = dist;
				HitTri = triangle; //save the index of the triangle that was hit by this ray
			}
	
		}//end triangle loop
			
		if ( (MinDist < numeric_limits<double>::infinity()) && (MinDist > 0) )
		{	
			ClosestIntersection = R.PointAlong(MinDist);
			bIntersect = true;
		}
			
	}//end intersect cube if

	
	//setup the return point (no matter what happened, save the angle information)
	LidarPoint LidarReturn;
	if(bIntersect)
	{
		LidarReturn = LidarPoint(ClosestIntersection, Direction, CalculateDepth(ClosestIntersection), CalculateHeight(ClosestIntersection), true);
	}
	else
	{
		LidarReturn.setDirection(Direction);
		//Point=(0,0,0), Height=0, Depth=0, and Hit=false should already be set
	}

	Intersection = LidarReturn;
	Intersection.HitTri_ = HitTri;
	
	return bIntersect;

}

void LidarScanner::Orient(const vgl_point_3d<double> &Pos, const vgl_vector_3d<double> &Dir)
{
	//set scanner orientation
	Ray R1(vgl_point_3d<double>(0.0, 0.0, 0.0), DefaultForward());
	
	Ray R2(Pos, Dir);
	vnl_matrix<double> M = geom::CameraTransform(R1, R2);
	
	vnl_double_3x3 R;
	for(unsigned r = 0; r < 3; r++)
	{
		for(unsigned c = 0; c < 3; c++)
		{
			R.put(r,c, M.get(r,c));	
		}
	}

	//extract the first three entires of the last column (the translation vector)
	vgl_vector_3d<double> Translation(M.get(0,3), M.get(1,3), M.get(2,3));
	
	Transformation Trans(Translation, R);
	ScanParams.setTransformation(Trans);
}

bool LidarScanner::AcquirePointOctree(const ModelFile &Model, const vgl_vector_3d<double> &Direction, LidarPoint &Intersection) const
{
	OrientedPoint cp;
	Ray R(ScanParams.getLocation(), Direction);
	
	bool valid = Model.IntersectRay(R, cp);

	Intersection = LidarPoint();
	
	if(valid) // if there was an intersection
	{
		Intersection = LidarPoint(cp.getCoord(), Direction, CalculateDepth(cp.getCoord()), CalculateHeight(cp.getCoord()), true);
		Intersection.setNormal(cp.getNormal()); //save the normal
		Intersection.setValid(true);
	}
	else //there was no intersection
	{
		Intersection.setDirection(Direction);
		//Point=(0,0,0), Height=0, Depth=0, Valid=false, and Hit=false should already be set
	}
	
	return valid;
}


LidarStrip LidarScanner::AcquireStrip(const ModelFile &Scene, const double theta) const
{
// Inputs: 
	//Scene: the model to scan
	//theta: the theta angle (in radians)
	
	//create an empty strip
  	LidarStrip Strip(ScanParams.getNumPhiPoints());

	LidarPoint Point;
	
	for(unsigned int phi = 0; phi < ScanParams.getNumPhiPoints(); phi ++)
	{

		vgl_vector_3d<double> Direction = SetThetaPhi(theta, ScanParams.getPhiAngle(phi));
		
		if(UseOctree_)
		{
			assert(Scene.Octree_.Built_);
			AcquirePointOctree(Scene, Direction, Point);
		}
		else
			AcquirePoint(Scene, Direction, Point);
			
		Strip.setPoint(phi, Point); 
	}
	return Strip;
}



LidarScan LidarScanner::Strips2Scan(const vector<LidarStrip> &Strips) const
{
	LidarScan Scan(ScanParams);
	
	for(unsigned int theta = 0; theta < ScanParams.getNumThetaPoints(); theta++)
	{
		Scan.setStrip(theta, Strips[theta]);
	}
	
	return Scan;
}

LidarScan LidarScanner::AcquireScene(const ModelFile &Scene) const
{
	LidarScan Scan(ScanParams);

	for(unsigned int theta = 0; theta < ScanParams.getNumThetaPoints(); theta++)
	{
		double Angle = ScanParams.getThetaAngle(theta);
		LidarStrip S = AcquireStrip(Scene, Angle);
		Scan.setStrip(theta, S);
	}
	
	return Scan;
}


double LidarScanner::CalculateDepth(const vgl_point_3d<double> &Point) const
{
	vgl_vector_3d<double> b = Point - ScanParams.getLocation();
	
	return (dot_product(ScanParams.getForward(), b) * ScanParams.getForward()).length();
}

double LidarScanner::CalculateHeight(const vgl_point_3d<double> &Point) const
{
	vgl_vector_3d<double> b = Point - ScanParams.getLocation();
	
	return (dot_product(ScanParams.getUp(), b) * ScanParams.getUp()).length();
}


//make grid on XY axis centered at (0,0) facing (0, 0, -1)
vector<vector<vgl_vector_3d<double> > > LidarScanner::MakeLinearGrid()
{
	
	double DistanceToGrid = 100;
	
	vgl_point_3d<double> Origin(0,0,0);
		
	vgl_vector_3d<double> vBottomLeft = Sphere2Rect(ScanParams.getPhiMin(), ScanParams.getThetaMin());
	
	
	vgl_point_3d<double> pBottomLeft = VXLHelpers::PointAlong(Origin, vBottomLeft, DistanceToGrid);
	double bottomleftX = pBottomLeft.x();
	double bottomleftY = pBottomLeft.y();
	
	//we want a flat grid, so the Z values are all the same
	double allZ = pBottomLeft.z();
	
	vgl_vector_3d<double> vBottomRight = Sphere2Rect(ScanParams.getPhiMin(), ScanParams.getThetaMax());
	vgl_point_3d<double> pBotomRight = VXLHelpers::PointAlong(Origin, vBottomRight, DistanceToGrid);
	double bottomrightX = pBotomRight.x();
	//double bottomrightY = pBotomRight.y();
	
	vgl_vector_3d<double> vTopLeft = Sphere2Rect(ScanParams.getPhiMax(), ScanParams.getThetaMin());
	vgl_point_3d<double> pTopLeft = VXLHelpers::PointAlong(Origin, vTopLeft, DistanceToGrid);
	//double topleftX = pTopLeft.x();
	double topleftY = pTopLeft.y();
	
	double stepX;
	if (ScanParams.getNumThetaPoints() > 1)
		stepX = fabs(bottomrightX - bottomleftX) / (ScanParams.getNumThetaPoints() - 1);
	else
		stepX = 0;
	
	double stepY;
	if (ScanParams.getNumPhiPoints() > 1)
		stepY = fabs(topleftY - bottomleftY) / (ScanParams.getNumPhiPoints() - 1);
	else
		stepY = 0;
	
	vector<vector<vgl_vector_3d<double> > > Grid(ScanParams.getNumThetaPoints());
	vector<vgl_vector_3d<double> > CurrentColumn(ScanParams.getNumPhiPoints());
	
	
	for(unsigned int xcounter = 0; xcounter < ScanParams.getNumThetaPoints(); xcounter++)
	{
		CurrentColumn.clear();
		CurrentColumn.resize(ScanParams.getNumThetaPoints());
		for(unsigned int ycounter = 0; ycounter < ScanParams.getNumPhiPoints(); ycounter++)
		{
			vgl_point_3d<double> P(bottomleftX + (xcounter * stepX), bottomleftY + (ycounter * stepY), allZ);
		
			vgl_vector_3d<double> CurrentDirection = P - Origin;
		
			CurrentColumn[ycounter] = CurrentDirection;
		}
	
		Grid[xcounter] = CurrentColumn;
	}
	
	return Grid;

}

vgl_vector_3d<double> LidarScanner::SetThetaPhi(const double Theta, const double Phi) const
{

	assert((Phi <= M_PI/2) && (Phi >= -M_PI/2));
	
	//we start with the forward vector
	vgl_vector_3d<double> V = ScanParams.getForward();
	
	//+ phi is a negative rotation around +x(right)
	V = VXLHelpers::Rotate(V, ScanParams.getRight(), Phi);
	
	//+theta is a negative rotation around +z(up)
	V = VXLHelpers::Rotate(V, ScanParams.getUp(), Theta);
	
	return V;
}


//make grid on XY axis centered at (0,0) facing (0,0,-1)
vector<vector<vgl_vector_3d<double> > > LidarScanner::MakeSphericalGrid()
{
	
	vector<vector<vgl_vector_3d<double> > > Grid(ScanParams.getNumThetaPoints());
	vector<vgl_vector_3d<double> > CurrentColumn(ScanParams.getNumPhiPoints());
	
	for(unsigned int thetaCounter = 0; thetaCounter < ScanParams.getNumThetaPoints(); thetaCounter++)
	{
		CurrentColumn.clear();
		CurrentColumn.resize(ScanParams.getNumThetaPoints());
		for(unsigned int phiCounter = 0; phiCounter < ScanParams.getNumPhiPoints(); phiCounter++)
		{
			double phi = ScanParams.getPhiMin() + phiCounter * ScanParams.getPhiStep();
			double theta = ScanParams.getThetaMin() + thetaCounter * ScanParams.getThetaStep();
			
			vgl_vector_3d<double> CurrentDirection = Sphere2Rect(phi, theta);
		
			CurrentColumn[phiCounter] = CurrentDirection;
		}
	
		Grid[thetaCounter] = CurrentColumn;
	}
	
	return Grid;

}

////////// External Operators /////////////

ostream & operator << (ostream &output, const LidarScanner &Scanner)
{
  output << "Scanner" << endl
		<< "-----------" << endl
		<< "Location: " << Scanner.ScanParams.getLocation() << endl
		  << "Forward: " << Scanner.ScanParams.getForward() << endl
		  << "Up: " << Scanner.ScanParams.getUp() << endl
		  << "Theta Points: " << Scanner.ScanParams.getNumThetaPoints() << endl
		  << "Phi Points: " << Scanner.ScanParams.getNumPhiPoints() << endl
		  << "Theta Min: " << Scanner.ScanParams.getThetaMin() << endl
		  << "Theta Max: " << Scanner.ScanParams.getThetaMax() << endl
		  << "Phi Min: " << Scanner.ScanParams.getPhiMin() << endl
		  << "Phi Max: " << Scanner.ScanParams.getPhiMax() << endl << endl;

    return output;
}
