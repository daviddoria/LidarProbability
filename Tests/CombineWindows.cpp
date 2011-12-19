#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <ModelFile/ModelFile.h>
#include <VNLGeometry/Color.h>

using namespace std;

int main()
{
	int verbosity = 0;
	
	string Path = "/home/doriad/Projects/LidarProbability/Data/WallInterpCoarse/Model_";
	
	//setup VTK stuff
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
	
	//for colors
	vtkSmartPointer< vtkCellArray > carray = vtkSmartPointer< vtkCellArray >::New();
	
	//for lines
	//make points into lines
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	
	//setup position index
	vtkSmartPointer<vtkIntArray> PositionIndex = vtkSmartPointer<vtkIntArray>::New(); 
	PositionIndex->SetName("PositionIndex");
	
	//setup colors
	//vtkSmartPointer<vtkDoubleArray> Colors = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkUnsignedCharArray> Colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	//vtkSmartPointer<vtkFloatArray> Colors = vtkSmartPointer<vtkFloatArray>::New();    
	
	Colors->SetNumberOfComponents(3);
	Colors->SetName("RGB colors");
	
	for(unsigned int model = 0; model < 13; model++)
	{
		//read the current window file
		stringstream FilenameStream;
		FilenameStream << Path << model << ".vtk";
		ModelFile Model(FilenameStream.str());
		
		vector<vgl_point_3d<double> > Points = Model.getPointCoords();
		vector<Color<unsigned char> > PointColors = Model.getColors();
		
		//go through all the points in the current model
		unsigned int PointCounter = 0;
		for(unsigned int point = 0; point < Points.size(); point++)
		{
			vgl_point_3d<double> CurrentPoint = Points[point];
			if(verbosity >= 1)
				cout << "Current point: " << CurrentPoint << endl;
			
			double PointArray[3];
			PointArray[0] = CurrentPoint.x();
			PointArray[1] = CurrentPoint.y();
			PointArray[2] = CurrentPoint.z();
			
			//store the current point of the current model in the output file
			int loc = pts->InsertNextPoint(PointArray);
			vtkIdType pts_ids[1];
			pts_ids[0] = loc;
			carray->InsertNextCell( 1, pts_ids );
			
			//store the point's color
			Color<unsigned char> ColorD = PointColors[point];
			
			Color<unsigned char> ColorC;
			
			//Colors->InsertNextTuple(ColorArray);
			//Colors->InsertNextTuple3(ColorArray[0], ColorArray[1], ColorArray[2]);
			//Colors->InsertNextTupleValue(ColorArray);
					
			//associate the current model number with the point
			PositionIndex->InsertNextValue(model);
			PointCounter++;
		}

		cout << "Added " << PointCounter << " points from model " << model << endl;
	}
	
	vtkSmartPointer<vtkPolyData> pdata = vtkSmartPointer<vtkPolyData>::New();
	
	//add the points to the dataset
	pdata->SetPoints(pts);
	
	//add the vertices to the dataset
	pdata->SetVerts(carray);
	
	//add the colors to the dataset
	//pdata->GetPointData()->SetVectors(Colors);
	//pdata->GetPointData()->SetActiveScalars(Colors->GetName());
	pdata->GetPointData()->AddArray(Colors);
	
	//add the position indexes to the dataset
	pdata->GetPointData()->AddArray(PositionIndex);
	
	//write the file
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetInput(pdata);
	string OutputFile = "/home/doriad/Projects/LidarProbability/Data/WallInterpCoarse/AllWindows.vtp";
	writer->SetFileName(OutputFile.c_str());
	writer->Write();
	
	return 0;
}