#include "LidarScan.h"

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


///////// Constructors ///////////
LidarScan::LidarScan()
{
	ScanParams.setNumThetaPoints(0);
	ScanParams.setNumPhiPoints(0);
	ScanParams.setPhiMin(0);
	ScanParams.setPhiMax(0);
	ScanParams.setThetaMin(0);
	ScanParams.setThetaMax(0);
}
	
LidarScan::LidarScan(unsigned int ThetaPoints, unsigned int PhiPoints)
{
	ScanParams.setNumThetaPoints(ThetaPoints);
	ScanParams.setNumPhiPoints(PhiPoints);
	
	LidarStrip EmptyStrip(PhiPoints);

	Strips_.resize(ScanParams.getNumThetaPoints());

	for(unsigned int i = 0; i < ScanParams.getNumThetaPoints(); i++)
	{
		Strips_[i] = EmptyStrip;
	}
}

/////////////// External Operators //////////////////
std::ostream& operator<<(std::ostream& output, const LidarScan &Scan)
{
  
  output << "LidarScan:" << endl
		<< "-----------" << endl
		<< "Phi Min: " << Scan.ScanParams.getPhiMin() << endl << "Phi Max: " << Scan.ScanParams.getPhiMax() << endl
		<< "Theta Min: " << Scan.ScanParams.getThetaMin() << endl << "Theta Max: " << Scan.ScanParams.getThetaMax() << endl
		<< "Phi Points: " << Scan.ScanParams.getNumPhiPoints() << endl << "Theta Points: " << Scan.ScanParams.getNumThetaPoints() << endl
		<< "Total Points: " << Scan.ScanParams.TotalPoints() << endl;
/*
	for(int i = 0; i < Scan.NumStrips(); i++)
	{
		output << "Strip " << i << ": Angle: " << Scan.getThetaAngle(i) << " NumHits: " << Scan.getStrip(i).NumHits() << endl;
	}
*/
	output << endl;

	return output;
}

////////// Accessors ///////////////

double LidarScan::AverageHitsPerStrip() const
{
	double hits = 0;
	for(unsigned int i = 0; i < NumStrips(); i++)
	{
		hits += Strips_[i].NumHits();
	}
	return hits / NumNonEmptyStrips();
}

unsigned int LidarScan::NumHits() const
{
	unsigned int hits = 0;
	for(unsigned int i = 0; i < NumStrips(); i++)
	{
		hits += Strips_[i].NumHits();
	}
	return hits;
}

unsigned int LidarScan::FirstNonEmptyStrip() const
{
	unsigned int hits = 0;
	for(unsigned int i = 0; i < NumStrips(); i++)
	{
		hits = Strips_[i].NumHits();
		if(hits > 0)
			return i;
	}
	
	return -1;//there are no non-empty strips
}

unsigned int LidarScan::NumNonEmptyStrips() const
{
	unsigned int  counter = 0;
	for(unsigned int i = 0; i < ScanParams.getNumThetaPoints(); i++)
	{
		if(Strips_[i].NumHits() != 0)
			counter++;
	}
	return counter;
}
		  
LidarStrip LidarScan::getStrip(const unsigned int strip) const 
{
	if( (strip >= 0) && (strip < Strips_.size()) )
	{
		return Strips_[strip];
	}
	else
	{
		cout << "Invalid strip index!" << endl;
		return LidarStrip();
	}
	  
}

//////// Mutators ////////
void LidarScan::setStrip(const unsigned int ind, const LidarStrip &Strip)
{
	if( (ind < Strips_.size()) && (ind >= 0) )
	{
		Strips_[ind] = Strip;
	}
	else
	{
		cout << "Index error in LidarScan::setStrip!" << endl
		<< "Requested ind: " << ind << endl
		<< "Strips_.size(): " << Strips_.size() << endl << endl;
	}
}

