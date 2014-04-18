
#ifndef A2EventActionMessenger_h
#define A2EventActionMessenger_h 1

#include "globals.hh"

#include "G4UImessenger.hh"

class A2EventAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class A2EventActionMessenger: public G4UImessenger
{
  public:
    A2EventActionMessenger(A2EventAction*);
   ~A2EventActionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    A2EventAction*     feventAction;
    G4UIdirectory*        feventDir;   
    G4UIcmdWithAString*   fDrawCmd;
  G4UIcmdWithAString*   fOutFileCmd;
   G4UIcmdWithAString*   fHitDrawCmd;
    G4UIcmdWithAnInteger* fPrintCmd;    
   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
