#include "vtkSphereSource.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkMath.h"
#include "vtkProperty.h"

#include "vtkQuadric.h"
#include "vtkSampleFunction.h"
#include "vtkContourFilter.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"

#include "vtkPolyData.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char* agrv[])
{
	
	vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName("/media/portable/Data/Cars/5cars/Models/0.vtp");
	reader->Update();
	vtkSmartPointer<vtkPolyData> polydata = reader->GetOutput();
  
	// Create a renderer
	vtkRenderer* ren = vtkRenderer::New();
	ren->SetBackground(1.0,1.0,1.0);

	// Create a line and add to the renderer
	vtkLineSource* line_src = vtkLineSource::New();
	line_src->SetPoint1(0.0, 0.0, 0.0);
	line_src->SetPoint1(1.0, 1.0, 1.0);
	vtkPolyDataMapper* line_mapper = vtkPolyDataMapper::New();
	line_mapper->SetInputConnection(line_src->GetOutputPort());
	vtkActor* line_actor = vtkActor::New();
	line_actor->SetMapper(line_mapper);
	line_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
	ren->AddActor(line_actor);
	/*
	// Create a sphere and add to the renderer
	vtkSphereSource* sphere_src = vtkSphereSource::New();
	vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
	mapper->SetInputConnection(sphere_src->GetOutputPort());
	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);
	ren->AddActor(actor);
	*/

	/*	
	// Add the car to the renderer
	vtkPolyDataMapper* car_mapper = vtkPolyDataMapper::New();
	car_mapper->SetInput(polydata);
	vtkActor* car_actor = vtkActor::New();
	car_actor->SetMapper(car_mapper);
	ren->AddActor(car_actor);
	*/
	
	// Create a render window
	vtkRenderWindow* renWin = vtkRenderWindow::New();
	renWin->AddRenderer( ren );
	renWin->SetSize( 800, 600);

	// Create an interactor
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	renWin->SetInteractor( iren );

	vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();
	iren->SetInteractorStyle( style );

	// Initialize and enter interactive mode
	iren->Initialize();
	iren->Start();
	
	for(unsigned int i = 0; i < 1e5; i++)
	{
		std::cout << i << std::endl;
	}

	return 0;
}

