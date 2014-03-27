
#include "A2DetectorMessenger.hh"

#include "A2DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ThreeVector.hh"



A2DetectorMessenger::A2DetectorMessenger(
                                           A2DetectorConstruction* A2Det)
:fA2Detector(A2Det)
{ 
  fA2Dir = new G4UIdirectory("/A2/");
  fA2Dir->SetGuidance("UI commands of this example");
  
  fdetDir = new G4UIdirectory("/A2/det/");
  fdetDir->SetGuidance("detector control");

  fUseTAPSCmd = new G4UIcmdWithAnInteger("/A2/det/useTAPS",this);
  fUseTAPSCmd->SetGuidance("Construct TAPS");
  fUseTAPSCmd->SetParameterName("UseTAPS",false);
  //fUseTAPSCmd->SetRange("UseTAPS=0 don't build TAPS or UseTAPS!=0 build TAPS");
  fUseTAPSCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  fUseCBCmd = new G4UIcmdWithAnInteger("/A2/det/useCB",this);
  fUseCBCmd->SetGuidance("Construct CB");
  fUseCBCmd->SetParameterName("UseCB",false);
  //fUseCBCmd->SetRange("UseCB=0 don't build CB or UseCB!=0 build CB");
  fUseCBCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  fUsePIDCmd = new G4UIcmdWithAnInteger("/A2/det/usePID",this);
  fUsePIDCmd->SetGuidance("Construct PID");
  fUsePIDCmd->SetParameterName("UsePID",false);
  //fUsePIDCmd->SetRange("UsePID=0 don't build PID or UsePID!=0 build PID");
  fUsePIDCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fUseMWPCCmd = new G4UIcmdWithAnInteger("/A2/det/useMWPC",this);
  fUseMWPCCmd->SetGuidance("Construct MWPC");
  fUseMWPCCmd->SetParameterName("UseMWPC",false);
  fUseMWPCCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fUseTargetCmd=new G4UIcmdWithAString("/A2/det/useTarget",this);
  fUseTargetCmd->SetGuidance("Select the type of target");
  fUseTargetCmd->SetGuidance("Either Cryo or Solid");
  fUseTargetCmd->SetParameterName("UseTarget",false);
  fUseTargetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTargetMatCmd=new G4UIcmdWithAString("/A2/det/targetMaterial",this);
  fTargetMatCmd->SetGuidance("Select the target material");
  fTargetMatCmd->SetParameterName("TargetMaterial",false);
  fTargetMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  fTargetLengthCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTargetLength",this);
  fTargetLengthCmd->SetGuidance("Set target cell length");
  fTargetLengthCmd->SetParameterName("TargetLength",false);
  fTargetLengthCmd->SetUnitCategory("Length");
  fTargetLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  // Target magnetic coils type
  fTargetMagneticCoilsCmd = new G4UIcmdWithAString("/A2/det/targetMagneticCoils",this);
  fTargetMagneticCoilsCmd->SetGuidance("Select the target magnetic coils type");
  fTargetMagneticCoilsCmd->SetParameterName("TargetMagneticCoils",false);
  fTargetMagneticCoilsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  // Target magnetic field map
  fTargetMagneticFieldCmd = new G4UIcmdWithAString("/A2/det/setTargetMagneticFieldMap",this);
  fTargetMagneticFieldCmd->SetGuidance("Set path/name of the target magnetic field map file");
  fTargetMagneticFieldCmd->SetParameterName("TargetMagneticField",false);
  fTargetMagneticFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fHemiGapCmd = new G4UIcmdWith3VectorAndUnit("/A2/det/setHemiGap",this);
  fHemiGapCmd->SetGuidance("Set air gap between each hemisphere and equator");
  fHemiGapCmd->SetParameterName("HemiGapUp","HemiGapDown","HemiGapNA",false);
  fHemiGapCmd->SetUnitCategory("Length");
  fHemiGapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTAPSFileCmd = new G4UIcmdWithAString("/A2/det/setTAPSFile",this);
  fTAPSFileCmd->SetGuidance("Set the taps configuration file.");
  fTAPSFileCmd->SetParameterName("tapsfile",false);
  fTAPSFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTAPSZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTAPSZ",this);
  fTAPSZCmd->SetGuidance("Set distance of TAPS from centre of ball");
  fTAPSZCmd->SetParameterName("TAPSZ",false);
  fTAPSZCmd->SetUnitCategory("Length");
  fTAPSZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTAPSNCmd = new G4UIcmdWithAnInteger("/A2/det/setTAPSN",this);
  fTAPSNCmd->SetGuidance("Set number of active taps crystals");
  fTAPSNCmd->SetParameterName("NTAPS",false);
  fTAPSNCmd->SetRange("NTAPS>0 && NTAPS<=552");
  fTAPSNCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTAPSPbCmd = new G4UIcmdWithAnInteger("/A2/det/setTAPSPbWO4Rings",this);
  fTAPSPbCmd->SetGuidance("Set number of active BaF2 rings");
  fTAPSPbCmd->SetParameterName("NPbWO4",false);
  fTAPSPbCmd->SetRange("NPbWO4>=0 && NPbWO4<=15");
  fTAPSPbCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPIDZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setPIDZ",this);
  fPIDZCmd->SetGuidance("Set distance of PID from centre of ball");
  fPIDZCmd->SetParameterName("PIDZ",false);
  fPIDZCmd->SetUnitCategory("Length");
  fPIDZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fUpdateCmd = new G4UIcmdWithoutParameter("/A2/det/update",this);
  fUpdateCmd->SetGuidance("Update calorimeter geometry.");
  fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
  fUpdateCmd->AvailableForStates(G4State_Idle);

  fUseTOFCmd = new G4UIcmdWithAnInteger("/A2/det/useTOF",this);
  fUseTOFCmd->SetGuidance("Construct TOF");
  fUseTOFCmd->SetParameterName("UseTOF",false);
  fUseTOFCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTOFFileCmd = new G4UIcmdWithAString("/A2/det/setTOFFile",this);
  fTOFFileCmd->SetGuidance("Set the tof configuration file.");
  fTOFFileCmd->SetParameterName("toffile",false);
  fTOFFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fUseCherenkovCmd = new G4UIcmdWithAnInteger("/A2/det/useCherenkov",this);
  fUseCherenkovCmd->SetGuidance("Construct Cherenkov");
  fUseCherenkovCmd->SetParameterName("UseCherenkov",false);
  //fUseCherenkovCmd->SetRange("UseCherenkov=0 don't build Cherenkov or UseCherenkov!=0 build Cherenkov");
  fUseCherenkovCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 
}



