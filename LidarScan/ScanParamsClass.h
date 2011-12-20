#ifndef SCANPARAMSCLASS_H
#define SCANPARAMSCLASS_H

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <Geometry/Transformation.h>
#include <Geometry/Angles.h>

class ScanParamsClass
{
private:
	unsigned int NumThetaPoints_, NumPhiPoints_;//how many points to take in each direction
	double PhiMin_, PhiMax_;//where to start and end each strip
	double ThetaMin_, ThetaMax_;//where to start and end taking strips
	std::vector<double> PhiAngles_;
	std::vector<double> ThetaAngles_;

	Transformation Trans_;

public:
	/////////// Constructors //////////////
	ScanParamsClass() : NumThetaPoints_(128), NumPhiPoints_(128), PhiMin_(deg2rad(-10)), PhiMax_(deg2rad(10)), ThetaMin_(deg2rad(-10)), ThetaMax_(deg2rad(10)) 
	{
		vgl_vector_3d<double> T(0,0,0);
		Transformation Trans(T);
	
		setTransformation(Trans);
		
		CalculateAngles();
	}
	
	////////// Mutators /////////////////
	void setThetaMin(const double ThetaMin) {ThetaMin_ = ThetaMin; CalculateAngles();}
	void setThetaMax(const double ThetaMax) {ThetaMax_ = ThetaMax; CalculateAngles();}
	void setNumThetaPoints(const unsigned int n) {NumThetaPoints_ = n; CalculateAngles();}
	void setPhiMin(const double PhiMin) {PhiMin_ = PhiMin; CalculateAngles();}
	void setPhiMax(const double PhiMax) {PhiMax_ = PhiMax; CalculateAngles();}
	void setNumPhiPoints(const unsigned int n) {NumPhiPoints_ = n; CalculateAngles();}
	/*
	void setLocation(const vgl_point_3d<double> &Location) {Location_ = Location;}
	void setForward(const vgl_vector_3d<double> &Forward) {Forward_ = normalized(Forward);}
	void setUp(const vgl_vector_3d<double> &Up) {Up_ = normalized(Up);}
	*/
	void setTransformation(const Transformation &Trans) {Trans_ = Trans;}
	void setLocation(const vgl_vector_3d<double> &Loc);
	void setLocation(const vgl_point_3d<double> &Loc) {setLocation(VXLHelpers::vgl_point_to_vgl_vector(Loc));}
	void setRotation(const vnl_double_3x3 &R);

	void setThetaSpan(const double theta)
	{
		ThetaMin_ = -theta/2;
		ThetaMax_ = theta/2;
	}

	void setPhiSpan(const double phi)
	{
		PhiMin_ = -phi/2;
		PhiMax_ = phi/2;
	}
	
	///////////// Accessors ///////////////////
	unsigned int getNumThetaPoints() const {return NumThetaPoints_;}
	unsigned int getNumPhiPoints() const {return NumPhiPoints_;}
	double getThetaMin() const {return ThetaMin_;}
	double getThetaMax() const {return ThetaMax_;}
	double getPhiMin() const {return PhiMin_;}
	double getPhiMax() const {return PhiMax_;}
	double getThetaAngle(int ind) const {return ThetaAngles_[ind];}
	double getPhiAngle(int ind) const {return PhiAngles_[ind];}
	std::vector<double> getThetaAngleList() const {return ThetaAngles_;}
	std::vector<double> getPhiAngleList() const {return PhiAngles_;}
	
	vgl_point_3d<double> getLocation() const
	{
		return VXLHelpers::vgl_vector_to_vgl_point(Trans_.getTranslation());
	}
	vgl_vector_3d<double> getUp() const {return VXLHelpers::vnl_vector_to_vgl_vector(Trans_.getRotation().get_column(2));}
	vgl_vector_3d<double> getRight() const {return VXLHelpers::vnl_vector_to_vgl_vector(Trans_.getRotation().get_column(0));}
	vgl_vector_3d<double> getForward() const {return VXLHelpers::vnl_vector_to_vgl_vector(Trans_.getRotation().get_column(1));}
	vgl_plane_3d<double> getForwardUpPlane() const;
	vgl_plane_3d<double> getForwardRightPlane() const;
	
	std::vector<double> CalculateThetaAngles();
	std::vector<double> CalculatePhiAngles();
	
	void CalculateAngles()
	{
		ThetaAngles_ = CalculateThetaAngles();
		PhiAngles_ = CalculatePhiAngles();
	}

	///////// Calculated Accessors /////////////////
	double getPhiStep() const {return fabs(PhiMax_ - PhiMin_)/static_cast<double> (NumPhiPoints_ - 1);}
	double getThetaStep() const {return fabs(ThetaMax_-ThetaMin_)/static_cast<double>(NumThetaPoints_ - 1);}
	double TotalPoints() const{	return NumPhiPoints_ * NumThetaPoints_;	}
};

#endif