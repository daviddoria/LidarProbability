#include "LidarPoint.h"

/////////// External Operators ////////////
std::ostream& operator << (std::ostream& output, const LidarPoint &LP)
{
	char Mode = LP.getWriteMode();
	assert( (Mode == 's') || (Mode == 'f') );
	if(Mode == 's')
	{
		output << "Point: " << LP.getCoord() << std::endl
				<< "Direction: " << LP.getDirection() << std::endl
				<< "Depth: " << LP.getDepth() << std::endl
				<< "Height: " << LP.getHeight() << std::endl
				<< "Hit: " << LP.getHit() << std::endl;
	}
	else if(Mode == 'f')
	{
		if(LP.getHit())
		{
			output << "1 "; //indicate "hit"
      
      			//format is: X Y Z theta_x theta_y theta_z
			vgl_point_3d<double> P = LP.getCoord();
			output << P;
      
		}
		else //point is a "miss"
		{
     			 //write a "miss" to the file
			output << "0 "; //to indicate miss
			output << "0 0 0 "; //filler
		}

 		 //either way, write the angle
		vgl_vector_3d<double> V = LP.getDirection();
		output << V;
	}

	return output;
}

