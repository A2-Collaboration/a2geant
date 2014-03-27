
#include "A2PrimaryGeneratorMessenger.hh"

#include "A2PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "Randomize.hh"


A2PrimaryGeneratorMessenger::A2PrimaryGeneratorMessenger(
                                          A2PrimaryGeneratorAction* A2Gun)
:A2Action(A2Gun)
{
  gunDir = new G4UIdirectory("/A2/generator/");
  gunDir->SetGuidance("PrimaryGenerator control");

  SetInputCmd = new G4UIcmdWithAString("/A2/generator/InputFile",this);
  SetInputCmd->SetGuidance("Set the file with the input ROOT ntuple");
  SetInputCmd->SetParameterName("inputfile",false);
  SetInputCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetInput2Cmd = new G4UIcmdWithAString("/A2/generator/PrepareInput",this);
  SetInput2Cmd->SetGuidance("Prepare the file with the input ROOT ntuple");
  SetInput2Cmd->SetParameterName("inputfile",false);
  SetInput2Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetNTrackCmd = new G4UIcmdWithAnInteger("/A2/generator/NToBeTracked",this);
  SetNTrackCmd->SetGuidance("Set the number of generated particles to be tracked");
  SetNTrackCmd->SetParameterName("Ntrack",false);
  SetNTrackCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetTrackCmd = new G4UIcmdWithAnInteger("/A2/generator/Track",this);
  SetTrackCmd->SetGuidance("Set the index of the particles to be tracked, this comes from the ntuple branch name.");
  SetTrackCmd->SetParameterName("Track",false);
  SetTrackCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetModeCmd = new G4UIcmdWithAnInteger("/A2/generator/Mode",this);
  SetModeCmd->SetGuidance("Set the mode of the generator, command line, phase space or ROOT");
  SetModeCmd->SetParameterName("Mode",false);
  SetModeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetSeedCmd = new G4UIcmdWithAnInteger("/A2/generator/Seed",this);
  SetSeedCmd->SetGuidance("Set the random seed  of the generator");
  SetSeedCmd->SetParameterName("Seed",false);
  SetSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetTminCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetTMin",this);
  SetTminCmd->SetGuidance("Set the minimum particle energy for the phase space generator");
  SetTminCmd->SetParameterName("Tmin",false);
  SetTminCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetTminCmd->SetUnitCategory("Energy");

  SetTmaxCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetTMax",this);
  SetTmaxCmd->SetGuidance("Set the maximum particle energy for the phase space generator");
  SetTmaxCmd->SetParameterName("Tmax",false);
  SetTmaxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetTmaxCmd->SetUnitCategory("Energy");

  SetThetaminCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetThetaMin",this);
  SetThetaminCmd->SetGuidance("Set the minimum particle theta for the phase space generator");
  SetThetaminCmd->SetParameterName("Tmin",false);
  SetThetaminCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetThetaminCmd->SetUnitCategory("Angle");

  SetThetamaxCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetThetaMax",this);
  SetThetamaxCmd->SetGuidance("Set the maximum particle theta for the phase space generator");
  SetThetamaxCmd->SetParameterName("Tmax",false);
  SetThetamaxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetThetamaxCmd->SetUnitCategory("Angle");

  SetBeamXSigmaCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetBeamXSigma",this);
  SetBeamXSigmaCmd->SetGuidance("Set the X width of the photon beam");
  SetBeamXSigmaCmd->SetParameterName("BeamXSigma",false);
  SetBeamXSigmaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetBeamXSigmaCmd->SetUnitCategory("Length");

  SetBeamYSigmaCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetBeamYSigma",this);
  SetBeamYSigmaCmd->SetGuidance("Set the Y width of the photon beam");
  SetBeamYSigmaCmd->SetParameterName("BeamYSigma",false);
  SetBeamYSigmaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetBeamYSigmaCmd->SetUnitCategory("Length");

  SetTargetZ0Cmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetTargetZ0",this);
  SetTargetZ0Cmd->SetGuidance("Set the target Z0 position");
  SetTargetZ0Cmd->SetParameterName("TargetZ0",false);
  SetTargetZ0Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetTargetZ0Cmd->SetUnitCategory("Length");

  SetTargetThickCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetTargetThick",this);
  SetTargetThickCmd->SetGuidance("Set the target thickness");
  SetTargetThickCmd->SetParameterName("TargetThick",false);
  SetTargetThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetTargetThickCmd->SetUnitCategory("Length");

  SetTargetRadiusCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetTargetRadius",this);
  SetTargetRadiusCmd->SetGuidance("Set the x-y radius of the target");
  SetTargetRadiusCmd->SetParameterName("TargetRadius",false);
  SetTargetRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetTargetRadiusCmd->SetUnitCategory("Length");

  SplitThetaCmd = new G4UIcmdWithADoubleAndUnit("/A2/generator/SetSplitTheta",this);
  SplitThetaCmd->SetGuidance("Set the splitoff angle in the overlap generator");
  SplitThetaCmd->SetParameterName("SplitTheta",false);
  SplitThetaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SplitThetaCmd->SetUnitCategory("Angle");

}



