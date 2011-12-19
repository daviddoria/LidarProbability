#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include <GL/GLee.h>
#include <GL/glut.h>

#include <SDL/SDL.h>
#include "SDL_Helper.h"

#include <LidarScanner.h>

#include <ModelFile.h>


#ifndef EPSILON
const double epsilon = 1e-6;
#endif

void GenerateColors(map<geom_Color<unsigned char>, int> &Map, vector<geom_Color<unsigned char> > &List, int num);

double ProbModel(ModelFile &WorldVtk, ModelFile &ObjectVtk);

double CreateCompareableModels(const ModelFile &WorldInput, const ModelFile &ModelInput, ModelFile &World, ModelFile &Model);
			
double Likelihood(double WorldDepth, double ModelDepth, char &TypeOfPoint);

double Theta(const geom_Point3 &P, const char CoordSystem);
double Phi(const geom_Point3 &P, const char CoordSystem);

double Length(const geom_Point3 &P);
void BoundingFrustrum(const ModelFile &Object, double &MinTheta, double &MaxTheta, double &MinPhi, double &MaxPhi, char Angles);
void SetZero(double &d);
void ReadEricFile(string filename, vector<geom_Point3> &points);

int BadPoints = 0, GoodPoints = 0, Uninformative = 0;

void CartesianToSpherical(const geom_Point3 &P, double &theta, double &phi);

void ReadAllTransformations(const string Filename, vector<geom_Matrix3> &Rotations, vector<geom_Vector3> &Translations);
void ReadTransformation(const string Filename, const int n, geom_Matrix3 &R, geom_Vector3 &T);
int NumTransformations(const string Filename);
		
//vector<geom_Point3> PointsInsideFrustrum(const VtkPolydataFile &WorldVtk, const ObjFile &ModelObj, string Angles);
vector<geom_Point3> PointsInsideFrustrum(const ModelFile &WorldVtk, const ModelFile &ModelObj, char Angles);

void OutputVector(vector<double> &V);

int V = 2; //verbosity

double x,y,z;
int PositionIndex = 1;

//string Path = "/home/doriad/Projects/LidarProbability/Data/" + Folder;
string Path = "/home/doriad/Desktop/JudaProject/";

void ReportError()
{
	GLenum test = glGetError();
	if(test == GL_NO_ERROR)
		cout << "no error!" << endl;
	else
		cout << "error: " << test << endl;
}

