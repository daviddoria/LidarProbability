#ifndef CONFIDENCE_H
#define CONFIDENCE_H

#include <ModelFile/ModelFile.h>
#include <LidarScan/LidarScanner.h>
#include <Geometry/Color.h>
#include <ComparableModels/ComparableModels.h>

class Confidence
{
	public:
		//members
		double SphereSize;
		ModelFile Model, World;
		bool ShowProgress;
		
		//constructors
		Confidence(ModelFile &model, const ModelFile &world, const double spheresize) : Model(model), World(world), SphereSize(spheresize), ShowProgress(true){}
		
		//functions
		void DeductInformation(const vgl_point_3d<double> &ModelIntersection);
		void DeductAllPointInformation(void);
		double CalculateConfidence(void);
		void ResetInformation();
		std::vector<double> getTotalInformation() const;
		std::vector<double> getRemainingInformation() const;
		
		double SumRemainingInformation() const;
		double SumTotalInformation() const;
		
};



#endif