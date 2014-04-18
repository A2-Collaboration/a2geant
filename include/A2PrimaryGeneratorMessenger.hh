
#ifndef A2PrimaryGeneratorMessenger_h
#define A2PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"


class A2PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;




class A2PrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    A2PrimaryGeneratorMessenger(A2PrimaryGeneratorAction*);
   ~A2PrimaryGeneratorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    A2PrimaryGeneratorAction* A2Action;
    G4UIdirectory*               gunDir;

 
  G4UIcmdWithAString* SetInputCmd;
  G4UIcmdWithAString* SetInput2Cmd;
  G4UIcmdWithAnInteger* SetNTrackCmd;
  G4UIcmdWithAnInteger* SetTrackCmd;
  G4UIcmdWithAnInteger* SetModeCmd;
  G4UIcmdWithAnInteger* SetSeedCmd;
  G4UIcmdWithADoubleAndUnit* SetTminCmd;
  G4UIcmdWithADoubleAndUnit* SetTmaxCmd;
  G4UIcmdWithADoubleAndUnit* SetThetaminCmd;
  G4UIcmdWithADoubleAndUnit* SetThetamaxCmd;
  G4UIcmdWithADoubleAndUnit* SetBeamXSigmaCmd;
  G4UIcmdWithADoubleAndUnit* SetBeamYSigmaCmd;
  G4UIcmdWithADoubleAndUnit* SetTargetZ0Cmd;
  G4UIcmdWithADoubleAndUnit* SetTargetThickCmd;
  G4UIcmdWithADoubleAndUnit* SetTargetRadiusCmd;
  G4UIcmdWithADoubleAndUnit* SplitThetaCmd;
};



#endif

