// Class PolNPRotate
// Azimuthal modulation of polarised nucleon scattering
// 20/06/09 Adapt Derek Glazier's class DGPolHadronElasticPhysics
// 09/04/10 JRMA check updates to equivalent G4 class.
// 17/04/10 JRMA New class separated from hadronic processes
// 20/11/10 JRMA Save polarisation
// 15/11/15 DGLA class for elastic np scatter (inc. quasi free)
 
#include "PolNPRotate.hh"
#include "TROOT.h"
#include <typeinfo>
//----------------------------------------------------------------------------
PolNPRotate::PolNPRotate( G4double ay, G4int pay, G4int verbose ):PolNucleonRotate(ay,pay,verbose),fApFile(0),fAyPPh(0),fAyNPh(0),fAyh(0)
{
  G4cout<<" PolNPRotate opening analysing power file "<<G4endl;
  fApFile=new TFile("QuasiFreeApow.root");
  if(!fApFile->IsOpen()) {G4cerr<<"PolNPRotate::PolNPRotate() Need file QuasiFreeApow.root"<<G4endl;exit(0);}
  fAyPPh=fAyNPh=fAyh=NULL;
  fApFile->ls();
  gROOT->cd();//come out of file directory so can close file safely
  fAyPPh=dynamic_cast<TH2F*>(fApFile->Get("ppApow;1")->Clone());
  fAyNPh=dynamic_cast<TH2F*>(fApFile->Get("npApow;1")->Clone());
  fApFile->Close();
  fSaveScat=true;
  fScatFree=true;
  fExchange = false;
  fScatType=0;
}

//-----------------------------------------------------------------------------
PolNPRotate::~PolNPRotate(){
  if(fApFile) {
    //   fApFile->Close();
    delete fApFile;//M. Sikora, Oct 12, 2012
    delete fAyPPh;
    delete fAyNPh;
  }
}
