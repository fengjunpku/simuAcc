#ifndef ACCGUN_HH
#define ACCGUN_HH 1

#include <map>
#include <string>
#include <iostream>

#include <TVector3.h>
#include <TH1F.h>
#include <TFile.h>
#include <TF1.h>
#include <TRandom3.h>
#include <Math/SpecFuncMathMore.h>
#include <Math/SpecFuncMathCore.h>

#include "accDef.hh"
#include "JunErrors.hh"

using namespace std;
using namespace accDef;

struct JunPiece
{
  bool isBreak;
  double mass;// MeV
  TVector3 direction;
  double energy;
};

struct JunMode
{
  string name;
  int num;
  map<string,JunPiece> piece;
  double threshold;// MeV
};

/*
Example:
  accGun *gun = new accGun()
  gun->SetBeam()
  **Loop
    gun->BeamOn()
*/
class accGun
{
public:
  accGun();
  virtual ~accGun();
  void SetBeam(double mass,double energy,TVector3 dir = TVector3(0,0,1));
  void LoadCrossSection(string csfile);
  void BeamOn();
  double GetEx();
  TVector3 GetMom(string pname,int modeID = 0);
  double GetEne(string pname,int modeID = 0);
private:
  TH1F* hist_cs;
  double mass_a;//beam     a
  double mass_A;//target   A
  double mass_b;//outgoing b
  double mass_B;//recoil   B
  double Eb;    //beam energy
  double Ex;    //excited energy
  TVector3 beamDir;
  TRandom3 *ran;
  JunMode jmode[2];
  void Generate(JunMode &mode,double excitedEnergy);
  double GetThetaByCS();
  double GetExCompositEenergy(double theta);
  double DisOfPiece(int l_jun,int m_jun);
};

#endif