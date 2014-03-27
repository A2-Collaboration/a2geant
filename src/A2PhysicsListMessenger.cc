//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: A2PhysicsListMessenger.cc,v 1.7 2010-01-13 15:53:44 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
//
/////////////////////////////////////////////////////////////////////////
//
// A2PhysicsListMessenger
//
// Created: 31.01.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of hadr01 (V.Ivanchenko)
//
////////////////////////////////////////////////////////////////////////
//
// 

#include "A2PhysicsListMessenger.hh"

#include "A2PhysicsList.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UImanager.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

A2PhysicsListMessenger::A2PhysicsListMessenger(A2PhysicsList* pPhys)
:pA2PhysicsList(pPhys)
{   
  fphysDir = new G4UIdirectory("/A2/physics/");
  fphysDir->SetGuidance("physics control");

  gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/CutGamma",this);  
  gammaCutCmd->SetGuidance("Set gamma cut.");
  gammaCutCmd->SetParameterName("Gcut",false);
  gammaCutCmd->SetUnitCategory("Length");
  gammaCutCmd->SetRange("Gcut>=0.0");
  gammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  electCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/CutEl",this);  
  electCutCmd->SetGuidance("Set electron cut.");
  electCutCmd->SetParameterName("Ecut",false);
  electCutCmd->SetUnitCategory("Length");
  electCutCmd->SetRange("Ecut>=0.0");
  electCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  posCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/CutPos",this);
  posCutCmd->SetGuidance("Set positron cut.");
  posCutCmd->SetParameterName("Pcut",false);
  posCutCmd->SetUnitCategory("Length");
  posCutCmd->SetRange("Pcut>=0.0");
  posCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  pCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/CutProt",this);
  pCutCmd->SetGuidance("Set proton cut.");
  pCutCmd->SetParameterName("ProtCut",false);
  pCutCmd->SetUnitCategory("Length");
  pCutCmd->SetRange("ProtCut>=0.0");
  pCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  allCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/CutsAll",this);
  allCutCmd->SetGuidance("Set cut for all.");
  allCutCmd->SetParameterName("cut",false);
  allCutCmd->SetUnitCategory("Length");
  allCutCmd->SetRange("cut>=0.0");
  allCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  pListCmd = new G4UIcmdWithAString("/A2/physics/Physics",this);
  pListCmd->SetGuidance("Add modula physics list.");
  pListCmd->SetParameterName("PList",false);
  pListCmd->AvailableForStates(G4State_PreInit);

  listCmd = new G4UIcmdWithoutParameter("/A2/physics/ListPhysics",this);
  listCmd->SetGuidance("Available Physics Lists");
  listCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fRegCutCmd = new G4UIcmdWithADoubleAndUnit("/A2/physics/RegionCut",this);
  fRegCutCmd->SetGuidance("Set cut for selected region. Need to call SetRegion first or defaults to world");
  fRegCutCmd->SetParameterName("cut",false);
  fRegCutCmd->SetUnitCategory("Length");
  fRegCutCmd->SetRange("cut>0.0");
  fRegCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fRegCmd = new G4UIcmdWithAString("/A2/physics/SetRegion",this);
  fRegCmd->SetGuidance("Select region to set cut for");
  fRegCmd->SetParameterName("Region",false);
  fRegCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

A2PhysicsListMessenger::~A2PhysicsListMessenger()
{
  delete gammaCutCmd;
  delete electCutCmd;
  delete posCutCmd;
  delete pCutCmd;
  delete allCutCmd;
  delete pListCmd;
  delete listCmd;
  delete fRegCutCmd;
  delete fRegCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void A2PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if( command == gammaCutCmd ) {
    if(pA2PhysicsList) {
      pA2PhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));
    } else {
      UI->ApplyCommand("/run/setCutForAGivenParticle gamma " + newValue);
    }

  } else if( command == electCutCmd ) {
    if(pA2PhysicsList) {
      pA2PhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));
    } else {
      UI->ApplyCommand("/run/setCutForAGivenParticle e- " + newValue);
    }

  } else if( command == posCutCmd ) {
    if(pA2PhysicsList) {
      pA2PhysicsList->SetCutForPositron(posCutCmd->GetNewDoubleValue(newValue));
    } else {
      UI->ApplyCommand("/run/setCutForAGivenParticle e+ " + newValue);
    }

  } else if( command == pCutCmd ) {
    if(pA2PhysicsList) {
      pA2PhysicsList->SetCutForProton(pCutCmd->GetNewDoubleValue(newValue));
    } else {
      UI->ApplyCommand("/run/setCutForAGivenParticle proton " + newValue);
    }

  } else if( command == allCutCmd ) {

    if(pA2PhysicsList) {
      G4double cut = allCutCmd->GetNewDoubleValue(newValue);
      pA2PhysicsList->SetCutForGamma(cut);
      pA2PhysicsList->SetCutForElectron(cut);
      pA2PhysicsList->SetCutForPositron(cut);
      pA2PhysicsList->SetCutForProton(cut);
    } else {
      UI->ApplyCommand("/run/setCut " + newValue);
    }

  } else if( command == pListCmd ) {
    if(pA2PhysicsList) {
      G4String name = newValue;
      if(name == "PHYSLIST") {
	char* path = getenv(name);
	if (path) name = G4String(path);
	else {
	  G4cout << "### A2PhysicsListMessenger WARNING: "
		 << " environment variable PHYSLIST is not defined"
		 << G4endl;
	  return; 
	}
      }
      pA2PhysicsList->AddPhysicsList(name);
    } else {
      G4cout << "### A2PhysicsListMessenger WARNING: "
	     << " /testhadr/Physics UI command is not available "
	     << "for reference Physics List" << G4endl;
    }

  } else if( command == listCmd ) {
    if(pA2PhysicsList) {
      pA2PhysicsList->List();
    } else { 
      G4cout << "### A2PhysicsListMessenger WARNING: "
	     << " /testhadr/ListPhysics UI command is not available "
	     << "for reference Physics List" << G4endl;
    }
  }
  if( command == fRegCutCmd )
    {
      G4double cut = fRegCutCmd->GetNewDoubleValue(newValue);
      G4cout<<"Set cut of "<<cut/mm<<" for "<<fRegion<<G4endl;
      pA2PhysicsList->SetCutForRegion(fRegion,cut);
     }

  if( command == fRegCmd ) {
    fRegion = newValue;
    G4cout<<"Selected region "<<fRegion<<G4endl;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
