// Pol a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolPhysicsListMessenger
// Control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
//  7/11/10 JRMA Add neutron Ay (scaled Dubna)
//  7/09/11 JRMA Add new polarised elastic class
// 29/01/12 JRMA Remove alusions to old elastic class
// 31/01/12 JRMA Turn on/off spin precession


#ifndef PolPhysicsListMessenger_h
#define PolPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class PolPhysicsList;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3Vector;
class G4UIdirectory;
class PolHadronicProcess;
class G4HadronicProcess;
class PolNucleonRotate;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PolPhysicsListMessenger: public G4UImessenger
{
public:
  
  PolPhysicsListMessenger(PolPhysicsList* );
  virtual ~PolPhysicsListMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  PolPhysicsList* fpPhysicsList;
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
  G4UIcmdWith3Vector* fSpinPrecN;  
  G4UIdirectory* fPolDir;
  PolHadronicProcess* fNproc;
  PolNucleonRotate* fPolRot;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

