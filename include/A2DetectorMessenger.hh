
#ifndef A2DetectorMessenger_h
#define A2DetectorMessenger_h 1

#include "globals.hh"

#include "G4UImessenger.hh"

class A2DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;

class A2DetectorMessenger: public G4UImessenger
{
  public:
    A2DetectorMessenger(A2DetectorConstruction* );
   ~A2DetectorMessenger();

    void SetNewValue(G4UIcommand*, G4String);

  private:
    A2DetectorConstruction* fA2Detector;

    G4UIdirectory*             fA2Dir;
    G4UIdirectory*             fdetDir;
    G4UIcmdWithAnInteger*      fUseCBCmd;
    G4UIcmdWithAnInteger*      fUseTAPSCmd;
    G4UIcmdWithAnInteger*      fUsePIDCmd;
    G4UIcmdWithAnInteger*      fUseMWPCCmd;
    G4UIcmdWithAnInteger*      fUsePolCmd;
    G4UIcmdWithAnInteger*      fUseTOFCmd;
    G4UIcmdWithAnInteger*      fUseCherenkovCmd;
    G4UIcmdWithAString*      fTOFFileCmd;
    G4UIcmdWithAString*      fUseTargetCmd;
    G4UIcmdWithAnInteger*      fUseCryoTgtCmd;
    G4UIcmdWithAString*      fTargetMatCmd;
    G4UIcmdWithAString*      fTargetMagneticCoilsCmd;
    G4UIcmdWithADoubleAndUnit* fTargetLengthCmd;
    G4UIcmdWithAString*      fTargetMagneticFieldCmd;
    G4UIcmdWith3VectorAndUnit* fHemiGapCmd;
    G4UIcmdWithAString*      fTAPSFileCmd;
    G4UIcmdWithAnInteger*      fTAPSNCmd;
    G4UIcmdWithAnInteger*      fTAPSPbCmd;
    G4UIcmdWithADoubleAndUnit* fTAPSZCmd;
    G4UIcmdWithoutParameter*   fUpdateCmd;
    G4UIcmdWithADoubleAndUnit* fPIDZCmd;
    G4UIcmdWithADoubleAndUnit* fMWPCZCmd;
    G4UIcmdWithADoubleAndUnit* fPolZCmd;
    G4UIcmdWithADoubleAndUnit* fTargetZCmd;
 };

#endif

