#ifndef LIDARSCAN_H
#define LIDARSCAN_H

#include <fstream>
#include <iostream>
#include <limits>
#include <cstdio>
#include <string>
#include <sstream>
#include <cassert>

#include <ModelFile/ModelFile.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include "LidarPoint.h"
#include "LidarStrip.h"
#include "ScanParamsClass.h"

class LidarScan
{

private:
	
	//vector<double> ThetaAngleList_;

	//The ScanGrid is a vector of columns(strips) (hence a grid) where each entry is a ScanPoint.
	std::vector<LidarStrip> Strips_;

public:

	//these are set directly from the scan parameters of the scanner which took the scan
	ScanParamsClass ScanParams;

	///////////////////// Constructors //////////////////////////////////
	LidarScan(); //only to be used when creating the object with the intent of copying another LidarScan into it.
	LidarScan(const ScanParamsClass &S)
	{
		ScanParams = S;
		
		LidarStrip EmptyStrip(ScanParams.getNumPhiPoints());

		Strips_.resize(ScanParams.getNumThetaPoints());
		/*
		for(int i = 0; i < ScanParams.getNumThetaPoints(); i++)
		{
			Strips_[i] = EmptyStrip;
		}
		*/
	}
	
	LidarScan(const std::string Filename, bool &success)//to read from a .lid file
	{
		ReadLidFile(Filename, success);
	}

	LidarScan(unsigned int ThetaPoints, unsigned int PhiPoints);

	/////////////////////// Accessors /////////////////
	unsigned int NumNonEmptyStrips() const;
	LidarStrip getStrip(const unsigned int strip) const;
	LidarPoint getPoint(const unsigned int strip, const unsigned int point) const
	{
		LidarStrip Strip = getStrip(strip);
		return Strip.getPoint(point);
	}

	unsigned int NumStrips() const {return Strips_.size();}
	double AverageHitsPerStrip() const;
	unsigned int NumHits() const;
	unsigned int FirstNonEmptyStrip() const;

	
	/////////// Mutators //////////
	void setStrip(const unsigned int ind, const LidarStrip &Strip);
	
	//////////// Functions ////////////////
	double CalculateDepth(const vgl_point_3d<double> &Point, const vgl_point_3d<double> &ScannerLocation, const vgl_vector_3d<double> &Forward) const;
	double CalculateHeight(const vgl_point_3d<double> &Point, const vgl_point_3d<double> &ScannerLocation, const vgl_vector_3d<double> &Up) const ;
	
	double AverageDepth(void);

	std::vector<vgl_point_3d<double> > GetAllPoints() const;

	bool WritePTX(const std::string &Filename) const;
	
	/////////// Lid File Functions //////////
	void WriteLidFile(const string &sFileName);
	void ReadLidFile(const string &Filename, bool &success);

	void CreateModelFile(ModelFile &Model);
	void WriteImage(const std::string &Filename);
	void WriteBinaryHitImage(const std::string &Filename);

};

//////////// External Operators /////////////
std::ostream& operator<< (std::ostream& output, const LidarScan &Scan);

#endif

