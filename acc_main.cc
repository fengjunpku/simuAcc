// ************************************************************************* 
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#include <TVector3.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TTree.h>

#include "JunErrors.hh"
#include "accGun.hh"
#include "accTel.hh"
#include "accDef.hh"

using namespace std;
using namespace accDef;

//--
// l1   l0   r0  r1  : 1_1_1_1
//1000  100  10   1

int main(int argc,char** argv)
{
  TStopwatch watch;
  int num = 10;
  int run = 0;
  if(argc >= 2) num = atoi(argv[1]);
  if(argc == 3) run = atoi(argv[2]);
  //
  //time_t currt = time(NULL);
  //tm *p = localtime(&currt);
  //char st[30];
  //sprintf(st,"%d%02d%02d%02d%02d%d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
  //
  float ex;
  int cn;
  char fname[30];
  sprintf(fname,"out/t_%04d.root",run);
  TFile *f = new TFile(fname,"RECREATE");
  TTree *t = new TTree("tree","tree");
  t->Branch("ex",&ex,"ex/F");
  t->Branch("cn",&cn,"cn/I");
  int count = 0;
  //*****set the telescopes
  accTel *tele = new accTel();
  tele->addTele("l0",teDis["l0"],teAng["l0"],50,50,50);//string,mm,deg,mm,mm,um
  tele->addTele("r0",teDis["r0"],teAng["r0"],50,50,50);
  tele->addTele("l1",teDis["l1"],teAng["l1"],50,50,50);
  tele->addTele("r1",teDis["r1"],teAng["r1"],50,50,50);
  //
  accGun *gun = new accGun();
  gun->SetBeam(Mass_C13,65);
  for(int i=0;i<num;i++)
  {
    gun->BeamOn();
    TVector3 vp(0,0,1);
    double en(0);
    //be9r
    vp = gun->GetMom("recoil");
    en = gun->GetEne("recoil");
    int f0 = 4*tele->isDetected(vp,"Be9",en);
    //he4b
    vp = gun->GetMom("he4b");
    en = gun->GetEne("he4b");
    int f1 = 2*tele->isDetected(vp,"He4",en);
    //be9b
    vp = gun->GetMom("be9b");
    en = gun->GetEne("be9b");
    int f2 = 1*tele->isDetected(vp,"Be9",en);
    //
    if(f0*f1*f2) 
    {
      count++;
      //cout<<f0+f1+f2<<endl;
      cn = f0+f1+f2;
      ex = gun->GetEx();
      t->Fill();
    }
  }
  f->cd();
  t->Write();
  float acc3 = float(count)/float(num);
  //
  cout<<"   ======= End of simu "<<count<<"/"<<num<<" = "<<acc3<<" events ======="<<endl;
  printf("CPU_Time: %f s, RealTime: %f s\n",watch.CpuTime(),watch.RealTime());
}
