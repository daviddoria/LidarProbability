#ifndef LIDARSTRIP_H
#define LIDARSTRIP_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <vgl/vgl_point_3d.h>

#include "LidarPoint.h"

class LidarStrip
{
private:
	std::string WriteMode_;
	std::vector<LidarPoint> Points_;

public:
	/////////// Constructors /////////
	LidarStrip(){}
	LidarStrip(unsigned int NumPoints);

	/////////Accessors///////////
	LidarPoint getPoint(unsigned int ind) const;
	std::string getWriteMode() const {return WriteMode_;}
	unsigned int NumPoints() const {return Points_.size();}

	////////////Mutators/////////
	void setPoint(unsigned int ind, LidarPoint Point);
	void setWriteMode(std::string WriteMode) {WriteMode_ = WriteMode;}

	//////////////Functions//////////////
	unsigned int NumHits() const;
	bool IsEmpty() const;
	std::vector<double> getAllDepths() const;
	std::vector<vgl_point_3d<double> > GetAllPoints() const;
	
	void OutputHitPointsXYZ();
	void OutputHitPointsDH();
	void WriteToStripFile(std::string &Filename);

	std::ostream& WriteToFile(std::ostream& output);

};

std::ostream & operator << (std::ostream &output, const LidarStrip &S);

#endif

