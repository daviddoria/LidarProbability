
void LidarScanner::AcquirePointFast(const ModelFile &Scene, const vgl_vector_3d<double> &Direction, vgl_point_3d<double> &ClosestIntersection) const
{
	double inf = numeric_limits<double>::infinity();
	double MinDist = inf;
	Ray R(ScanParams.getLocation(), Direction);

	vgl_box_3d<double> Box = Scene.BoundingBox();
	
	vgl_point_3d<double> Intersection;

	bool bIntersectBox;
	R.IntersectBox(Box, bIntersectBox);

	if(bIntersectBox)
	{
		Triangle Tri;
		double dist;
		bool bIntersectTriangle;
		for (int triangle = 0; triangle < Scene.NumTriangles(); triangle++)
		{
			Tri = Scene.getTriangle(triangle);
			Intersection = R.IntersectTriangle(Tri, dist, bIntersectTriangle);
		
			if ( bIntersectTriangle && (dist < MinDist) && (dist > 0) )
				MinDist = dist;
			
		}//end triangle loop
	
		if ( (MinDist < inf) && (MinDist > 0) )
			ClosestIntersection = R.PointAlong(MinDist);
	
	}//end intersect cube if
}


LidarScan LidarScanner::AcquireSceneFast(const ModelFile &Scene) const
{
	
	bool success;
	SDL_Surface *screen = InitSDL(640,480,"Test Window");
	
	glColor3f(1.0f, 0.0f, 0.0f);
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(50, 1, 1, 100); //field of view angle, aspect ratio, znear, zfar
    	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//gluLookAt(2, 2, 10, 0, 0, 0, 0, 1, 0);
	gluLookAt(0, 0, 10, 0, 0, -10, 0, 1, 0);
	
	cout << Scene << endl;
	//Scene.DrawPoints();
	Scene.DrawTriangles(true,true);
	SDL_GL_SwapBuffers();
	
	
	//setup Scan
	LidarScan Scan(ScanParams);
	
	Scan.setThetaAngleList(ThetaAngles_);
	
	//#pragma omp parallel for
	for(int theta = 0; theta < ScanParams.getNumThetaPoints(); theta++)
	{
		Scan.setStrip(theta, AcquireStrip(Scene, ThetaAngles_[theta]));
	}
	
	SDL_Quit();  
	
	return Scan;
}

//working OpenGL idea
LidarPoint LidarScanner::AcquirePointFast(const ModelFile &Model, const vgl_vector_3d<double> &Direction, bool &bValid) const
{
	ModelFile ColoredScene = Model.Colored();

	Ray R(ScanParams.getLocation(), Direction);
	
	vgl_point_3d<double> P = R.PointAlong(1);
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	double n = .001;
	glOrtho(-n, n, -n, n, .1, 50);
    	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	//gluLookAt(ScanParams.getLocation().x(), ScanParams.getLocation().y(), ScanParams.getLocation().z(), P.x(), P.y(), P.z(), 0, 1, 0);
	gluLookAt(ScanParams.getLocation().x(), ScanParams.getLocation().y(), ScanParams.getLocation().z(), P.x(), P.y(), P.z(), ScanParams.getUp().x(), ScanParams.getUp().y(), ScanParams.getUp().z());
	
	//display(Model.getList());
	display(ColoredScene.getList());
	
	SDL_GL_SwapBuffers();

	vgl_point_3d<double> Intersection;
	Color<unsigned char> CenterColor = PickCenter(5/2, 5/2);
	//int tri = Model.ColorLookup(CenterColor);
	int tri = ColoredScene.ColorLookup(CenterColor);
	
	unsigned char pixel[3] = {255, 0, 0};
	
	LidarPoint LidarReturn;
	LidarReturn.setDirection(Direction);//no matter what happened, save the angle information
	
	bValid = false; //unless otherwise set
	if(tri > 0)
	{
		Triangle Tri = Model.getTriangle(tri);
	
		double dist;
		bool bIntersectTriangle = false;
		Intersection = R.IntersectTriangle(Tri,dist, bIntersectTriangle);
		if(!bIntersectTriangle)
		{
			return LidarReturn;
		}
		
		//a valid intersection was found, so return that point
		bValid = true;
		LidarReturn = LidarPoint(Intersection, Direction, CalculateDepth(Intersection), CalculateHeight(Intersection), true);
		return LidarReturn;
	}
	else
	{
		return LidarReturn;//no triangle intersection
	}
}



LidarPoint LidarScanner::AcquirePointFast(const ModelFile &Model, const vgl_vector_3d<double> &Direction, bool &bValid) const
{
		//working - Point cloud distance
	double MinDist = numeric_limits<double>::infinity();

	Ray R(ScanParams.getLocation(), Direction);
	
	vgl_box_3d<double> Box = Model.BoundingBox();
	
	assert(!Box.is_empty());
	
	vgl_point_3d<double> ClosestPoint;

	bool bIntersectBox = R.IntersectBox(Box);

	double DistToScanner, DistToLine;
	
	if(bIntersectBox) //if the ray intersects the bounding box
	{
		//two points that define a line
		vgl_line_3d_2_points<double> Line(getPosition(), getPosition() + Direction);

		for(int point = 0; point < Model.NumPoints(); point++)
		{
			DistToLine = vgl_distance(Line, Model.getPoint(point));
			//cout << "Distance: " << D << endl;
			if(DistToLine < Thresh_)
			{
				DistToScanner = vgl_distance(getPosition(), Model.getPoint(point));
			}
			else
			{
				continue;
			}
			
			if (DistToScanner < MinDist)
			{
				MinDist = DistToScanner;
				ClosestPoint = Model.getPoint(point);
			}
		}
			
	}//end intersect box

	assert(MinDist >= 0);
	
	//setup the return point (no matter what happened, save the angle information)
	LidarPoint LidarReturn;
	//if ( (MinDist < numeric_limits<double>::infinity()) && (MinDist > 0) )
	if (MinDist < numeric_limits<double>::infinity()) //one of the poitns qualified
	{
		LidarReturn = LidarPoint(ClosestPoint, Direction, CalculateDepth(ClosestPoint), CalculateHeight(ClosestPoint), true);
	}
	else
	{
		LidarReturn.setDirection(Direction);
		//Point=(0,0,0), Height=0, Depth=0, and Hit=false should already be set
	}

	return LidarReturn;
}