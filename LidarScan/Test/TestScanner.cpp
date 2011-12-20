#include <iostream>
#include <vector>

#include <LidarScan/LidarScanner.h>
#include <LidarScan/LidarScan.h>
#include <LidarScan/LidarStrip.h>
#include <LidarScan/LidarPoint.h>

#include <vul/vul_timer.h>

#include <omp.h>

void TestOutput();
void TestAngles();

void TestSetThetaPhi();

void TestIsRightHanded();

int main(int argc, char* argv[])
{
	//  TestOutput();
  	//  TestAngles();
	
	//TestSetThetaPhi();
	TestIsRightHanded();
	return 0;
}


void TestOutput()
{
	LidarScanner Scanner;
	cout << Scanner;
}

void TestAngles()
{
	LidarScanner Scanner;

	std::cout << "Theta Angles: " << std::endl
			<< "-------------" << std::endl;
/*
	for(int i = 0; i < Scanner.ScanParams.getNumThetaPoints(); i++)
		std::cout << Scanner.getThetaAngle(i) << std::endl;

	std::cout << "Phi Angles: " << std::endl
			<< "-------------" << std::endl;

	for(int i = 0; i < Scanner.ScanParams.getNumPhiPoints(); i++)
		std::cout << Scanner.getPhiAngle(i) << std::endl;

*/
}

void TestSetThetaPhi()
{

	LidarScanner Scanner;
	//vgl_vector_3d<double> V = Scanner.SetThetaPhi(PI/2, 0);
	vgl_vector_3d<double> V = Scanner.SetThetaPhi(0, M_PI/2);
	
	
	std::cout << "right: " << Scanner.ScanParams.getRight() << std::endl;
	std::cout << "forward: " << Scanner.ScanParams.getForward() << std::endl;
	std::cout << "up: " << Scanner.ScanParams.getUp() << std::endl;
	
	std::cout << "new: " << V << std::endl;
}

void TestIsRightHanded()
{
//BROKEN!!!
	LidarScanner Scanner;

	std::cout << "Right: " << Scanner.ScanParams.getRight() << std::endl;
	std::cout << "Forward: " << Scanner.ScanParams.getForward() << std::endl;
	std::cout << "Up: " << Scanner.ScanParams.getUp() << std::endl;
	std::cout << "Right Handed? " << IsRightHanded(Scanner.ScanParams.getRight(), Scanner.ScanParams.getForward(), Scanner.ScanParams.getUp()) << endl;

	vgl_vector_3d<double> T(0.0, 0.0, 0.0);
	//vnl_double_3x3 R = MakeRotation('x', M_PI/2);
	//Transformation Trans(T,R);

	//Scanner.ScanParams.setTransformation(Trans);

	std::cout << "Right: " << Scanner.ScanParams.getRight() << std::endl;
	std::cout << "Forward: " << Scanner.ScanParams.getForward() << std::endl;
	std::cout << "Up: " << Scanner.ScanParams.getUp() << std::endl;
	std::cout << "Right Handed? " << IsRightHanded(Scanner.ScanParams.getRight(), Scanner.ScanParams.getForward(), Scanner.ScanParams.getUp()) << std::endl;


}