A2DetectorMessenger::~A2DetectorMessenger()
{
  delete fUseTAPSCmd;
  delete fUseCBCmd;
  delete fUsePIDCmd;
  delete fUseTargetCmd;
  delete fTargetMatCmd;
  delete fTargetMagneticCoilsCmd;
  delete fUpdateCmd;
  delete fTAPSFileCmd;
  delete fTAPSZCmd;
  delete fTAPSNCmd;
  delete fTAPSPbCmd;
  delete fPIDZCmd;
  delete fUseMWPCCmd;
  delete fTOFFileCmd;
  delete fUseTOFCmd;
  delete fUseCherenkovCmd;
  delete fTargetLengthCmd;
  delete fTargetMagneticFieldCmd;
  delete fHemiGapCmd;

 }



void A2DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 


  if( command == fUseTAPSCmd )
    { fA2Detector->SetUseTAPS(fUseTAPSCmd->GetNewIntValue(newValue));}

  if( command == fUseCBCmd )
    { fA2Detector->SetUseCB(fUseCBCmd->GetNewIntValue(newValue));}

  if( command == fUsePIDCmd )
    { fA2Detector->SetUsePID(fUsePIDCmd->GetNewIntValue(newValue));}


  if( command == fUseMWPCCmd )
    { fA2Detector->SetUseMWPC(fUseMWPCCmd->GetNewIntValue(newValue));}

  if( command == fUseCherenkovCmd )
    { fA2Detector->SetUseCherenkov(fUseCherenkovCmd->GetNewIntValue(newValue));}
  
  if( command == fUpdateCmd )
    { fA2Detector->UpdateGeometry(); }
  
  if( command == fUseTargetCmd )
    { fA2Detector->SetUseTarget(newValue);}
  
  if( command == fTargetMatCmd )
    { fA2Detector->SetTargetMaterial(newValue);}
    
  if ( command == fTargetMagneticCoilsCmd )
    { fA2Detector->SetTargetMagneticCoils(newValue); }
  
  if( command == fTargetLengthCmd )
    { fA2Detector->SetTargetLength(fTargetLengthCmd->GetNewDoubleValue(newValue));}
    
  // Target magnetic field map
  if( command == fTargetMagneticFieldCmd )
    { fA2Detector->SetTargetMagneticFieldMap(newValue); }

   if( command == fHemiGapCmd )
    { fA2Detector->SetHemiGap(fHemiGapCmd->GetNew3VectorValue(newValue));}
 
  if( command == fTAPSFileCmd )
    { fA2Detector->SetTAPSFile(newValue);}
  
  if( command == fTAPSZCmd )
    { fA2Detector->SetTAPSZ(fTAPSZCmd->GetNewDoubleValue(newValue));}
  
  
  if( command == fTAPSNCmd )
    {fA2Detector->SetTAPSN(fTAPSNCmd->GetNewIntValue(newValue));}
  
  if( command == fTAPSPbCmd )
    {fA2Detector->SetTAPSPbWO4Rings(fTAPSPbCmd->GetNewIntValue(newValue));}
  
  if( command == fPIDZCmd )
    { fA2Detector->SetPIDZ(fPIDZCmd->GetNewDoubleValue(newValue));}
  
  if( command == fTOFFileCmd )
    { fA2Detector->SetTOFFile(newValue);}
  
  if( command == fUseTOFCmd )
    { fA2Detector->SetUseTOF(fUseTOFCmd->GetNewIntValue(newValue));}
  
 }


