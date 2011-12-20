#include "ScanParamsClass.h"

void ScanParamsClass::setLocation(const vgl_vector_3d<double> &Loc)
{
	Trans_ = Transformation(Loc, Trans_.getRotation());
}

void ScanParamsClass::setRotation(const vnl_double_3x3 &R)
{
	Trans_ = Transformation(Trans_.getTranslation(), R);
}


vgl_plane_3d<double> ScanParamsClass::getForwardUpPlane() const
{
	vgl_vector_3d<double> normal = cross_product(getUp(), getForward());

	return vgl_plane_3d<double> (normal, VXLHelpers::vgl_vector_to_vgl_point(Trans_.getTranslation()));
}

vgl_plane_3d<double> ScanParamsClass::getForwardRightPlane() const
{
	vgl_vector_3d<double> normal = cross_product(getRight(), getForward());

	return vgl_plane_3d<double> (normal, VXLHelpers::vgl_vector_to_vgl_point(Trans_.getTranslation()));
}
	

std::vector<double> ScanParamsClass::CalculateThetaAngles()
{
	assert(NumThetaPoints_ > 0);
	
	std::vector<double> Angles(NumThetaPoints_);

	if(NumThetaPoints_ == 1)
	{
		Angles[0] = (ThetaMin_ + ThetaMax_) / 2.0;
	}
	else
	{
		for(unsigned int theta = 0; theta < NumThetaPoints_; theta++)
		{
			Angles[theta] = ThetaMin_ + (theta * getThetaStep());
		}
	}
	return Angles;
}

std::vector<double> ScanParamsClass::CalculatePhiAngles()
{
	assert(NumPhiPoints_ > 0);
	
	std::vector<double> Angles(NumPhiPoints_);

	if(NumPhiPoints_ == 1)
	{
		Angles[0] = (PhiMin_ + PhiMax_) / 2.0;
	}
	else
	{
		for(unsigned int phi = 0; phi < NumPhiPoints_; phi++)
		{
			Angles[phi] = PhiMin_ + (phi * getPhiStep());
		}
	}
	
	return Angles;
}