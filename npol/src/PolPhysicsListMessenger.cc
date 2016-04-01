// Pol a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class PolPhysicsListMessenger
// Online control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
//  7/11/10 JRMA Add neutron Ay (scaled Dubna)
//  7/09/11 JRMA Add new polarised elastic class
// 29/01/12 JRMA Remove alusions to old elastic class
// 31/01/12 JRMA Turn on/off spin precession

#include "PolPhysicsListMessenger.hh"

#include "PolPhysicsList.hh"
#include "PolHadronElasticPhysics.hh"
#include "PolHadronInelasticPhysics.hh"
#include "PolHadronicProcess.hh"
#include "PolHadronElasticProcess.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PolPhysicsListMessenger::PolPhysicsListMessenger(PolPhysicsList* pPhys)
:fpPhysicsList(pPhys)
{   
  fphysDir = new G4UIdirectory("/Pol/physics/");
  fphysDir->SetGuidance("physics control");
  
  fgammaCutCmd = new G4UIcmdWithADoubleAndUnit("/Pol/physics/CutGamma",this);  
  fgammaCutCmd->SetGuidance("Set gamma cut.");
  fgammaCutCmd->SetParameterName("Gcut",false);
  fgammaCutCmd->SetUnitCategory("Length");
  fgammaCutCmd->SetRange("Gcut>0.0");
  fgammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  felectCutCmd = new G4UIcmdWithADoubleAndUnit("/Pol/physics/CutEl",this);  
  felectCutCmd->SetGuidance("Set electron cut.");
  felectCutCmd->SetParameterName("Ecut",false);
  felectCutCmd->SetUnitCategory("Length");
  felectCutCmd->SetRange("Ecut>0.0");
  felectCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  fposCutCmd = new G4UIcmdWithADoubleAndUnit("/Pol/physics/CutPos",this);
  fposCutCmd->SetGuidance("Set positron cut.");
  fposCutCmd->SetParameterName("Pcut",false);
  fposCutCmd->SetUnitCategory("Length");
  fposCutCmd->SetRange("Pcut>0.0");
  fposCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fallCutCmd = new G4UIcmdWithADoubleAndUnit("/Pol/physics/CutsAll",this);
  fallCutCmd->SetGuidance("Set cut for all.");
  fallCutCmd->SetParameterName("cut",false);
  fallCutCmd->SetUnitCategory("Length");
  fallCutCmd->SetRange("cut>0.0");
  fallCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fpListCmd = new G4UIcmdWithAString("/Pol/physics/Physics",this);
  fpListCmd->SetGuidance("Add modula physics list.");
  fpListCmd->SetParameterName("PList",false);
  fpListCmd->AvailableForStates(G4State_PreInit);

  flistCmd = new G4UIcmdWithoutParameter("/Pol/physics/ListPhysics",this);
  flistCmd->SetGuidance("Available Physics Lists");
  flistCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPolDir = new G4UIdirectory("/Pol/pol/");
  fPolDir->SetGuidance("Polarised nucleon scattering control");
  
  fAyValue = new G4UIcmdWithADouble("/Pol/pol/Ay-Value",this);  
  fAyValue->SetGuidance("Set constant default value of analysing power");
  fAyValue->SetParameterName("AyVal",false);
  fAyValue->SetRange("-1.0<=AyVal && AyVal<=1.0");
  fAyValue->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fAyModel = new G4UIcmdWithAString("/Pol/pol/Ay-Model",this);
  fAyModel->SetGuidance("Choose parametrisation of Ay");
  fAyModel->SetParameterName("AyModel",false);
  fAyModel->AvailableForStates(G4State_PreInit,G4State_Idle);
  //
  fAyProc = new G4UIcmdWithAString("/Pol/pol/Ay-Process",this);
  fAyProc->SetGuidance("Choose hadronic process for following Ay commands");
  fAyProc->SetParameterName("AyProc",false);
  fAyProc->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSpinPrecN = new G4UIcmdWith3Vector("/Pol/pol/N-spin-precess",this);
  fSpinPrecN->AvailableForStates(G4State_PreInit,G4State_Idle);
  fSpinPrecN->SetGuidance("Enable nucleon spin precession in a magnetic field");
  fSpinPrecN->SetGuidance("Set proton and neutron anomalous magnetic moment");
  fSpinPrecN->SetParameterName("muP","muN","spare",true,true); 
  //  fSpinPrecN->SetRange("muP != 0 || muN != 0 || spare != 0");

  fNproc = NULL;
  fPolRot = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PolPhysicsListMessenger::~PolPhysicsListMessenger()
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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PolPhysicsListMessenger::SetNewValue(G4UIcommand* command,
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
        G4cout << "### PolPhysicsListMessenger WARNING: "
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
    if(name == "p-elastic"){
      fPolRot = fpPhysicsList->GetPolHadronElasticPhysics()
	->GetpElPolProc()->GetPolRot();
      fPolRot->SetN(Epp);
    }
    else if(name == "n-elastic"){
      fPolRot = fpPhysicsList->GetPolHadronElasticPhysics()
	->GetnElPolProc()->GetPolRot();
      fPolRot->SetN(Enp);
    }
    else if(name == "p-inelastic"){
      fPolRot = fpPhysicsList->GetPolHadronInelasticPhysics()
	->GettheProtonInelastic()->GetPolRot();
      fPolRot->SetN(EpC);
    }
    else if(name == "n-inelastic"){
      fPolRot = fpPhysicsList->GetPolHadronInelasticPhysics()
	->GettheNeutronInelastic()->GetPolRot();
      fPolRot->SetN(EnC);
    }
    else{
      G4cout << " Permitted hadronic processes:" <<G4endl;
      G4cout << " p-elastic, n-elastic, p-inelastic, n-inelastic" <<G4endl;
    }
  }
  // Default analysing-power value
  else if( command == fAyValue ){
    if( !fPolRot )
      G4cout << " Hadronic process not set.../Pol/pol/Ay-Process" <<G4endl;
    else fPolRot->SetAy(fAyValue->GetNewDoubleValue(newValue));
  }
  // Analysing power model
  else if( command == fAyModel ) {
    G4String name = newValue;
    G4int iname;
    if(name == "Null")
      iname = EAyNull;
    else if(name == "Ladygin")
      iname = EAyLadygin;
    else if(name == "Spinka")
      iname = EAySpinka;
    else if(name == "McNaughton")
      iname = EAyMcNaughton;
    else if(name == "Glister")
      iname = EAyGlister;
    else if(name == "Azhgirey")
      iname = EAyAzhgirey;
    else if(name == "Sikora")
      iname = EAySikora;
    else{
      iname = EAyNull;
      G4cout << " Permitted Ay parametrisations:" <<G4endl;
      G4cout << " Null:       Ay assumed constant" <<G4endl;
      G4cout << " Ladygin:    Ladygin parametrisation free n-p, p-p" <<G4endl;
      G4cout << " Spinka:     Spinka parametrisation free p-p" <<G4endl;
      G4cout << " McNaughton: McNaughton parametrisation" <<G4endl;
      G4cout << " Glister:    Hall-A parametrisation" <<G4endl;
      G4cout << " Azhgirey:   High momentum proton parametrisiation" <<G4endl;
    }
    G4cout<<" Apower parameterisation setting "<<name<<" "<<iname<<G4endl;
    if( !fPolRot )
      G4cout << " Hadronic process not set.../Pol/pol/Ay-Process" <<G4endl;
    else fPolRot->SetAyParm(iname);
  }
  else if( command == fSpinPrecN ) {
    G4ThreeVector mu = fSpinPrecN->GetNew3VectorValue(newValue);
    fpPhysicsList->SetSpinPrecN(mu.x(),mu.y());
  }    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