///////////////// Functions /////////////////////
void LidarScan::WriteLidFile(const string &sFileName)
{
	ofstream output(sFileName.c_str(), ios::out);

	//Angle Info
	output << ScanParams.getNumPhiPoints() << " " << ScanParams.getPhiMin() << " " << ScanParams.getPhiMax()
			<< " " << ScanParams.getNumThetaPoints() << " " << ScanParams.getThetaMin()
			<< " " << ScanParams.getThetaMax() << endl;

	output << ScanParams.getLocation() << endl << ScanParams.getForward() << endl;

	for (unsigned int theta = 0; theta < ScanParams.getNumThetaPoints(); theta++)
	{
		output << Strips_[theta];
	}

	output.close();

	cout << "Finished writing .lid file." << endl;

}

void LidarScan::CreateModelFile(ModelFile &Model)
{
	vector<vgl_point_3d<double> > Points;
	vector<vgl_vector_3d<double> > Normals;
	
	for (unsigned int theta = 0; theta < ScanParams.getNumThetaPoints(); theta++)
	{
		LidarStrip Strip = Strips_[theta];
		for(unsigned int i = 0; i < Strip.NumPoints(); i++)
		{
			vgl_point_3d<double> P = Strip.getPoint(i).getCoord();
			if(Strip.getPoint(i).getHit())
			{
				Points.push_back(P);
				Normals.push_back(Strip.getPoint(i).getNormal());
			}
		}
	}

	
	Model.setCoords(Points);
	Model.setNormals(Normals);
	Model.setScannerLocation(ScanParams.getLocation());
}

void LidarScan::ReadLidFile(const std::string &Filename, bool &success)
{
	int verbosity = 0;
	
	if(verbosity >= 1)
		cout << "Reading .lid file " << Filename << "..." << endl;
	
	ifstream infile;
	infile.open(Filename.c_str());
	if(!infile)
	{
		cout << "Could not open file!" << endl;
		success = false;
		return;
	}

	string line;

	//read scan parameters
	getline(infile, line);
	double numphi, phimin, phimax, numtheta, thetamin, thetamax;
	
	stringstream(line) >> numphi >> phimin >> phimax >> numtheta >> thetamin >> thetamax;
	ScanParams.setNumPhiPoints(numphi);
	ScanParams.setPhiMin(phimin);
	ScanParams.setPhiMax(phimax);
	ScanParams.setNumThetaPoints(numtheta);
	ScanParams.setThetaMin(thetamin);
	ScanParams.setThetaMax(thetamax);
	
	//read scannerlocation
	getline(infile, line);
	double locx,locy,locz;
	stringstream(line) >> locx >> locy >> locz;
	//ScanParams.setLocation(vgl_point_3d<double>(locx,locy,locz)); //!!!
	
	//read scanner forward
	getline(infile, line);
	double fwdx, fwdy, fwdz;
	stringstream(line) >> fwdx >> fwdy >> fwdz;
	//ScanParams.setForward(vgl_vector_3d<double>(fwdx, fwdy, fwdz));//!!!
	
	//empty the current contents of the scan, if any
	Strips_.clear();
	Strips_.resize(ScanParams.getNumThetaPoints());
	
	//for now, up is always +y
	//ScanParams.setUp(vgl_vector_3d<double>(0,1,0));//!!!
	
	for(unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++)
	{
		LidarStrip Strip(ScanParams.getNumPhiPoints());
		for(unsigned int point = 0; point < ScanParams.getNumPhiPoints(); point++)
		{
			getline(infile, line);
			bool hit;

			vector<double> coords(3,0);
			vector<double> dir(3,0);
			stringstream(line) >> hit >> coords[0] >> coords[1] >> coords[2] >> dir[0] >> dir[1] >> dir[2];
			
			LidarPoint Point(vgl_point_3d<double>(coords[0], coords[1], coords[2]), vgl_vector_3d<double>(dir[0], dir[1], dir[2]), CalculateDepth(vgl_point_3d<double>(coords[0], coords[1], coords[2]), ScanParams.getLocation(), ScanParams.getForward()), CalculateHeight(vgl_point_3d<double>(coords[0], coords[1], coords[2]), ScanParams.getLocation(), ScanParams.getUp()), hit);
			
			Strip.setPoint(point, Point);
		}
		Strips_[strip] = Strip;
	}
	
	if(verbosity >= 1)
		cout << "Finished reading .lid file " << Filename << "." << endl;
	
	success = true;
}


