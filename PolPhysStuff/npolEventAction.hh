


#ifndef npolEventAction_h
#define npolEventAction_h 1

#include "A2EventAction.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "npolPhysicsList.hh"
#include "globals.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class npolEventAction : public G4UserEventAction
{
  public:
  npolEventAction();
  npolEventAction(G4String filename);
  virtual ~npolEventAction();

  public:
    virtual void   BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    
  void SetIncMom(G4ThreeVector mom){SetMomentum(fIncMom,mom);}
  void SetIncMass(G4double mass){fIncMom->SetVectM(fIncMom->Vect(),mass);}
  void SetSPMom(G4ThreeVector mom){SetMomentum(fSPMom,mom);}
  void SetSNMom(G4ThreeVector mom){SetMomentum(fSNMom,mom);}
  void SetMomentum(TLorentzVector *mom4,G4ThreeVector mom3){
    mom4->SetXYZM(mom3.x()/MeV,mom3.y()/MeV,mom3.z()/MeV,mom4->M());
  }
  void SetPhysicsList(npolPhysicsList* npl){fPL=npl;}
  private:
  //G4int                       germCollID;                

  G4int totEvents; //total number of events processed

  //ROOT STUFF
  TFile* fnpolFile;
  TTree* fnpolTree;
  TString fFileName;

  TLorentzVector *fIncMom; //incident momentum
  TLorentzVector *fSPMom;  //scattered proton
  TLorentzVector *fSNMom;  //scatterd neutron
  TLorentzVector *fRealScat;  //scatterd neutron

  Float_t fedep;
  Float_t ftime;
  G4int npolCollID;

  npolPhysicsList* fPL;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
