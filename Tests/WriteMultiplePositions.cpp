#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include <ModelFile/ModelFile.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_double_3x3.h>

using namespace std;

#ifndef EPSILON
const double epsilon = 1e-6;
#endif

	
double Theta(const vgl_point_3d<double> &P, const char CoordSystem);
double Phi(const vgl_point_3d<double> &P, const char CoordSystem);

void SetZero(double &d);
void ReadEricFile(const string filename, vector<vgl_point_3d<double> > &points);

int BadPoints = 0, GoodPoints = 0, Uninformative = 0;

void CartesianToSpherical(const vgl_point_3d<double> &P, double &theta, double &phi);

void ReadAllTransformations(const string &Filename, vector<vnl_double_3x3> &Rotations, vector<vgl_vector_3d<double> > &Translations);
void ReadTransformation(const string &Filename, const int n, vnl_double_3x3 &R, vgl_vector_3d<double> &T);
int NumTransformations(const string &Filename);

string FileExtension(const string &Filename);
string GetFilename(const string &Filename);


int V = 2; //verbosity

double x,y,z;
int PositionIndex = 1;

int main(int argc, char *argv[])
{
	bool ReadSuccess;
	
	string XformFilename = argv[1];
	string ModelFilename = argv[2];
	
	string Basename = vul_file::basename(ModelFilename);
	string Extension = vul_file::extension(ModelFilename);
	
	
	cout << "ModelName: " << Basename << endl;
	cout << "Extension: " << Extension << endl; 
	
	ModelFile Original;
	ReadSuccess = Original.Read(ModelFilename);
	
	vector<vgl_point_3d<double> > PositionList;
	vector<vnl_double_3x3> Rotations;
	vector<vgl_vector_3d<double> > Translations;
	
	ReadAllTransformations(XformFilename, Rotations, Translations);
	
	//%05d
	int NumPositions = Rotations.size();
	
	cout << "There are " << NumPositions << " positions." << endl;
	
	for(PositionIndex = 0; PositionIndex < NumPositions; PositionIndex++)
	{
		ModelFile Model = Original;
		
		Model.Rotate(Rotations[PositionIndex]);
		Model.Translate(Translations[PositionIndex]);
					
		stringstream NameStream;
		
		char result[5];
		sprintf(result, "%05d", PositionIndex);
		
		NameStream << Basename << "_" << string(result) << "." << Extension;
		
		cout << "Writing " << NameStream.str() << endl;
		
		Model.Write(NameStream.str());
	}
	
	return 0;
}
/*
string FileExtension(const string &Filename)
{
	string ext = Filename.substr(Filename.size() - 3, 3);
	cout << ext << endl;
	return ext;
}
*/


double Theta(const vgl_point_3d<double> &P, const char CoordSystem)
{
	double epsilon = 1e-6;
	if(CoordSystem == 'd')//dave
	{
		if(fabs(P.z()) < epsilon)
			return 0;
		else
		{
			//return atan(P.getX()/fabs(P.getZ()));
			return atan2(P.x(), P.z());
		}
	}
	else if(CoordSystem == 's')//scanner
	{
		if(fabs(P.x()) < epsilon)
			return 0;
		else
		{
			return atan2(P.y(), fabs(P.x()));
		}
	}

	
	assert(0);
	return 0.0;
}

double Phi(const vgl_point_3d<double> &P, const char CoordSystem)
{
	//double epsilon = 1e-6;
	if(CoordSystem == 'd')//dave
	{
		if(fabs(P.z()) < epsilon)
			return 0.0;
		else
		{
			//return atan(P.getY()/ fabs(P.getZ()));
			return atan2(P.y(), P.z());
		}
	}
	else if(CoordSystem == 's')//scanner
	{
		if(fabs(P.z()) < epsilon)
			return 0.0;
		else
		{
			return atan2(P.z(), fabs(P.x()));
		}
	}

	assert(0);
	return 0.0;
}


void SetZero(double &d)
{
	
	if(fabs(d) < epsilon)
		d = 0.0;
}

void CartesianToSpherical(const vgl_point_3d<double> &P, double &theta, double &phi)
{
	double x = P.x();
	double y = P.y();
	double z = P.z();
	double r = sqrt(x*x+y*y+z*z);
	theta = 0.0;
	phi = 0.0;

	if(r > 0.0)  
		phi = acos(z/r);

	if( (x*x + y*y) > 0.0)
		theta = atan2(y, x);

	if (theta < 0.0)
		theta += 2.*3.14159;

}


int NumTransformations(const string &Filename)
{	
	ifstream TransformFile(Filename.c_str());
	int n;
	TransformFile >> n;
	TransformFile.close();
	return n;
}

void ReadAllTransformations(const string &Filename, vector<vnl_double_3x3> &Rotations, vector<vgl_vector_3d<double> > &Translations)
{
	vnl_double_3x3 R;
	vgl_vector_3d<double> T;
	int NumTrans = NumTransformations(Filename);
	for(int i = 0; i < NumTrans; i++)
	{
		ReadTransformation(Filename, i, R, T);
		Rotations.push_back(R);
		Translations.push_back(T);
		
		cout << "Translation " << i << ": " << T << endl;
	}
	
	
}

void ReadTransformation(const string &Filename, const int n, vnl_double_3x3 &R, vgl_vector_3d<double> &T)
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
	
	/*
	vnl_double_3x3 RT(Rotation);
	R = RT.Transpose();
	T = vgl_vector_3d<double> (Translation);
	*/
}


void ReadEricFile(const string &filename, vector<vgl_point_3d<double> > &points)
{
	FILE *file = fopen(filename.c_str(), "rb");
	while(!feof(file))
	{
		double point[3];
		fread(point, sizeof(double)*3, 1, file);
		points.push_back(vgl_point_3d<double> (point[0], point[1], point[2]));
	}
	fclose(file);
}
