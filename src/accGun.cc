#include "accGun.hh"

accGun::accGun()
{
  hist_cs = NULL;
  mass_A = Mass_Be9;//target
  ran = new TRandom3(0);
  //
  jmode[0].name = "13c9be4he";
  jmode[0].threshold = 10.65;
  jmode[0].num = 4;
  jmode[0].piece["outgo"]  = {0,Mass_C13,TVector3(0,0,-1),0};
  jmode[0].piece["recoil"] = {0,Mass_Be9,TVector3(0,0,-1),0};
  jmode[0].piece["be9b"] = {1,Mass_Be9,TVector3(0,0,-1),0};
  jmode[0].piece["he4b"] = {1,Mass_He4,TVector3(0,0,-1),0};
}

accGun::~accGun()
{
  delete ran;
}

void accGun::SetBeam(double mass,double energy,TVector3 dir)
{
  mass_a = mass;//beam
  Eb = energy;
  beamDir = dir;
}

void accGun::BeamOn()
{
  double exe = ran->Uniform(11,20);
  Generate(jmode[0],exe);
}

void accGun::Generate(JunMode &mode,double excitedEnergy)
{
  mass_b = mode.piece["outgo"].mass;
  mass_B = mode.piece["recoil"].mass;
  //
  Ex = excitedEnergy;
  double th = mode.threshold;
  //the beam
  TVector3 pin = beamDir;
  pin.SetMag(TMath::Sqrt((Eb+2*mass_a)*Eb));
  //the excited composit one
  double theta0 = GetThetaByCS();//exC13
  double phi0   = ran->Uniform(0,2*TMath::Pi());
  double ek0    = GetExCompositEenergy(theta0);
  TVector3 p0;//mometum of 13C
  double p0mag = TMath::Sqrt((ek0+2*mass_b)*ek0);
  p0.SetMagThetaPhi(p0mag,theta0,phi0);
  mode.piece["outgo"].direction = p0.Unit();
  mode.piece["outgo"].energy = p0mag;
  //the recoil one
  TVector3 Pr = pin-p0;
  double Er = TMath::Sqrt(Pr*Pr+mass_B*mass_B)-mass_B;
  mode.piece["recoil"].direction = Pr.Unit();
  mode.piece["recoil"].energy = Er;
  //
  if(mode.name == "13c9be4he")
  {
    double ml = mode.piece["he4b"].mass;
    double mh = mode.piece["be9b"].mass;
    double pcms = TMath::Sqrt(2*(Ex-th)*ml*mh/(ml+mh));//相对论的解太复杂了
    double thetacms = TMath::ACos(DisOfPiece(0,0));//!!!
    double phicms = ran->Uniform(0,2*TMath::Pi());
    TVector3 vpcms;//vector of mometum in cms
    vpcms.SetMagThetaPhi(pcms,thetacms,phicms);
    TVector3 vl = 1./mass_b*p0+1./ml*vpcms;
    TVector3 vh = 1./mass_b*p0-1./mh*vpcms;
    double El = 0.5*ml*vl*vl;
    double Eh = 0.5*mh*vh*vh;
    mode.piece["he4b"].direction = vl;
    mode.piece["be9b"].direction = vh;
    mode.piece["he4b"].energy = El;
    mode.piece["be9b"].energy = Eh;
  }
}

double cExp(double *x,double *p)
{
  return TMath::Exp(-1.*x[0]/p[0]);
}
double cSin4(double *x,double *p)
{
  return p[0]*TMath::Power(TMath::Sin(0.5*x[0]*deg),-4);
}
double legendre2(double *x,double *p)
{
  return TMath::Power(ROOT::Math::assoc_legendre(p[0],p[1],x[0]),2);
}

double accGun::DisOfPiece(int l_jun,int m_jun)
{
  TF1 flen2("len",legendre2,-1,1,2);
  flen2.SetParameters(l_jun,m_jun);
  return flen2.GetRandom();
}

void accGun::LoadCrossSection(string csfile)
{
  TFile *in_file = TFile::Open(csfile.c_str());
  hist_cs = (TH1F*)in_file->GetObjectChecked("hcs","TH1F");
}

double accGun::GetThetaByCS()//unit:rad
{
  //TF1 cs2("cExp",cExp,0.,180,1);
  //cs2.SetParameter(0,10.);//after the angle(degree) cs to 1/e
  TF1 cs2("cSin4",cSin4,5,175,1);
  cs2.SetParameter(0,1.);
  double th_c = cs2.GetRandom()*deg;//theta in c.m.s
  if(hist_cs) th_c = hist_cs->GetRandom()*deg;
  double cosE = Eb*mass_A/(mass_A+mass_a);
  double gamma = mass_a/mass_A*TMath::Sqrt(cosE/(cosE-Ex));
  double maxTh_c = gamma>1.?TMath::ACos(-1./gamma):TMath::Pi();
  double termUp = gamma + TMath::Cos(th_c);
  double termDw = TMath::Sqrt(1+2*gamma*termUp-gamma*gamma);
  double iflag = th_c>maxTh_c?-1:1;
  //cout<<iflag*TMath::ACos(termUp/termDw)/deg<<endl;
  return iflag*TMath::ACos(termUp/termDw);
}

double accGun::GetExCompositEenergy(double theta)
{
  double Qvalue = 0.-Ex;
  double iflag = (theta>0)?1.:-1.;
  double part1 = TMath::Sqrt(mass_a*mass_b*Eb)*TMath::Cos(theta)/(mass_B+mass_b);
  double part2 = Eb*(mass_B-mass_a)/(mass_B+mass_b)+part1*part1;//TMath::Power(part1,2);
  double part3 = mass_B*Qvalue/(mass_B+mass_b);
  //if(iflag*part2+part3<0) {cout<<"Warn! : "<<scatteredTheta/deg<<" "<<exEnergy<<" "<<part1<<" "<<part2<<endl;exit(0);}
  return TMath::Power((part1+iflag*TMath::Sqrt(part2+part3)),2);
}

double accGun::GetEx()
{
  return Ex;
}

TVector3 accGun::GetMom(string pname,int modeID)
{
  if(modeID!=0&&modeID!=1)
    MiaoError(" Miao ! Error : modeID should be 0/1 in accGun::Getmom");
  if(!jmode[modeID].piece.count(pname))
    MiaoError(" Miao ! Error : Invalid Key of piece name {"+pname+"} in accGun::GetMon() ! ");
  return jmode[modeID].piece[pname].direction;
}

double accGun::GetEne(string pname,int modeID)
{
  if(modeID!=0&&modeID!=1)
    MiaoError(" Miao ! Error : modeID should be 0/1 in accGun::Getmom");
  if(!jmode[modeID].piece.count(pname))
    MiaoError(" Miao ! Error : Invalid Key of piece name {"+pname+"} in accGun::GetMon() ! ");
  return jmode[modeID].piece[pname].energy;
}