double LidarScan::CalculateDepth(const vgl_point_3d<double> &Point, const vgl_point_3d<double> &ScannerLocation, const vgl_vector_3d<double> &ScannerForward) const
{
	vgl_vector_3d<double> a = ScannerForward;
	vgl_vector_3d<double> b = ScannerLocation - Point;
	vgl_vector_3d<double> projected = (dot_product(a,b) / a.length()) * a/a.length();
	return projected.length();
}

double LidarScan::CalculateHeight(const vgl_point_3d<double> &Point, const vgl_point_3d<double> &ScannerLocation, const vgl_vector_3d<double> &ScannerUp) const
{
	vgl_vector_3d<double> a = ScannerUp;
	vgl_vector_3d<double> b = ScannerLocation - Point;
	vgl_vector_3d<double> projected = (dot_product(a,b) / a.length()) * a/a.length();
	return projected.length();
}

std::vector<vgl_point_3d<double> > LidarScan::GetAllPoints() const
{
	if(ScanParams.TotalPoints() > 0)
	{
		std::vector<vgl_point_3d<double> > Points;
	
		for (unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++)
		{
			vector<vgl_point_3d<double> > StripPoints = Strips_[strip].GetAllPoints();
			for(unsigned int i = 0; i < StripPoints.size(); i++)
			{
				Points.push_back(StripPoints[i]);
			}
		}
		
		return Points;
	}
	else
	{
		std::vector<vgl_point_3d<double> > Points;
		return Points;
	}
	
}

void LidarScan::WriteBinaryHitImage(const std::string &Filename)
{
	//vil_image_view<vil_rgb<vxl_byte> > ColorImg(ScanParams.getNumPhiPoints(), ScanParams.getNumThetaPoints(), 1, 1); //(ni, nj, n_planes, n_interleaved_planes)
	vil_image_view<vil_rgb<vxl_byte> > ColorImg(ScanParams.getNumThetaPoints(), ScanParams.getNumPhiPoints(), 1, 1); //(ni, nj, n_planes, n_interleaved_planes)
	
	unsigned int h = ScanParams.getNumPhiPoints();
	unsigned int w = ScanParams.getNumThetaPoints();
	
	std::cout << "H (Phi Points) = " << h << std::endl;
	std::cout << "W (Theta Points) = " << w << std::endl;
	
	for (unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++) //cols
	{
		for(unsigned int point = 0; point < ScanParams.getNumPhiPoints(); point++) //rows
		{
			//Color<unsigned char> C = Strips_[strip].getPoint(point).getColor();
			//ColorImg(point,strip) = vil_rgb<vxl_byte>(C.getR(), C.getG(), C.getB());
			
			//unsigned int r = point;
			//unsigned int c = strip;
			
			unsigned int r = h-1-point;
			unsigned int c = w-1-strip;
			
			std::cout << "Current pixel: (r,c) = (" << r << ", " << c << ")" << std::endl;
			
			bool hit = Strips_[strip].getPoint(point).getHit();
			if(hit)
			{
				//ColorImg(r, c) = vil_rgb<vxl_byte>(255,255,255);//white
				ColorImg(c,r) = vil_rgb<vxl_byte>(255,255,255);//white
			}
			else
			{
				//ColorImg(r,c) = vil_rgb<vxl_byte>(0,0,0);//black
				ColorImg(c,r) = vil_rgb<vxl_byte>(0,0,0);//black
			}
		}
	}
	
	vil_save(ColorImg, Filename.c_str());
	std::cout << "Wrote " << Filename << std::endl;
}