int main(int argc, char *argv[])
{
	//World_x_y_z_.vtk are the points from the world that contributed to the comparison
	//Model_x_y_z_.vtk are the points from the model that contributed to the comparison
	//Window_x_y_z_.vtk is the entire model at the position (x,y,z) relative to the correct position
	
	string OutputFile = Path + "prob.txt";
	ofstream fout(OutputFile.c_str(), ios::out);
	
	//string PlotFile = Path + "PlotPositions.vtp";
	
	//string WorldFilename = Path + "Data/vcc_south1.vtp";
	//string ModelFilename = Path + "Data/window.vtp";
	string WorldFilename = Path + "generated.ptx";
	string ModelFilename = Path + "window_c.vtp";
	
	ModelFile WorldVtp;
	WorldVtp.Read(WorldFilename);
	ModelFile ModelVtp;
	ModelVtp.Read(ModelFilename);
	
	//this model's position will be updated each iteration
	ModelFile CurrentWindow;
		
	vector<geom_Point3> PositionList;
	vector<geom_Matrix3> Rotations;
	vector<geom_Vector3> Translations;
	
	//char MotionType = 'a';//axis aligned
	char MotionType = 'g';//general
	
	if(MotionType == 'a')//axis aligned movement, no rotation
	{
		for(x = -5; x < 5 + epsilon; x += .1)
		{
			for(y = -5; y < 5 + epsilon; y += .1)
			{
				for(double z = -.5; z < .5 + epsilon; z += .25)
				{
					SetZero(x);SetZero(y);SetZero(z);
					PositionList.push_back(geom_Point3(x,y,z));
				}
			}
		}
	}
	else if(MotionType == 'g')//general transformations
	{
		//string TransformFile = Path + "Data/transforms.xform";
		string TransformFile = Path + "transforms.xform";
		cout << "Reading transformations from: " << TransformFile << endl;
		ReadAllTransformations(TransformFile, Rotations, Translations);
	}
		
	int NumPositions;
	if(MotionType == 'g')
		NumPositions = Rotations.size();
	else if(MotionType == 'a')
		NumPositions = PositionList.size();
	
	vector<geom_Point3> PlotPositions;
	vector<double> Probability;
	vector<geom_Point3> CentersOfMass;
	
	cout << "There are " << NumPositions << " positions." << endl;
	
	for(PositionIndex = 0; PositionIndex < NumPositions; PositionIndex++)
	//for(PositionIndex = 0; PositionIndex < 4; PositionIndex++)
	{
		//attempt to read world vtk
		stringstream WorldStream;
		WorldStream << Path << "World_" << PositionIndex << ".vtp";
		ModelFile World;
		bool ReadWorldSuccess = World.Read(WorldStream.str());
		
		//attempt to read model	vtk
		stringstream ModelStream;
		ModelStream << Path << "Model_" << PositionIndex << ".vtp";
		ModelFile Model;
		bool ReadModelSuccess = Model.Read(ModelStream.str());
		
		if(!(ReadModelSuccess && ReadWorldSuccess)) //the files do not exist
		{
			CurrentWindow = ModelVtp;
			
			assert( (MotionType == 'g') || (MotionType == 'a') );
			if(MotionType == 'g')
			{
				CurrentWindow.Rotate(Rotations[PositionIndex]);
				CurrentWindow.Translate(Translations[PositionIndex]);
			}
			else if(MotionType == 'a')
			{
				CurrentWindow.setPosition(PositionList[PositionIndex]);
			}
			
			stringstream CurrentStream;
			CurrentStream << Path << "Window_" << PositionIndex << ".vtp";
			CurrentWindow.Write(CurrentStream.str());
			
			CreateCompareableModels(WorldVtp, CurrentWindow, World, Model);
		}
		
		//for "live" (over windows) graph of P
		geom_Point3 CenterOfMass = Model.CenterOfMass();
		CentersOfMass.push_back(CenterOfMass);
	
		//at this point, the files already existed or have been created
		cout << "Calculating P..." << endl;
		
		double P = ProbModel(World, Model);
		Probability.push_back(P);	
		
		cout << " P = " << P << " GoodPoints: " << GoodPoints << endl << "BadPoints: " << BadPoints << endl << "Uninformative: " << Uninformative << endl;
		//cout << "TotalPoints: " << GoodPoints + BadPoints + Uninformative << endl;
		fout << x << " " << y << " " << z << " " << P << " " << GoodPoints << " " << BadPoints << " " << Uninformative << endl;

	}//end PositionIndex loop
	
	//normalize P
	vector<double> NormalizedP;
	
	//find max
	double MaxProb = -1e6;
	for(int i = 0; i < Probability.size(); i++)
	{
		if(Probability[i] > MaxProb)
			MaxProb = Probability[i];
	}
	
	for(int i = 0; i < Probability.size(); i++)
	{
		NormalizedP.push_back(Probability[i]/MaxProb);
	}
	
	//for "live" (over windows) graph of P
	for(int i = 0; i < CentersOfMass.size(); i++)
	{
		geom_Point3 PlotPoint = CentersOfMass[i];
		PlotPoint.setZ(PlotPoint.getZ() + 2.0*NormalizedP[i]);
		PlotPositions.push_back(PlotPoint);
	}
	
	/*
	VtkPolydataFile Plot(PlotPositions);
	bool writesuccess = Plot.Write(PlotFile);
	*/
	
	fout.close();
	//PlotPositionOut.close();
	
	return 0;
}

