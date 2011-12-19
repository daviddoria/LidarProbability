#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkLine.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkXMLPolyDataWriter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include <ModelFile/ModelFile.h>

#include <vgl/vgl_point_3d.h>

int main()
{

	//string Path = "/home/doriad/Projects/LidarProbability/Data/WallInterpCoarse/";
	string Path = "/home/doriad/Projects/LidarProbability/Data/WallInterpFine/";
	
	//setup VTK stuff
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
	
	//setup position index
	vtkSmartPointer<vtkIntArray> PositionIndex = vtkSmartPointer<vtkIntArray>::New(); 
	PositionIndex->SetName("PositionIndex");

	//read the current window file
	stringstream FilenameStream;
	FilenameStream << Path << "PlotPositions.vtk";
	ModelFile Model(FilenameStream.str());
	
	vector<vgl_point_3d<double> > Points = Model.getPointCoords();
	
	for(unsigned int point = 0; point < Points.size(); point++)
	{		
		vgl_point_3d<double> CurrentPoint = Points[point];
		
		double PointArray[3];
		PointArray[0] = CurrentPoint.x() + 1; //move the plot infront of the wall
		PointArray[1] = CurrentPoint.y();
		PointArray[2] = CurrentPoint.z() + 4; //raise the plot above the windows
		
		//store the current point of the current model in the output file
		int loc = pts->InsertNextPoint(PointArray);
		vtkIdType pts_ids[1];
		pts_ids[0] = loc;

	}
	
	for(unsigned int pos = 0; pos < Points.size(); pos++)
	{
		vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
		
		//for(int i = 0; i <= pos; i++)
		for(unsigned int i = 0; i < pos; i++)
		{
			vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
			line->GetPointIds()->SetId(0,i);
			line->GetPointIds()->SetId(1,i+1);
			lines->InsertNextCell(line);
		}
		
		vtkSmartPointer<vtkPolyData> pdata = vtkSmartPointer<vtkPolyData>::New();
	
		//add the points to the dataset
		pdata->SetPoints(pts);
	
		//add the lines to the dataset
		pdata->SetLines(lines);
	
		//add the vertices to the dataset
		//pdata->SetVerts(carray);
	
		//write the file
		vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
		writer->SetInput(pdata);
		
		stringstream OutputFileStream;
		OutputFileStream << Path << "Plot" << pos << ".vtp";
		writer->SetFileName(OutputFileStream.str().c_str());
		writer->Write();  
		
		cout << "Wrote Plot file for position " << pos << endl;
	}

	return 0;
}