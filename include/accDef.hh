#ifndef ACCDEF_HH
#define ACCDEF_HH 1

#include <TROOT.h>
#include <TMath.h>
#include <map>
#include <string>

using namespace std;

namespace accDef
{
  static const double deg = TMath::DegToRad();
  static const double Mass_C13 = 12109.5;//*MeV
  static const double Mass_Be9 = 8392.75;//*MeV
  static const double Mass_He4 = 3727.38;//*MeV
  //
  static const float teW1w = 3.125;
  static const float teB7w = 2.0;
  //global var should be declared adn initialized
  static map<string,float> teDis = {{"l0",160},{"r0",160},{"l1",140},{"r1",140}};//should be static but not const
  static map<string,float> teAng = {{"l0",24},{"r0",-24},{"l1",47},{"r1",-47}};
  //code for tele
  static map<string,int> teBase = {{"l1",1000},{"l0",100},{"r0",10},{"r1",1}};
}
#endif
