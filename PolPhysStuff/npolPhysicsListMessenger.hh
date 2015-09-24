// npol a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class npolPhysicsListMessenger
// Control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
//  7/11/10 JRMA Add neutron Ay (scaled Dubna)
//  7/09/11 JRMA Add new polarised elastic class

#ifndef npolPhysicsListMessenger_h
#define npolPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class npolPhysicsList;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;
class G4UIdirectory;
class PolHadronicProcess;
class G4HadronicProcess;
class PolNucleonRotate;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class npolPhysicsListMessenger: public G4UImessenger
{
public:
  
  npolPhysicsListMessenger(npolPhysicsList* );
  virtual ~npolPhysicsListMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  npolPhysicsList* fpPhysicsList;
  // physics cuts and listing
  G4UIcmdWithADoubleAndUnit* fgammaCutCmd;
  G4UIcmdWithADoubleAndUnit* felectCutCmd;
  G4UIcmdWithADoubleAndUnit* fposCutCmd;
  G4UIcmdWithADoubleAndUnit* fallCutCmd;
  G4UIcmdWithAString*        fpListCmd;
  G4UIcmdWithoutParameter*   flistCmd;  
  G4UIdirectory* fphysDir;
  // polarised nucleon stuff
  G4UIcmdWithADouble* fAyValue;
  G4UIcmdWithAString* fAyModel;
  G4UIcmdWithAString* fAyProc;
  G4UIcmdWithABool* fSaveScat;
  G4UIcmdWithABool* fDetFrame;
  G4UIdirectory* fPolDir;
  PolHadronicProcess* fNproc;
  PolNucleonRotate* fPolRot;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

