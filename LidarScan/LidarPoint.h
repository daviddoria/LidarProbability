#ifndef LIDARPOINT_H
#define LIDARPOINT_H

#include <iostream>
#include <string>
#include <assert.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <Geometry/Color.h>

class LidarPoint
{
private:
	char WriteMode_;

	vgl_point_3d<double> Coord_;//(x,y,z) coords of point in scanner centered coordinates
	vgl_vector_3d<double> Direction_;//the direction from which the point was scanned
	double Depth_;//the projection of the point on the scanner forward axis
	double Height_;//the proejction of the point on the scanner up axis
	Color<unsigned char> Color_;
	bool Hit_;//hit or miss? is the point valid?
	bool Valid_;
	vgl_vector_3d<double> Normal_; //the normal of the triangle that was intersected
	

public:
	unsigned int HitTri_;
	
  ///////////// Constructors //////////
	LidarPoint() : WriteMode_('s'), Depth_(0.0), Height_(0.0), Hit_(false), Valid_(false) {}

	LidarPoint(const vgl_point_3d<double> &Coord, const vgl_vector_3d<double> &Direction, const double Depth, const double Height, const bool Hit) : WriteMode_('s'), Coord_ (Coord), Direction_ (Direction), Depth_(Depth), Height_(Height), Hit_(Hit){}


	////////// Accessors ///////////
	vgl_point_3d<double> getCoord() const {return Coord_;}
	vgl_vector_3d<double> getDirection() const {return Direction_;}
	double getDepth() const {if(Hit_) return Depth_; else return -1.0;}
	double getHeight() const {if(Hit_) return Height_; else return -1.0;} // !!! this could be a problem because -1 is a valid height value
	bool getHit() const {return Hit_;}
	bool getValid() const {return Valid_;}
	vgl_vector_3d<double> getNormal() const {return Normal_;}
	char getWriteMode() const {return WriteMode_;}
	Color<unsigned char> getColor() const {return Color_;}

	/////////// Mutators ///////////
	void setCoord(const vgl_point_3d<double> Coord){Coord_ = Coord;}
	void setDirection(const vgl_vector_3d<double> Direction){Direction_ = Direction;}
	void setDepth(const double Depth) {Depth_ = Depth;}
	void setHeight(const double Height) {Height_ = Height;}
	void setHit(const bool Hit) {Hit_ = Hit;}
	void setValid(const bool v) {Valid_ = v;}
	void setNormal(const vgl_vector_3d<double> &N) {Normal_ = N;}
	void setWriteMode(const char WriteMode) {WriteMode_ = WriteMode;}

};

std::ostream & operator << (std::ostream &output, const LidarPoint &P);

#endif

