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
// $Id: PhysicsListMessenger.hh,v 1.4 2009-12-29 19:23:26 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
//
/////////////////////////////////////////////////////////////////////////
//
// PhysicsListMessenger
//
// Created: 31.01.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of hadr01 (V.Ivanchenko)
//
////////////////////////////////////////////////////////////////////////
// 

#ifndef A2PhysicsListMessenger_h
#define A2PhysicsListMessenger_h 1

#include "globals.hh"

#include "G4UImessenger.hh"

class A2PhysicsList;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;
class G4UIdirectory;
//class PolHadronicProcess;
//class G4HadronicProcess;
//class PolNucleonRotate;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class A2PhysicsListMessenger: public G4UImessenger
{
public:
  
  A2PhysicsListMessenger(A2PhysicsList* p = 0);
  virtual ~A2PhysicsListMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  A2PhysicsList* pA2PhysicsList;
  G4String fRegion;
  
  G4UIcmdWithADoubleAndUnit* gammaCutCmd;
  G4UIcmdWithADoubleAndUnit* electCutCmd;
  G4UIcmdWithADoubleAndUnit* posCutCmd;
  G4UIcmdWithADoubleAndUnit* pCutCmd;
  G4UIcmdWithADoubleAndUnit* allCutCmd;
  G4UIcmdWithAString*        pListCmd;
  G4UIcmdWithoutParameter*   listCmd;  
  G4UIcmdWithADoubleAndUnit* fRegCutCmd;
  G4UIcmdWithAString*        fRegCmd;
  G4UIdirectory* fphysDir;
  // polarised nucleon stuff
  G4UIcmdWithADouble* fAyValue;
  G4UIcmdWithAString* fAyModel;
  G4UIcmdWithAString* fAyProc;
  G4UIcmdWithABool* fSaveScat;
  G4UIcmdWithABool* fDetFrame;
  G4UIdirectory* fPolDir;
  // PolHadronicProcess* fNproc;
  // PolNucleonRotate* fPolRot;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

