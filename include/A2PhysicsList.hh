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
/// \file hadronic/Hadr01/include/A2PhysicsList.hh
/// \brief Definition of the A2PhysicsList class
//
//
// $Id: A2PhysicsList.hh 68803 2013-04-05 13:59:55Z gcosmo $
//
/////////////////////////////////////////////////////////////////////////
//
// A2PhysicsList
//
// Created: 31.04.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of Hadr01 (V.Ivanchenko)
//
////////////////////////////////////////////////////////////////////////
// 

#ifndef A2PhysicsList_h
#define A2PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4LorentzVector.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class A2PhysicsListMessenger;
//class PolHadronElasticPhysics;
//class PolHadronElasticPhysicsN;
//class PolHadronInelasticPhysics;
//class PolNucleonRotate;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class A2PhysicsList: public G4VModularPhysicsList
{
public:

  A2PhysicsList();
  virtual ~A2PhysicsList();

  virtual void ConstructParticle();
  virtual void ConstructProcess();    

  virtual void SetCuts();

  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);
  void SetCutForProton(G4double);
  void SetCutForRegion(G4String reg,G4double cut);
        
  void AddPhysicsList(const G4String& name);
  void List();
  
private:

  //void SetPolList(G4bool = false, G4bool = false);
  //void SetNoHadList();
  void SetBuilderList0(G4bool flagHP = false);
  void SetBuilderList1(G4bool flagHP = false);
  void SetBuilderList2();

  G4double fCutForGamma;
  G4double fCutForElectron;
  G4double fCutForPositron;
  G4double fCutForProton;

  G4VPhysicsConstructor*  fEmA2PhysicsList;
  G4VPhysicsConstructor*  fParticleList;
  std::vector<G4VPhysicsConstructor*>  fHadronPhys;
    
  A2PhysicsListMessenger* fMessenger;
  // PolHadronElasticPhysics*  fPolHadronElasticPhysics;
  // PolHadronElasticPhysicsN*  fPolHadronElasticPhysicsN;
  //PolHadronInelasticPhysics*  fPolHadronInelasticPhysics;
  //G4bool dump;
  //G4bool fIsNew;
  //G4bool fSaveScat;

  //public:
  //PolHadronElasticPhysics* GetPolHadronElasticPhysics()
  // { return  fPolHadronElasticPhysics; }
  // PolHadronElasticPhysicsN* GetPolHadronElasticPhysicsN()
  // { return  fPolHadronElasticPhysicsN; }
  // PolHadronInelasticPhysics* GetPolHadronInelasticPhysics()
  // { return fPolHadronInelasticPhysics; }
  // PolNucleonRotate* GetPolNucleonRotate(G4String type);

  // void SetSaveScat(G4bool save=true){fSaveScat=save;}
  // G4bool IsSaveScat(){return fSaveScat;} 
  // G4LorentzVector* GetScatVec();
  // void ResetEvent();

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

