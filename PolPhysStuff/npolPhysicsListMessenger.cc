// npol a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class npolPhysicsListMessenger
// Online control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
//  7/11/10 JRMA Add neutron Ay (scaled Dubna)
//  7/09/11 JRMA Add new polarised elastic class

#include "npolPhysicsListMessenger.hh"

#include "npolPhysicsList.hh"
#include "PolHadronElasticPhysics.hh"
#include "PolHadronElasticPhysicsN.hh"
#include "PolHadronInelasticPhysics.hh"
#include "PolUHadronElasticProcess.hh"
#include "PolHadronicProcess.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolPhysicsListMessenger::npolPhysicsListMessenger(npolPhysicsList* pPhys)
:fpPhysicsList(pPhys)
{   
  fphysDir = new G4UIdirectory("/npol/physics/");
  fphysDir->SetGuidance("physics control");
  
  fgammaCutCmd = new G4UIcmdWithADoubleAndUnit("/npol/physics/CutGamma",this);  
  fgammaCutCmd->SetGuidance("Set gamma cut.");
  fgammaCutCmd->SetParameterName("Gcut",false);
  fgammaCutCmd->SetUnitCategory("Length");
  fgammaCutCmd->SetRange("Gcut>0.0");
  fgammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  felectCutCmd = new G4UIcmdWithADoubleAndUnit("/npol/physics/CutEl",this);  
  felectCutCmd->SetGuidance("Set electron cut.");
  felectCutCmd->SetParameterName("Ecut",false);
  felectCutCmd->SetUnitCategory("Length");
  felectCutCmd->SetRange("Ecut>0.0");
  felectCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  fposCutCmd = new G4UIcmdWithADoubleAndUnit("/npol/physics/CutPos",this);
  fposCutCmd->SetGuidance("Set positron cut.");
  fposCutCmd->SetParameterName("Pcut",false);
  fposCutCmd->SetUnitCategory("Length");
  fposCutCmd->SetRange("Pcut>0.0");
  fposCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fallCutCmd = new G4UIcmdWithADoubleAndUnit("/npol/physics/CutsAll",this);
  fallCutCmd->SetGuidance("Set cut for all.");
  fallCutCmd->SetParameterName("cut",false);
  fallCutCmd->SetUnitCategory("Length");
  fallCutCmd->SetRange("cut>0.0");
  fallCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fpListCmd = new G4UIcmdWithAString("/npol/physics/Physics",this);
  fpListCmd->SetGuidance("Add modula physics list.");
  fpListCmd->SetParameterName("PList",false);
  fpListCmd->AvailableForStates(G4State_PreInit);

  flistCmd = new G4UIcmdWithoutParameter("/npol/physics/ListPhysics",this);
  flistCmd->SetGuidance("Available Physics Lists");
  flistCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPolDir = new G4UIdirectory("/npol/pol/");
  fPolDir->SetGuidance("Polarised nucleon scattering control");
  
  fAyValue = new G4UIcmdWithADouble("/npol/pol/Ay-Value",this);  
  fAyValue->SetGuidance("Set constant default value of analysing power");
  fAyValue->SetParameterName("AyVal",false);
  fAyValue->SetRange("-1.0<=AyVal && AyVal<=1.0");
  fAyValue->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fAyModel = new G4UIcmdWithAString("/npol/pol/Ay-Model",this);
  fAyModel->SetGuidance("Choose parametrisation of Ay");
  fAyModel->SetParameterName("AyModel",false);
  fAyModel->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fAyProc = new G4UIcmdWithAString("/npol/pol/Ay-Process",this);
  fAyProc->SetGuidance("Choose hadronic process for following Ay commands");
  fAyProc->SetParameterName("AyProc",false);
  fAyProc->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fSaveScat = new G4UIcmdWithABool("/npol/pol/SaveScat",this);
  fSaveScat->SetGuidance("Save the scattering information for an event");
  fSaveScat->SetParameterName("SaveScat",false);
  fSaveScat->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fDetFrame = new G4UIcmdWithABool("/npol/pol/UseDetFrame",this);
  fDetFrame->SetGuidance("Make the polarisation defined in the lab frame");
  fDetFrame->SetParameterName("DetFrame",false);
  fDetFrame->AvailableForStates(G4State_PreInit,G4State_Idle);

  fNproc = NULL;
  fPolRot = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolPhysicsListMessenger::~npolPhysicsListMessenger()
{
  delete fgammaCutCmd;
  delete felectCutCmd;
  delete fposCutCmd;
  delete fallCutCmd;
  delete fpListCmd;
  delete flistCmd;
  delete fphysDir;
  delete fAyValue;
  delete fAyModel;
  delete fAyProc;
  delete fPolDir;
  delete fSaveScat;
  delete fDetFrame;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolPhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue)
{
  //  PolHadronicProcess* proc;
  if( command == fgammaCutCmd )
    fpPhysicsList->SetCutForGamma(fgammaCutCmd->GetNewDoubleValue(newValue));

  else if( command == felectCutCmd )
    fpPhysicsList->SetCutForElectron(felectCutCmd->GetNewDoubleValue(newValue));

  else if( command == fposCutCmd )
   fpPhysicsList->SetCutForPositron(fposCutCmd->GetNewDoubleValue(newValue));

  else if( command == fallCutCmd )
    {
      G4double cut = fallCutCmd->GetNewDoubleValue(newValue);
      fpPhysicsList->SetCutForGamma(cut);
      fpPhysicsList->SetCutForElectron(cut);
      fpPhysicsList->SetCutForPositron(cut);
    }
  // Add physics process
  else if( command == fpListCmd ) {
    G4String name = newValue;
    if(name == "PHYSLIST") {
      char* path = getenv(name);
      if (path) name = G4String(path);
      else {
        G4cout << "### npolPhysicsListMessenger WARNING: "
	       << " environment variable PHYSLIST is not defined"
	       << G4endl;
	return; 
      }
    }
    fpPhysicsList->AddPhysicsList(name);
  }
  // List physics processes
  else if( command == flistCmd )
    fpPhysicsList->List();
  // Set Hadronic process for any subsequent polarisation control cmds
  else if( command == fAyProc ){
    G4String name = newValue;
    fPolRot =fpPhysicsList->GetPolNucleonRotate(name);
    if(!fPolRot){
      G4cout << " Permitted hadronic processes:" <<G4endl;
      G4cout << " p-elastic, n-elastic, p-inelastic, n-inelastic" <<G4endl;
    }
    // if(name == "p-elastic")
    //   //      fPolRot = fpPhysicsList->GetPolHadronElasticPhysics()
    //   //	->GetpElPolProc()->GetPolRot();
    //   fPolRot =  fpPhysicsList->GetPolNucleonRotate();
    // else if(name == "n-elastic")
    //   //      fPolRot = fpPhysicsList->GetPolHadronElasticPhysics()
    //   //	->GetnElPolProc()->GetPolRot();
    //   fPolRot =  fpPhysicsList->GetPolNucleonRotate();
    // else if(name == "p-inelastic")
    //   fPolRot = fpPhysicsList->GetPolHadronInelasticPhysics()
    // 	->GettheProtonInelastic()->GetPolRot();
    // else if(name == "n-inelastic")
    //   fPolRot = fpPhysicsList->GetPolHadronInelasticPhysics()
    // 	->GettheNeutronInelastic()->GetPolRot();
    // else{
    //   G4cout << " Permitted hadronic processes:" <<G4endl;
    //   G4cout << " p-elastic, n-elastic, p-inelastic, n-inelastic" <<G4endl;
    // }
  }
  // Default analysing-power value
  else if( command == fAyValue ){
    if( !fPolRot )
      G4cout << " Hadronic process not set.../npol/pol/Ay-Process" <<G4endl;
    else fPolRot->SetAy(fAyValue->GetNewDoubleValue(newValue));
  }
  // Analysing power model
  else if( command == fAyModel ) {
    G4String name = newValue;
    G4int iname;
    if(name == "Null")
      iname = EAyNull;
    else if(name == "McNaughton")
      iname = EAyMcNaughton;
    else if(name == "JLab")
      iname = EAyJLab;
    else if(name == "Dubna")
      iname = EAyDubna;
    else if(name == "Dubna-Neutron")
      iname = EAyDubnaNeutron;
    else if(name == "Sikora")
      iname = EAySikora;
    else{
      iname = EAyNull;
      G4cout << " Permitted Ay parametrisations:" <<G4endl;
      G4cout << " Null:       Ay assumed constant" <<G4endl;
      G4cout << " McNaughton: McNaughton parametrisation" <<G4endl;
      G4cout << " JLab:       Hall-A parametrisation" <<G4endl;
      G4cout << " Dubna:      High momentum proton parametrisiation" <<G4endl;
      G4cout << " Dubna-Neutron:High momentum neutron = p*0.4" <<G4endl;
      G4cout << " Sikora:     Sikora parameterization" << G4endl;
    }
    if( !fPolRot )
      G4cout << " Hadronic process not set.../npol/pol/Ay-Process" <<G4endl;
    else fPolRot->SetAyParm(iname);
  }
  //save the first scatter angles for this event
  else if( command == fSaveScat ){
    if( !fPolRot )
      G4cout << " Hadronic process not set.../npol/pol/Ay-Process" <<G4endl;
    else fPolRot->SetSaveScat(fSaveScat->GetNewBoolValue(newValue));
    //fpPhysicsList->SetSaveScat(fSaveScat->GetNewBoolValue(newValue));
  }
  else if( command == fDetFrame ){
    G4String process[]={ "p-elastic", "n-elastic", "p-inelastic", "n-inelastic", "p-inelastic-qf", "n-inelastic-qf"};
    for(G4int i=0;i<6;i++){
      fPolRot =fpPhysicsList->GetPolNucleonRotate(process[i]);
      if(fPolRot)fPolRot->SetDetFrame(fSaveScat->GetNewBoolValue(newValue));
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
