//1-20  500w
//21-40 800w
//41-90 800w
{
  TChain *fchain = new TChain("tree","tree");
  char filename[30];
  for(int i=1;i<=40;i++)
  {
    sprintf(filename,"t_%04d.root",i);
    fchain->AddFile(filename);
  }
}
