

#ifndef npolPrimaryGeneratorAction_h
#define npolPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class npolDetectorConstruction;
class npolEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class npolPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    npolPrimaryGeneratorAction(npolDetectorConstruction*,npolEventAction*);    
   ~npolPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun*                particleGun;	  //pointer a to G4  class
    npolDetectorConstruction*    npolDetector;  //pointer to the geometry
  npolEventAction* npolEvAct;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