double ProbModel(ModelFile &World, ModelFile &Model)
{
	if(V >= 2)
		cout<< "Enter ProbModel()" << endl;
	
	double DepthDifference, ModelDepth, WorldDepth, l;
	double logP = 0;
	
	vector<double> L;
	vector<double> Depths;
	
	BadPoints = 0; GoodPoints = 0; Uninformative = 0;
	
	stringstream HistStream;
	HistStream << Path << "Hist/" << PositionIndex << ".txt";
	ofstream hist(HistStream.str().c_str(), ios::out);
		
	vector<geom_Color<unsigned char> > ModelColors;
	vector<geom_Color<unsigned char> > WorldColors;
	
	geom_Color<unsigned char> Red(255,0,0);
	geom_Color<unsigned char> Green(0,255,0);
	geom_Color<unsigned char> Black(0,0,0);
	
	geom_Color<unsigned char> Yellow(255,255,0);
	geom_Color<unsigned char> Cyan(0,255,255);
	geom_Color<unsigned char> Purple(255,0,255);
	
	geom_Color<unsigned char> WorldGoodColor = Green;
	geom_Color<unsigned char> WorldBadColor = Red;
	geom_Color<unsigned char> WorldUninformativeColor = Black;
	
	geom_Color<unsigned char> ModelGoodColor = Green;
	geom_Color<unsigned char> ModelBadColor = Red;
	geom_Color<unsigned char> ModelUninformativeColor = Black;
	
	vector<double> BadDepths;
	
	for(int i = 0; i < World.NumPoints(); i++)
	{
		ModelDepth = Length(Model.getPoint(i));
		WorldDepth = Length(World.getPoint(i));
		
		DepthDifference = ModelDepth - WorldDepth;
		Depths.push_back(DepthDifference);
		hist << DepthDifference << endl;
				
		char TypeOfPoint;
		l = Likelihood(WorldDepth, ModelDepth, TypeOfPoint);
		L.push_back(l);
		
		if(TypeOfPoint == 'g')
		{
			WorldColors.push_back(WorldGoodColor);
			ModelColors.push_back(ModelGoodColor);
			
		}
		else if(TypeOfPoint == 'u')
		{
			WorldColors.push_back(WorldUninformativeColor);
			ModelColors.push_back(ModelUninformativeColor);
		}
		else if(TypeOfPoint == 'b')
		{
			WorldColors.push_back(WorldBadColor);
			ModelColors.push_back(ModelBadColor);
			BadDepths.push_back(DepthDifference);
		}
	}
	
	//OutputVector(BadDepths);
	
	hist.close();

	for(int i = 0; i < L.size(); i++)
	{
		logP += log(L[i]);
	}
	
	World.setColors(WorldColors);
	stringstream WorldStream;
	WorldStream << Path << "World_" << PositionIndex << ".vtp";
	bool s = World.Write(WorldStream.str());
	
	Model.setColors(ModelColors);
	stringstream ModelStream;
	ModelStream << Path << "Model_" << PositionIndex << ".vtp";
	bool s2 = Model.Write(ModelStream.str());
	
	return logP;
}

double CreateCompareableModels(const ModelFile &WorldInput, const ModelFile &ModelInput, ModelFile &World, ModelFile &Model)
{
	/*
	Inputs:
	WorldVtp, ModelVtp: the original files at their current positions
	
	Outputs:
	World, Model: Each point in these two files is a corresponding point
	*/
	
	map<geom_Color<unsigned char>, int> ColorMap;
	vector<geom_Color<unsigned char> > ColorList;
	GenerateColors(ColorMap, ColorList, ModelInput.NumTriangles());
	ModelFile ColoredModel(ModelInput.getPoints(), ModelInput.getVertexList(), ColorList);
	
	int PointsUsed = 0;
	BadPoints = 0; GoodPoints = 0; Uninformative = 0;
	
	LidarScanner Scanner;
	Scanner.setLocation(geom_Point3(-90.2085, -44.848, -0.793317));

	SDL_Surface *screen = InitSDL(5,5,"Test Window");
	int list;
	list = glGenLists(1);
	glNewList(list,GL_COMPILE);
		ColoredModel.DrawTriangles(false, true);
	glEndList();
	
	char method = 's';
	
	vector<geom_Point3> WorldPointsInsideFrustrum;
	if( (method == 'd') || (method == 's') )
	{
		WorldPointsInsideFrustrum = PointsInsideFrustrum(WorldInput, ModelInput, method);
	}
	
	int NumPointsInVisualHull = WorldPointsInsideFrustrum.size();
	cout << "World points inside frustrum: " << NumPointsInVisualHull << endl;
	
	vector<geom_Point3> ModelPoints(NumPointsInVisualHull);
	vector<geom_Point3> WorldPoints(NumPointsInVisualHull);
	
	int InvalidWorld = 0;
	int InvalidModel = 0;
	
	geom_Point3 WorldPoint;
	geom_Vector3 dir;
	geom_Point3 ModelPoint;
	
	int counter = 0;
	
	for(int i = 0; i < NumPointsInVisualHull; i++)
	{
		//output percent complete
		if(V >= 2)
		{
			if(i%1000 == 0)
				cout << i << endl;
		}
		
		WorldPoint = WorldPointsInsideFrustrum[i];
		
		if(!WorldPoint.IsValid())
		{
			InvalidWorld++;
			continue;
		}
		
		//get the point on the model along the current ray (assuming scanner is at (0,0,0) )
		//dir = WorldPoint.ToVector3();
		dir = WorldPoint - Scanner.getLocation();
		
		ModelPoint = Scanner.AcquirePointOpenGL(list, ColoredModel, dir, ColorMap);
		
		if(!ModelPoint.IsValid())
		{
			InvalidModel++;
			continue;
		}
		
		ModelPoints[counter] = ModelPoint;
		WorldPoints[counter] = WorldPoint;
		
		counter++;
	}
	
	SDL_Quit();
	
	ModelPoints.resize(counter);
	WorldPoints.resize(counter);
	
	cout << "Num invalid world points: " << InvalidWorld << endl;
	cout << "Num invalid model points: " << InvalidModel << endl;
	
	cout << "Num world points: " << WorldPoints.size() << endl;
	cout << "Num model points: " << ModelPoints.size() << endl;
		
	World = ModelFile(WorldPoints);
	Model = ModelFile(ModelPoints);
	
	return 0;
}

