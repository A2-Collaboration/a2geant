#include "A2Target.hh"

A2Target::A2Target()
{

  fMotherLogic=NULL;
  fMyLogic=NULL;
  fMyPhysi=NULL;
  fMaterial=NULL;
  fIsInteractive=1;
  fNistManager=G4NistManager::Instance();
  fLength=0;
}
A2Target::~A2Target()
{
 
}
