#ifndef ACCTEL_HH
#define ACCTEL_HH 1

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TMVA/TSpline2.h>
#include <TMath.h>
#include <TVector3.h>
#include <TGraph.h>

#include "accDef.hh"
#include "JunErrors.hh"

using namespace std;
using namespace TMVA;
using namespace accDef;

struct JunSimuTele
{
  double distance;
  double theta;
  double width;
  double height;
  double thickness;
};
/*
Example:
  accTel *tele = new accTel()
  tele->addTele()
  ***
  tele->isDetected()
*/
class accTel
{
public:
  accTel();
  virtual ~accTel();
  void addTele(string name,double distance,double angle,double width,double height,double thickness);
  double minEnergy(TVector3 dir,string particle,string telename);
  int   isDetected(TVector3 dir,string particle,double energy);

private:
  map<string,JunSimuTele> mTele;
  map<string,TSpline2*> mRvE;// range vs. energy
  map<string,TSpline2*> mEvR;// energy vs. range
  void addDataFile(string filename,string discription);
  double corEnergy(double range,double energy,string discription);
  
};

#endif