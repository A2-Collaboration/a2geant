// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolNPRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 20/11/10 JRMA Save polarisation
 
#include "PolNPRotate.hh"
#include <typeinfo>
//----------------------------------------------------------------------------
PolNPRotate::PolNPRotate( G4double ay, G4int pay, G4int verbose ):PolNucleonRotate(ay,pay,verbose)
{
  fApFile=new TFile("QuasiFreeApow.root");
  if(!fApFile->IsOpen()) {G4cerr<<"PolNPRotate::PolNPRotate() Need file QuasiFreeApow.root"<<G4endl;exit(0);}
  fAyPPh=fAyNPh=fAyh=NULL;
  fAyPPh=(TH2F*)(fApFile->Get("ppApow")->Clone());
  fAyNPh=(TH2F*)(fApFile->Get("npApow")->Clone());
  //fApFile->Close();
  fSaveScat=true;
  fScatFree=true;
  fScatType=0;
 
}

//-----------------------------------------------------------------------------
PolNPRotate::~PolNPRotate(){
  if(fApFile) {
    fApFile->Close();
    //Try to fix memory leak
    delete fApFile;//M. Sikora, Oct 12, 2012
    delete fAyPPh;
    delete fAyNPh;
  }
}