double LidarScan::AverageDepth(void)
{
	unsigned int NumHits = 0;
	double TotalDepth = 0.0;
	for (unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++) //cols
	{
		for(unsigned int point = 0; point < ScanParams.getNumPhiPoints(); point++) //rows
		{
			bool hit = Strips_[strip].getPoint(point).getHit();
			if(hit)
			{
				TotalDepth += CalculateDepth(Strips_[strip].getPoint(point).getCoord(), ScanParams.getLocation(), ScanParams.getForward());
				NumHits++;
			}
			
		}
	}
	
	return TotalDepth / static_cast<double>(NumHits);
}


void LidarScan::WriteImage(const std::string &Filename)
{
	vil_image_view<vil_rgb<vxl_byte> > ColorImg(ScanParams.getNumThetaPoints(), ScanParams.getNumPhiPoints(), 1, 1); //(ni, nj, n_planes, n_interleaved_planes)
	
	unsigned int h = ScanParams.getNumPhiPoints();
	unsigned int w = ScanParams.getNumThetaPoints();
	
	std::cout << "H (Phi Points) = " << h << std::endl;
	std::cout << "W (Theta Points) = " << w << std::endl;
	
	double AvDepth = AverageDepth();
	
	for (unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++) //cols
	{
		for(unsigned int point = 0; point < ScanParams.getNumPhiPoints(); point++) //rows
		{

			unsigned int r = h-1-point;
			unsigned int c = w-1-strip;
			
			//std::cout << "Current pixel: (r,c) = (" << r << ", " << c << ")" << std::endl;
			
			bool hit = Strips_[strip].getPoint(point).getHit();
			if(hit)
			{
				double depth = CalculateDepth(Strips_[strip].getPoint(point).getCoord(), ScanParams.getLocation(), ScanParams.getForward());
				unsigned char val = (depth / AvDepth) * 255.;
				std::cout << "Value: " << val << std::endl;
				
				ColorImg(c,r) = vil_rgb<vxl_byte>(val,val,val);
			}
			else
			{
				ColorImg(c,r) = vil_rgb<vxl_byte>(0,0,0);//black
			}
		}
	}
	
	vil_save(ColorImg, Filename.c_str());
	std::cout << "Wrote " << Filename << std::endl;
}


bool LidarScan::WritePTX(const std::string &Filename) const
{
	std::ofstream fout(Filename.c_str(), ios::out);

	fout << ScanParams.getNumPhiPoints() << std::endl
			<< ScanParams.getNumThetaPoints() << std::endl
			<< "0 0 0" << std::endl
			<< "1 0 0" << std::endl
			<< "0 1 0" << std::endl
			<< "0 0 1" << std::endl
			<< "1 0 0 0" << std::endl
			<< "0 1 0 0" << std::endl
			<< "0 0 1 0" << std::endl
			<< "0 0 0 1" << std::endl;
	
	for(unsigned int strip = 0; strip < ScanParams.getNumThetaPoints(); strip++)
	//for (int strip = ScanParams.getNumThetaPoints() - 1; strip >= 0; strip--)
	{
		LidarStrip CurrentStrip = Strips_[strip];
		//std::cout << CurrentStrip.NumHits() << endl;
		
		//for(unsigned int point = 0; point < CurrentStrip.NumPoints(); point++)
		for(int point = CurrentStrip.NumPoints() - 1; point >= 0; point--)
		{
			LidarPoint CurrentPoint = CurrentStrip.getPoint(point);
			if(!CurrentPoint.getHit())
			{
				vgl_vector_3d<double> dir = CurrentPoint.getDirection();
				normalize(dir);
				vgl_point_3d<double> UnitPoint = ScanParams.getLocation() + dir;
				
				fout << UnitPoint.x() << " " << UnitPoint.y() << " " << UnitPoint.z() << " .5 0 0 0" << std::endl;
			}
			else
			{
				vgl_point_3d<double> CurrentCoord = CurrentPoint.getCoord();
				fout << CurrentCoord.x() << " " << CurrentCoord.y() << " " << CurrentCoord.z() << " .25 100 100 100" << std::endl;
			}
		}
	
	}
	
	fout.close();
		
	return true;//write successful
}