double Likelihood(double WorldDepth, double ModelDepth, char &TypeOfPoint)
{
	double L;
	
	double Tol = 5e-2;//5cm
	//double Tol = 2e-3;//2mm
	//double Tol = 10e-2;//10cm
	
	double difference = fabs(WorldDepth - ModelDepth);
	
	if(fabs(WorldDepth - ModelDepth) <= Tol) // if there is less than 2mm error, the data is consistent with the model
	{
		TypeOfPoint = 'g';
		L = 5;
		GoodPoints++;
	}
	else if(WorldDepth < ModelDepth)  //model is behind world - uninformative point, could have been occlusion
	{
		TypeOfPoint = 'u';
		L = 1;
		Uninformative++;
	}
	else if(ModelDepth < WorldDepth) //ray went through model! very bad point
	{
		TypeOfPoint = 'b';
		L = .1;
		BadPoints++;
	}
	
	return L;
}

double Theta(const geom_Point3 &P, const char CoordSystem)
{
	double epsilon = 1e-6;
	if(CoordSystem == 'd')//dave
	{
		if(fabs(P.getZ()) < epsilon)
			return 0;
		else
		{
			//return atan(P.getX()/fabs(P.getZ()));
			return atan2(P.getX(), P.getZ());
		}
	}
	else if(CoordSystem == 's')//scanner
	{
		if(fabs(P.getX()) < epsilon)
			return 0;
		else
		{
			return atan2(P.getY(), fabs(P.getX()));
		}
	}
}

double Phi(const geom_Point3 &P, const char CoordSystem)
{
	//double epsilon = 1e-6;
	if(CoordSystem == 'd')//dave
	{
		if(fabs(P.getZ()) < epsilon)
			return 0;
		else
		{
			//return atan(P.getY()/ fabs(P.getZ()));
			return atan2(P.getY(), P.getZ());
		}
	}
	else if(CoordSystem == 's')//scanner
	{
		if(fabs(P.getZ()) < epsilon)
			return 0;
		else
		{
			return atan2(P.getZ(), fabs(P.getX()));
		}
	}
}

/*
double Theta(const geom_Point3 &P)
{
	
	if(fabs(P.getZ()) < epsilon)
		return 0;
	else
	{
		//return atan(P.getX()/fabs(P.getZ()));
		return atan2(P.getX(), P.getZ());
	}
}

double Phi(const geom_Point3 &P)
{
	if(fabs(P.getZ()) < epsilon)
		return 0;
	else
	{
		//return atan(P.getY()/ fabs(P.getZ()));
		return atan2(P.getY(), P.getZ());
	}
}

double ThetaScannerFrame(const geom_Point3 &P)
{
	if(fabs(P.getX()) < epsilon)
		return 0;
	else
	{
		return atan2(P.getY(), fabs(P.getX()));
	}
}

double PhiScannerFrame(const geom_Point3 &P)
{
	if(fabs(P.getZ()) < epsilon)
		return 0;
	else
	{
		return atan2(P.getZ(), fabs(P.getX()));
	}
}
*/

double Length(const geom_Point3 &P)
{
	//assume the scanner is at (0,0,0)
	double x = P.getX();
	double y = P.getY();
	double z = P.getZ();
	return sqrt(x*x + y*y + z*z);
}

