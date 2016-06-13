// Mandelbrot application benchmark for the experiment framework.
// Copyright (C) 2011  Håkan Sundell
// Copyright (C) 2014 - 2015  Anders Gidenstam
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NBLExpMandelbrot.h"

#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include <unistd.h>

#include "primitives.h"

const  int RESOLUTION_X = 4*2048;
const  int RESOLUTION_Y = 4*2048;
static int REGION_SIZE_X = 4;
static int REGION_SIZE_Y = 4;
static int NR_REGIONS_X  = RESOLUTION_X/REGION_SIZE_X;
static int NR_REGIONS_Y  = RESOLUTION_Y/REGION_SIZE_Y;

const double X_MIN = -1.50;
const double X_MAX =  0.50;
const double SCALE =  (X_MAX-X_MIN)/(RESOLUTION_X);
const double Y_MIN = -(X_MAX-X_MIN)/2;
const double Y_MAX = -Y_MIN;
#define Sleep usleep

struct Coord
{
  int X;
  int Y;
  int Z;
};

struct Int32Rect
{
  int X;
  int Y;
  int Width;
  int Height;
};

struct ImageRegion
{
  Int32Rect region;
  Coord* values;

  ImageRegion()
  {
    values = new Coord[REGION_SIZE_Y*REGION_SIZE_X];
  }
  ~ImageRegion()
  {
    delete[] values;
  }
  Coord& Get(int x, int y) {
    return values[x + y*REGION_SIZE_X];
  }
};

ImageRegion *regionBufferGlobal = 0;
int image[RESOLUTION_Y][RESOLUTION_X];

static int CalcZ(double xCoord, double yCoord);
static int FromHSV(int h, int s, int v);
static void SavePPM(string filename);

static int CalcColor(int z)
{
  return FromHSV(z, 255, 255);
}

static void GenerateGrid(ImageRegion *ir, double xStart, double yStart, int width, int height, int xIndex, int yIndex, double scale);

static void Produce(ImageRegion *region, int index);

static void Consume(ImageRegion *region);

static volatile int nrAdders=0;
static volatile int nrSums=0;

static double volatile test=0;
static double volatile resultMandelbrot;

NBLExpApplicationMandelbrot::NBLExpApplicationMandelbrot(void)
{
  PATTERN_NR=0;
  CONTENTION_NR=0;
}

NBLExpApplicationMandelbrot::~NBLExpApplicationMandelbrot(void)
{
}

string NBLExpApplicationMandelbrot::GetExperimentName()
{
  return string("ApplicationMandelbrot");
}

string NBLExpApplicationMandelbrot::GetCaseName()
{
  std::stringstream ss;
  ss << "-com.pat." << PATTERN_NR
     << "-cont." << CONTENTION_NR;
  return ss.str();
}

vector<string> NBLExpApplicationMandelbrot::GetParameters()
{
  vector<string> v;
  v.push_back(string("Communication pattern"));
  v.push_back(string("Contention level"));
  return v;
}

vector<string> NBLExpApplicationMandelbrot::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    v.push_back(string("0. N/2 Producer - N/2 Consumer threads. CPU-heavy producers."));
    break;
  case 1:
    for (int i=0; i<10; i++) {
      std::ostringstream text;
      text << i << ". Work unit size "
           <<  pow(2, i+1) << "x" << pow(2, i+1) << " pixels.";
      v.push_back(text.str());
    }
    break;
  }
  return v;
}

int NBLExpApplicationMandelbrot::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return PATTERN_NR;
  case 1:
    return CONTENTION_NR;
  default:
    return -1;
  }
}

void NBLExpApplicationMandelbrot::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    PATTERN_NR=value;
    break;
  case 1:
    CONTENTION_NR=value;
    break;
  default:
    break;
  }
}

void NBLExpApplicationMandelbrot::CreateScenario()
{
  srand((unsigned int)time(NULL));
}

void NBLExpApplicationMandelbrot::InitImplementationNr(int nr)
{
  // Reinitialize the experiment.
  REGION_SIZE_X = pow(2, CONTENTION_NR+1);
  REGION_SIZE_Y = pow(2, CONTENTION_NR+1);
  NR_REGIONS_X  = RESOLUTION_X/REGION_SIZE_X;
  NR_REGIONS_Y  = RESOLUTION_Y/REGION_SIZE_Y;

  if (regionBufferGlobal) delete[] regionBufferGlobal;
  regionBufferGlobal = new ImageRegion[NR_REGIONS_Y*NR_REGIONS_X];

  resultType=1;
  nrAdders=0;
  nrSums=0;

  NBLExpProducerConsumerBase::InitImplementationNr(nr);
}

