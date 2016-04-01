// Pol a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolPhysicsList
// Control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
// 15/01/11 JRMA Remove obsolete (as from G4.9.4) physics classes
//  7/09/11 JRMA Add new polarised elastic class
// 29/01/12 JRMA Remove fIsNew...use new elastic scattering class...G4.9.5
// 31/01/12 JRMA Add Enable spin precession switch

#ifndef PolPhysicsList_h
#define PolPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4LorentzVector.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PolPhysicsListMessenger;
class PolHadronElasticPhysics;
class PolHadronInelasticPhysics;
class PolNucleonRotate;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PolPhysicsList: public G4VModularPhysicsList
{
public:

  PolPhysicsList();
  virtual ~PolPhysicsList();

  void ConstructParticle();
    
  void SetCuts();
  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);
        
  void AddPhysicsList(const G4String& name);
  void ConstructProcess();
  void List();
  
private:
  void SetPolList(G4bool = false);
  void SetNoHadList();
  void SetBuilderList0(G4bool = false);
  void SetBuilderList1(G4bool = false);
  void SetBuilderList2(G4bool = false);
  void SetBuilderList3(G4bool = false);
  void SetBuilderList4(G4bool = false);
  void SetBuilderList5(G4bool = false);
  void SetBuilderList6(G4bool = false);

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;

  G4VPhysicsConstructor*  emPhysicsList;
  G4VPhysicsConstructor*  particleList;
  std::vector<G4VPhysicsConstructor*>  hadronPhys;
    
  PolPhysicsListMessenger* pMessenger;
  PolHadronElasticPhysics*  fPolHadronElasticPhysics;
  PolHadronInelasticPhysics*  fPolHadronInelasticPhysics;
  G4bool dump;
  G4bool fIsSpinPrecN;
  G4double fmuP;
  G4double fmuN;
  G4bool fIsNew;
  G4bool fSaveScat;
public:
  PolHadronElasticPhysics* GetPolHadronElasticPhysics()
  { return  fPolHadronElasticPhysics; }
  PolHadronInelasticPhysics* GetPolHadronInelasticPhysics()
  { return fPolHadronInelasticPhysics; }
  void SetSpinPrecN( G4double muP, G4double muN )
  { fIsSpinPrecN = true; fmuP = muP; fmuN = muN; }
  void SetSaveScat(G4bool save=true){fSaveScat=save;}
  G4bool IsSaveScat(){return fSaveScat;} 
  G4LorentzVector* GetScatVec();
  void ResetEvent();
  PolNucleonRotate* GetPolNucleonRotate(G4String type);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

