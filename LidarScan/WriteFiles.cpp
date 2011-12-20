#include "WriteFiles.h"

void WriteParaviewFile(const char* sOutputFileName, LidarScanner &Scanner)
{
  char* Filename = const_cast<char*>(sOutputFileName);
  std::ofstream fout_Paraview(strcat(Filename, ".py"), ios::app);
  fout_Paraview << "view=servermanager.GetRenderView()" << endl
		<< "view.ViewSize=[640,480]" << endl;
  //<< "view.CameraViewUp=" << Scanner.getUp() << endl
  //		<< "view.CameraViewAngle=" << 54.43 << endl
  //      << "view.CameraPosition=" << Scanner.getLocation() << endl;
  //<< "view.CameraFocalPoint=" << Scanner.getForward() + Scanner.getLocation();
  fout_Paraview.close();
}

void WriteRawPoints(const char* sOutputFileName, LidarScan &Scan)
{
  char* Filename = const_cast<char*>(sOutputFileName);
  std::ofstream output(strcat(Filename, ".raw"), ios::app);

  for (int i = 0; i < Scan.getThetaPoints(); i++)
    {
      Scan.getStrip(i).WriteToFile(output);
    }
}

