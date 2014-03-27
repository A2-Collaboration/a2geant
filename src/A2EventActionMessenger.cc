
#include "A2EventActionMessenger.hh"

#include "A2EventAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"



A2EventActionMessenger::A2EventActionMessenger(A2EventAction* EvAct)
:feventAction(EvAct)
{
  feventDir = new G4UIdirectory("/A2/event/");
  feventDir->SetGuidance("event control");
   
  fHitDrawCmd = new G4UIcmdWithAString("/A2/event/drawHitOpt",this);
  fHitDrawCmd->SetGuidance("Set the draw option for the hits");
  fHitDrawCmd->SetGuidance("  Choice : charge, edep, time, depth");
  fHitDrawCmd->SetParameterName("choice",true);
  fHitDrawCmd->SetDefaultValue("edep");
  fHitDrawCmd->SetCandidates("charge edep time depth");
  fHitDrawCmd->AvailableForStates(G4State_Idle);
  
   
  fDrawCmd = new G4UIcmdWithAString("/A2/event/drawTracks",this);
  fDrawCmd->SetGuidance("Draw the tracks in the event");
  fDrawCmd->SetGuidance("  Choice : none, charged(default),neutral, all");
  fDrawCmd->SetParameterName("choice",true);
  fDrawCmd->SetDefaultValue("all");
  fDrawCmd->SetCandidates("none charged neutral all");
  fDrawCmd->AvailableForStates(G4State_Idle);
  
    
  fOutFileCmd = new G4UIcmdWithAString("/A2/event/setOutputFile",this);
  fOutFileCmd->SetGuidance("set the full name and path of the output ROOT file");
  fOutFileCmd->SetParameterName("choice",true);
  fOutFileCmd->AvailableForStates(G4State_Idle);
  

  fPrintCmd = new G4UIcmdWithAnInteger("/A2/event/printModulo",this);
  fPrintCmd->SetGuidance("Print events modulo n");
  fPrintCmd->SetParameterName("EventNb",false);
  fPrintCmd->SetRange("EventNb>0");
  fPrintCmd->AvailableForStates(G4State_Idle);     


}



A2EventActionMessenger::~A2EventActionMessenger()
{
  delete fOutFileCmd;
  delete fDrawCmd;
  delete fPrintCmd;
  delete feventDir;   
}



void A2EventActionMessenger::SetNewValue(
                                        G4UIcommand* command,G4String newValue)
{ 
  if(command == fOutFileCmd)
    {feventAction->SetOutFileName(newValue.data());}
  
  if(command == fDrawCmd)
    {feventAction->SetDrawFlag(newValue);}
  
  if(command == fHitDrawCmd)
    {feventAction->SetHitDrawOpt(newValue);}
       
       
  if(command == fPrintCmd)
    {feventAction->SetPrintModulo(fPrintCmd->GetNewIntValue(newValue));}
}