A2PrimaryGeneratorMessenger::~A2PrimaryGeneratorMessenger()
{
  delete gunDir;
  delete SetInputCmd;
  delete SetInput2Cmd;
  delete SetNTrackCmd;
  delete SetTrackCmd;
  delete SetTminCmd;
  delete SetTmaxCmd;
  delete SetThetaminCmd;
  delete SetThetamaxCmd;
  delete SetModeCmd;
  delete SetSeedCmd;
  delete SetBeamXSigmaCmd;
  delete SetBeamYSigmaCmd;
  delete SetTargetZ0Cmd;
  delete SetTargetThickCmd;
  delete SetTargetRadiusCmd;
  delete SplitThetaCmd;
 
}



void A2PrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue)
{ 
  if( command == SetInputCmd )
    { A2Action->SetInputFile(static_cast<TString>(newValue));}
  //  { A2Action->SetUpROOTInput(static_cast<TString>(newValue));}
  if( command == SetInput2Cmd )
    {  A2Action->SetInputFile(static_cast<TString>(newValue));A2Action->SetUpROOTInput();}//prepare input file in interactive mode
  if( command == SetNTrackCmd )
     { A2Action->SetNParticlesToBeTracked(SetNTrackCmd->GetNewIntValue(newValue));}
 
  if( command == SetTrackCmd )
     { A2Action->SetParticlesToBeTracked(SetTrackCmd->GetNewIntValue(newValue));}
  
  if( command == SetModeCmd )
     { A2Action->SetMode(SetModeCmd->GetNewIntValue(newValue));}

  if( command == SetSeedCmd )
    { CLHEP::HepRandom::setTheSeed(SetSeedCmd->GetNewIntValue(newValue));}

   if( command == SetTminCmd )
     { A2Action->SetTmin(SetTminCmd->GetNewDoubleValue(newValue));}
 
  if( command == SetTmaxCmd )
     { A2Action->SetTmax(SetTmaxCmd->GetNewDoubleValue(newValue));}
 
  if( command == SetThetaminCmd )
     { A2Action->SetThetamin(SetThetaminCmd->GetNewDoubleValue(newValue));}
  
  if( command == SetThetamaxCmd )
     { A2Action->SetThetamax(SetThetamaxCmd->GetNewDoubleValue(newValue));}
 
   if( command == SetBeamXSigmaCmd )
     { A2Action->SetBeamXSigma(SetBeamXSigmaCmd->GetNewDoubleValue(newValue));}
 
   if( command == SetBeamYSigmaCmd )
     { A2Action->SetBeamYSigma(SetBeamYSigmaCmd->GetNewDoubleValue(newValue));}
 
   if( command == SetTargetZ0Cmd )
     { A2Action->SetTargetZ0(SetTargetZ0Cmd->GetNewDoubleValue(newValue));}
 
   if( command == SetTargetThickCmd )
     { A2Action->SetTargetThick(SetTargetThickCmd->GetNewDoubleValue(newValue));}
 
   if( command == SetTargetRadiusCmd )
     { A2Action->SetTargetRadius(SetTargetRadiusCmd->GetNewDoubleValue(newValue));}
 
   if( command == SplitThetaCmd )
     { A2Action->SetSplitTheta(SplitThetaCmd->GetNewDoubleValue(newValue));}
 
}