void BoundingFrustrum(const ModelFile &Object, double &MinTheta, double &MaxTheta, double &MinPhi, double &MaxPhi, char Angles)
{
	if(V >= 2)
		cout << "Enter BoundingFrustrum()" << endl;
	
	//find the bounding frustrum of the object
	int method = 2;
	
	if(method == 1) //using obj file bounding box
	{
		geom_OrientedCube BoundingBox = Object.BoundingBox();
	
		geom_Point3 LowerLeft = BoundingBox.FrontLowerLeft();
		geom_Point3 LowerRight = BoundingBox.FrontLowerRight();
		geom_Point3 UpperLeft = BoundingBox.FrontUpperLeft();
		geom_Point3 UpperRight = BoundingBox.FrontUpperRight();
		
		vector<geom_Point3> BoundingPoints;
		BoundingPoints.push_back(LowerLeft);
		BoundingPoints.push_back(LowerRight);
		BoundingPoints.push_back(UpperLeft);
		BoundingPoints.push_back(UpperRight);
	
		//Write bounding points
		/*
		VtkPolydataFile Bounding(BoundingPoints);
		stringstream BoundingStream;
		BoundingStream << Path << "Bounding_" << PositionIndex << ".vtk";
		Bounding.Write(BoundingStream.str());
		*/
		
		//Dave's way
		if( (Angles == 'd') || (Angles == 's') )
		{
	 		MinTheta = Theta(LowerLeft, Angles);
			MaxTheta = Theta(LowerRight, Angles);
			MinPhi = Phi(LowerLeft, Angles);
			MaxPhi = Phi(UpperLeft, Angles);	
		}
		else if(Angles == 'e')//Eric's Way
		{
			
			double theta, phi;
			CartesianToSpherical(LowerLeft, theta, phi);
			MinTheta = theta;
			CartesianToSpherical(LowerRight, theta, phi);
			MaxTheta = theta;
			CartesianToSpherical(LowerLeft, theta, phi);
			MinPhi = phi;
			CartesianToSpherical(UpperLeft, theta, phi);
			MaxPhi = phi;
		}

		
		
		/*
		cout << "LowerLeft: " << LowerLeft << endl
				<< "LowerRight: " << LowerRight << endl
				<< "UpperLeft: " << UpperLeft << endl
				<< "UpperRight: " << UpperRight << endl;
		*/
		
		//bounding theta and phi
		cout << "MinTheta: " << MinTheta << endl
				<< "MaxTheta: " << MaxTheta << endl
				<< "MinPhi: " << MinPhi << endl
				<< "MaxPhi: " << MaxPhi << endl;
		
	}
	else if(method == 2) //using loop through all points
	{
		MinTheta = 1e6;
		MaxTheta = -1e6;
		MinPhi = 1e6;
		MaxPhi = -1e6;
		
		double theta, phi;
		for(int i = 0; i < Object.NumPoints(); i++)
		{
			geom_Point3 P = Object.getPoint(i);
			if( (Angles == 'd') || (Angles == 's'))
			{
				theta = Theta(P,Angles);
				phi = Phi(P,Angles);
			}
			else if(Angles == 'e')
			{
				CartesianToSpherical(P, theta, phi);
			}
			
			if(theta < MinTheta)
				MinTheta = theta;
			
			if(theta > MaxTheta)
				MaxTheta = theta;
			
			if(phi < MinPhi)
				MinPhi = phi;
			
			if(phi > MaxPhi)
				MaxPhi = phi;
		}//end loop through object opoints
	}//end else
	
	
	
}

void SetZero(double &d)
{
	
	if(fabs(d) < epsilon)
		d = 0;
}

void CartesianToSpherical(const geom_Point3 &P, double &theta, double &phi)
{
	double x = P.getX();
	double y = P.getY();
	double z = P.getZ();
	double r = sqrt(x*x+y*y+z*z);
	theta = 0.0;
	phi = 0.0;

	if(r > 0)  
		phi = acos(z/r);

	if( (x*x + y*y) > 0)
		theta = atan2(y, x);

	if (theta < 0.0)
		theta += 2.*3.14159;

}


int NumTransformations(const string Filename)
{	
	ifstream TransformFile(Filename.c_str());
	int n;
	TransformFile >> n;
	TransformFile.close();
	return n;
}

