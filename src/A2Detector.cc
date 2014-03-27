#include "A2Detector.hh"

A2Detector::A2Detector()
{

  fMotherLogic=NULL;
  fMyLogic=NULL;
  fMyPhysi=NULL;

  fIsInteractive=1;
  fNistManager=G4NistManager::Instance();
}
A2Detector::~A2Detector()
{
 
}
