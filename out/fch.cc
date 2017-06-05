{
  TChain *fchain = new TChain("tree","tree");
  char filename[30];
  for(int i=1;i<=40;i++)
  {
    sprintf(filename,"t_%04d.root",i);
    fchain->AddFile(filename);
  }
}