void ReadAllTransformations(const string Filename, vector<geom_Matrix3> &Rotations, vector<geom_Vector3> &Translations)
{
	geom_Matrix3 R;
	geom_Vector3 T;
	int NumTrans = NumTransformations(Filename);
	for(int i = 0; i < NumTrans; i++)
	{
		ReadTransformation(Filename, i, R, T);
		Rotations.push_back(R);
		Translations.push_back(T);
		
		cout << "Translation " << i << ": " << T << endl;
	}
	
	
}

void ReadTransformation(const string Filename, const int n, geom_Matrix3 &R, geom_Vector3 &T)
{
	assert(n <= NumTransformations(Filename));
	
	ifstream TransformFile(Filename.c_str());
	string temp;
		
	//throw away first line (number of transformations)
	getline(TransformFile, temp);
	
	//throw away all the transformations we are not interested in
	for(int i = 0; i < n; i++) //naming transformations start at 1
		getline(TransformFile, temp);
	
	//read the transform we are interested in
	vector<double> Rotation(9);
	vector<double> Translation(3);

	for(int i = 0; i < 3; i++)
		TransformFile >> Translation[i];
	
	for(int i = 0; i < 9; i++)
		TransformFile >> Rotation[i];
	
	TransformFile.close();
	
	geom_Matrix3 RT(Rotation);
	R = RT.Transpose();
	T = geom_Vector3(Translation);

}

vector<geom_Point3> PointsInsideFrustrum(const ModelFile &World, const ModelFile &Model, char Angles)
{
	double MinTheta, MaxTheta, MinPhi, MaxPhi;
	BoundingFrustrum(Model, MinTheta, MaxTheta, MinPhi, MaxPhi, Angles);
	
	vector<geom_Point3> WorldPoints;

	for(int i = 0; i < World.NumPoints(); i++)
	{
		geom_Point3 WorldPoint = World.getPoint(i);
	
		//if(!WorldPoint.IsValid())
		//	continue;
			
		double theta, phi;
		
		if((Angles == 'd') || (Angles == 's') )
		{
			theta = Theta(WorldPoint, Angles);
			phi = Phi(WorldPoint, Angles);
		}
		else if (Angles == 'e')//Eric's way
		{
			CartesianToSpherical(WorldPoint, theta, phi);
		}
			
		//if(V >= 2)
			//cout << "Theta: " << theta << " Phi: " << phi << endl;
		
		//only consider points inside the bounding frustrum produced by the model at the current location
		/*
		if(MaxTheta < MinTheta)
		{
			double temp = MinTheta;
			MinTheta = MaxTheta;
			MaxTheta = temp;
		}
		if(MaxPhi < MinPhi)
		{
			double temp = MinPhi;
			MinPhi = MaxPhi;
			MaxPhi = temp;
		}
		*/
		
		if( (theta <= MaxTheta) && (theta >= MinTheta) && (phi <= MaxPhi) && (phi >= MinPhi) )
		{
			WorldPoints.push_back(WorldPoint);
		}
	}
	
	cout << "Points inside frustrum: " << WorldPoints.size() << endl;
	
	return WorldPoints;
}

void ReadEricFile(string filename, vector<geom_Point3> &points)
{
	FILE *file = fopen(filename.c_str(), "rb");
	while(!feof(file))
	{
		double point[3];
		fread(point, sizeof(double)*3, 1, file);
		points.push_back(geom_Point3(point[0], point[1], point[2]));
	}
	fclose(file);
}

void OutputVector(vector<double> &V)
{
	for(int i = 0; i < V.size(); i++)
		cout << V[i] << endl;
	
	cout << endl;
}

void GenerateColors(map<geom_Color<unsigned char>, int> &Map, vector<geom_Color<unsigned char> > &List, int num)
{
	geom_Color<unsigned char> junk(-1,-1,-1);
	List = vector<geom_Color<unsigned char> > (num + 1, junk);
	int ColorIndex = 0;
		
	for(unsigned char r = 0; r <= 254; r++)
	{
		//printf("r: %u\n", r);
		for(unsigned char g = 0; g <= 254; g++)
		{
			//printf("g: %u\n", g);
			for(unsigned char b = 0; b <= 254; b++)
			{
				//printf("b: %u\n", b);
				vector<unsigned char> color(3,0);
				color[0] = r;
				color[1] = g;
				color[2] = b;
				
				Map[color] = ColorIndex;
				List[ColorIndex] = color;
				//ColorList.push_back(color);
						
				ColorIndex++;
				if(ColorIndex == (num - 1))
					return;
			}
		}
	}
	
}