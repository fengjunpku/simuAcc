#include "accTel.hh"

accTel::accTel()
{
  addDataFile("He4_in_Si.txt", "He4InSi");
  addDataFile("Be9_in_Si.txt", "Be9InSi");
  addDataFile("He4_in_Al.txt", "He4InAl");
  addDataFile("Be9_in_Al.txt", "Be9InAl");
  addDataFile("He4_in_Be9.txt","He4InBe");
  addDataFile("Be9_in_Be9.txt","Be9InBe");
}

accTel::~accTel()
{

}

void accTel::addTele(string name,double distance,double angle,double width,double height,double thickness)
{
  JunSimuTele tele;
  tele.distance  = distance;
  tele.theta     = angle;
  tele.width     = width;
  tele.height    = height;
  tele.thickness = thickness;
  mTele[name]    = tele;
}

void accTel::addDataFile(string filename,string discription)
{
  ifstream fin;
  string datadir = "/home/fengjun/Codes/ana_13C/rebuild_13C/rebuild_v0/srim_data/";
  fin.open(datadir+filename,ios::in);
  if(!fin) MiaoError("Can not Open : "+filename);
  string buff;
  TGraph *gr = new TGraph();
  TGraph *rg = new TGraph();
  int n = 0;
  while(getline(fin,buff))
  {
    stringstream buffstream;
    buffstream.str(buff);
    if(buff[0]=='#') continue;
    double energy,tmp1,tmp2,range;
    string unit1,unit2;
    buffstream>>energy>>unit1>>tmp1>>tmp2>>range>>unit2;
    if(unit1=="keV") energy=energy/1000.;//keV->MeV
    if(unit2=="A") range = range/10000.;//A->um
    if(unit2=="mm") range = range*1000.;//mm->um
    gr->SetPoint(n,energy,range);
    rg->SetPoint(n,range,energy);
    n++;
  }
  mRvE[discription] = new TSpline2(discription.c_str(),gr);
  mEvR[discription] = new TSpline2(discription.c_str(),rg);
}

int accTel::isDetected(TVector3 dir,string particle,double energy,int &ix,int &iy)
{
  int flag = 0;
  if(energy<=0) return flag;
  for(auto iter=mTele.begin();iter!=mTele.end();iter++)
  {
    string nam = iter->first;
    double dis = iter->second.distance;
    double ang = iter->second.theta;
    double min = minEnergy(dir,particle,nam);
    double xli = iter->second.width*0.5;
    double yli = iter->second.height*0.5;
    if(energy<min) continue;
    //if(energy<min) cout<<nam<<" "<<min<<" "<<energy<<endl;
    TVector3 vro = dir;
    vro.RotateY(-1*ang*deg);
    double xx = dis*vro.X()/vro.Z();
    double yy = dis*vro.Y()/vro.Z();
    if(TMath::Abs(xx)>xli) continue;
    if(TMath::Abs(yy)>yli) continue;
    ix = int((xx+xli)/teW1w);
    iy = int((yy+yli)/teW1w);
    flag = flag+teBase[nam];
    //cout<<ang<<" "<<dir.Theta()/deg<<" "<<dir.Phi()/deg<<" "<<flag<<endl;
  }
  return flag;
}

double accTel::minEnergy(TVector3 dir,string particle,string telename)
{
  if(telename!="l0" && telename!="r0" && telename!="l1" && telename!="r1")
    MiaoError("telename should be l0/r0/l1/r1, not { "+telename+" } in accTel::minEnergy() ");
  if(particle!="He4" && particle!="Be9")
    MiaoError("particle should be He4/Be9, not { "+particle+" } in accTel::minEnergy ");
  TVector3 vd;
  vd.SetMagThetaPhi(1,mTele[telename].theta*deg,0);
  double costh = TMath::Cos(dir.Angle(vd));
  double minE = 0;
  //loss in Si for l0/r0
  if(telename=="l0" || telename=="r0")
  {
    string dsp1 = particle+"InSi";
    double eth1 = mTele[telename].thickness/costh;
    minE  = corEnergy(eth1,minE,dsp1);
  }
  //
  if(telename=="l1"||telename=="r1")
  {
    if(particle=="Be9") minE = 8;
  }
  //loss in Al dead layer
  string dsp2 = particle+"InAl";
  double eth2 = 1./costh;
  minE = corEnergy(eth2,minE,dsp2);
  //loss in target
  string dsp3 = particle+"InBe";
  double eht3 = 0.5/TMath::Cos(dir.Theta());
  minE = corEnergy(eht3,minE,dsp3);
  //
  return minE;
}

double accTel::corEnergy(double range,double energy,string discription)
{
  if(!mRvE.count(discription)||!mEvR.count(discription))
    MiaoError(" Undefined { "+discription+" } as index in accTel::corEnergy() ");
  double r0 = mRvE[discription]->Eval(energy);
  return mEvR[discription]->Eval(r0+range);
}