void NBLExpApplicationMandelbrot::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkTryRemove = 0;
  long countEmptyTryRemove = 0;

  handle_t* handle = ThreadInitImplementationNr(nr);
  {
    int myId=(int)threadID;
    int i=0;
    if(!(myId % 2)) {
      int regions = NR_REGIONS_X * NR_REGIONS_Y;
      int offset = myId/2;
      for(i = offset; i < regions; i += NR_CPUS/2) {
        ImageRegion *region = &regionBufferGlobal[i];
        Produce(region, i);
        Insert(handle,(void*)region, countInsert);
      }
      FAA32(&nrAdders,1);
    
    } else {
      ImageRegion *region;
      int checks = 0;
      while((region=
             (ImageRegion *)TryRemove(handle,
                                      countOkTryRemove,
                                      countEmptyTryRemove))!=NULL ||
            !((nrAdders == NR_CPUS/2) && (checks++ > 2))) {
        if(region) {
          Consume(region);
        }
      }
      FAA32(&nrSums,1);
    }
    while(nrAdders != ((NR_CPUS)/2) || nrSums != ((NR_CPUS+1)/2)) {
      Sleep(0);
    }
    if(threadID==0) {
      resultString=string("Mandelbrot");
    }

    // Update the global operation counters.
    SaveThreadStatistics(countInsert,
                         countOkTryRemove, countEmptyTryRemove);

    delete handle;
  }
}

string NBLExpApplicationMandelbrot::GetStatistics()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatistics()
     << " " << PATTERN_NR
     << " " << CONTENTION_NR;
#ifdef SAVEMANDELBROT
  SavePPM("tmp.ppm");
#endif
  return ss.str();
}

string NBLExpApplicationMandelbrot::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatisticsLegend()
     << " <communication pattern#>"
     << " <contention#>";
  return ss.str();
}

static int MAX_ITERATIONS = 255;

static int CalcZ(double xCoord, double yCoord)
{
  int iters = 0;
  double x = xCoord;
  double y = yCoord;
  while (iters < MAX_ITERATIONS) {
    double newx = x * x + xCoord - y * y;
    double newy = 2 * x * y + yCoord;
    x = newx;
    y = newy;
    if ((abs(x) > 2.0) || (abs(y) > 2.0)) {
      // This point is outside the set. Done.
      break;
    }
    iters++;
  }
  return iters;
}

static void GenerateGrid(ImageRegion *ir, double xStart, double yStart,
                         int width, int height,
                         int xIndex, int yIndex, double scale)
{
  ir->region.X = xIndex;
  ir->region.Y = yIndex;
  ir->region.Width = width;
  ir->region.Height = height;
  for(int dy=0;dy<REGION_SIZE_Y;dy++) {
    for(int dx=0;dx<REGION_SIZE_X;dx++) {
      Coord& c = ir->Get(dx, dy);
      c.X = xIndex + dx;
      c.Y = yIndex + dy;
      c.Z = CalcZ(xStart + (xIndex + dx)*scale, yStart + (yIndex + dy)*scale);
    }
  }
}

static int FromHSV(int h, int s, int v)
{
  double hue = (int)((double)h / 255.0 * 360.0) % 360;
  double sat = (double)s / 255.0;
  double val = (double)v / 255.0;

  double blue = 0;
  double green = 0;
  double red = 0;

  if (sat == 0) {
    red = green = blue = val;
  } else {
    // Divide the color wheel into 5 sectors
    double sectorPos = hue / 60;
    int sectorNumber = (int)floor(sectorPos);
    double fractionalSector = (sectorPos - sectorNumber);
    double p = val * (1 - sat);
    double q = val * (1 - (sat * fractionalSector));
    double t = (val * (1 - (sat * (1 - fractionalSector))));
    switch (sectorNumber) {
    case 0:
      red = val;
      green = t;
      blue = p;
      break;

    case 1:
      red = q;
      green = val;
      blue = p;
      break;

    case 2:
      red = p;
      green = val;
      blue = t;
      break;

    case 3:
      red = p;
      green = q;
      blue = val;
      break;

    case 4:
      red = t;
      green = p;
      blue = val;
      break;

    case 5:
      red = val;
      green = p;
      blue = q;
      break;
    }
  }
  return
    (((int)(red*255)&0xFF)<<16) |
    (((int)(green*255)&0xFF)<<8) |
    (((int)(blue*255)&0xFF)<<0);
}

static void Produce(ImageRegion *region, int index)
{
  int y = index / (NR_REGIONS_X);
  int x = index % (NR_REGIONS_X);
  GenerateGrid(region, X_MIN, Y_MIN, REGION_SIZE_X, REGION_SIZE_Y,
               x*REGION_SIZE_X, y*REGION_SIZE_Y, SCALE);
}

static void Consume(ImageRegion *region)
{
  int x = region->region.X;
  int y = region->region.Y;
  for(int dy=0; dy<REGION_SIZE_Y; dy++) {
    for(int dx=0; dx<REGION_SIZE_X; dx++) {
      image[y+dy][x+dx] = CalcColor(region->Get(dx, dy).Z);
    }
  }
}

static void SavePPM(string filename)
{
  std::ofstream file;
  file.open(filename.c_str(), std::ios::out);
  file << "P3" << std::endl;
  file << RESOLUTION_X << std::endl;
  file << RESOLUTION_Y << std::endl;
  file << 255 << std::endl;
  for(int y=0; y<RESOLUTION_Y; y++) {
    for(int x=0; x<RESOLUTION_X; x++) {
      int px = image[y][x];
      file << ((px & 0xFF0000) >> 16) << " "
           << ((px & 0xFF00)   >>  8) << " "
           << (px & 0xFF) << "   ";
    }
    file << std::endl;
  }
  file.close();
}
