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
/// \file hadronic/Hadr01/src/A2PhysicsList.cc
/// \brief Implementation of the A2PhysicsList class
//
//
// $Id: A2PhysicsList.cc 70761 2013-06-05 12:30:51Z gcosmo $
//
/////////////////////////////////////////////////////////////////////////
//
// A2PhysicsList
//
// Created: 31.04.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of Hadr01 (V.Ivanchenko)
// 26.04.2007 Physics according to 8.3 Physics List (V.Ivanchenko)
// 16.10.2012 Renamed used classes (A.Ribon)
//
////////////////////////////////////////////////////////////////////////
// 

#include "A2PhysicsList.hh"
#include "A2PhysicsListMessenger.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4ChargeExchangePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4NeutronCrossSectionXS.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsFTF_BIC.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_FTFP_BERT.hh"
#include "G4HadronPhysicsQGS_BIC.hh"

#include "G4LossTableManager.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"

//#include "PolHadronElasticPhysics.hh"
//#include "PolHadronElasticPhysicsN.hh"
//#include "PolHadronInelasticPhysics.hh"
//#include "PolNucleonRotate.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

A2PhysicsList::A2PhysicsList() 
 : G4VModularPhysicsList(),
   fEmA2PhysicsList(0), fParticleList(0), fMessenger(0)
{
  G4LossTableManager::Instance();
  defaultCutValue = 0.7*mm;
  fCutForGamma     = defaultCutValue;
  fCutForElectron  = defaultCutValue;
  fCutForPositron  = defaultCutValue;
  fCutForProton    = defaultCutValue;
  verboseLevel    = 1;

  fMessenger = new A2PhysicsListMessenger(this);

  // Particles
  fParticleList = new G4DecayPhysics("decays");

  // EM physics
  fEmA2PhysicsList = new G4EmStandardPhysics();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

A2PhysicsList::~A2PhysicsList()
{
  delete fMessenger;
  delete fParticleList;
  delete fEmA2PhysicsList;
  for(size_t i=0; i<fHadronPhys.size(); i++) {
    delete fHadronPhys[i];
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

//PolNucleonRotate* A2PhysicsList::GetPolNucleonRotate(G4String type)
//{
//  if(type==G4String("n-elastic"))return  fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRot();
// else if(type==G4String("p-elastic"))return fPolHadronElasticPhysicsN->GetpElPolProc()->GetPolRot();
//  else if(type==G4String("p-inelastic"))return fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRot();
//  else if(type==G4String("n-inelastic"))return fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRot();
  //else if(type==G4String("p-inelastic-qf"))return ((PolHadronQuasiFreeProcess*)fPolHadronInelasticPhysics->GettheProtonInelastic())->GetPolRotNP();//MHS, Nov 14, 2014, "p-inelastic-qf" not listed in npolPhysicsListMessenger
  //else if(type==G4String("n-inelastic-qf"))return ((PolHadronQuasiFreeProcess*)fPolHadronInelasticPhysics->GettheNeutronInelastic())->GetPolRotNP();MHS, Nov 14, 2014,"n-inelastic-qf" not listed in npolPhysicsListMessenger
  //else {G4cerr<<" npolPhysicsList::GetPolNucleonRotate() No process "<<type<<G4endl;return NULL;}

//void A2PhysicsList::ResetEvent(){
//  if(!fSaveScat) return;
  // if(fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent())fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent()->ResetEvent();
  // if(fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent())fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent()->ResetEvent();
  // if(fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent())fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent()->ResetEvent();
    // G4String process[]={ "p-elastic", "n-elastic", "p-inelastic", "n-inelastic", "p-inelastic-qf", "n-inelastic-qf"};
//G4String process[]={ "p-elastic", "n-elastic", "p-inelastic", "n-inelastic"};//MHS Nov 14, 2014
//   for(G4int i=0;i<4;i++){
//    PolNucleonRotate* fPolRot = GetPolNucleonRotate(process[i]);
//     if (fPolRot) fPolRot->ResetEvent();
      //if (fPolRot) fPolRot->ToggleDidScat(false);
// //     }
   
// // }
// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
// //G4LorentzVector* A2PhysicsList::GetScatVec(){
//   //This can be quite ambigous, particularly if there are more than 1 original nucleons
//   //The highest energy incident nucleon that participates in a scatter is taken for
//   //the saved nucleon
// //  if(!fSaveScat) return NULL;
// //  PolNucleonRotate* polRotE=fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent();
// //  PolNucleonRotate* polRotIN=fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent();
// //  PolNucleonRotate* polRotIP=fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent();
// //G4double NE,INE,IPE;
// // NE=INE=IPE=0;
// // if(polRotE)NE=polRotE->GetScatE();
// // if(polRotIN)INE=polRotIN->GetScatE();
// // if(polRotIP)IPE=polRotIP->GetScatE();

// // if(NE&&NE>INE&&NE>IPE) return polRotE->GetScatVec();
// // else if(INE&&INE>IPE)  return polRotIN->GetScatVec();
// // else if(IPE) return polRotIP->GetScatVec();
// // else return NULL;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::ConstructParticle()
{
  fParticleList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::ConstructProcess()
{
  AddTransportation();
  fEmA2PhysicsList->ConstructProcess();
  fParticleList->ConstructProcess();
  for(size_t i=0; i<fHadronPhys.size(); i++) {
    fHadronPhys[i]->ConstructProcess();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>0) {
    G4cout << "A2PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }
  if (name == "emstandard_opt0") {

    delete fEmA2PhysicsList;
    fEmA2PhysicsList = new G4EmStandardPhysics();

  } else if (name == "emstandard_opt1") {

    delete fEmA2PhysicsList;
    fEmA2PhysicsList = new G4EmStandardPhysics_option1();

  } else if (name == "emstandard_opt2") {

    delete fEmA2PhysicsList;
    fEmA2PhysicsList = new G4EmStandardPhysics_option2();

  } else if (name == "emstandard_opt3") {

    delete fEmA2PhysicsList;
    fEmA2PhysicsList = new G4EmStandardPhysics_option3();

  } else if (name == "emstandard_opt4") {

    delete fEmA2PhysicsList;
    fEmA2PhysicsList = new G4EmStandardPhysics_option4();

  } else if (name == "FTFP_BERT_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("FTFP_BERT");

  } else if (name == "FTFP_BERT_EMX") {

    AddPhysicsList("emstandard_opt2");
    AddPhysicsList("FTFP_BERT");

  } else if (name == "FTFP_BERT_EMZ") {

    AddPhysicsList("emstandard_opt4");
    AddPhysicsList("FTFP_BERT");

  } else if (name == "FTFP_BERT") {

    SetBuilderList1();
    fHadronPhys.push_back( new G4HadronPhysicsFTFP_BERT());

  } else if (name == "FTF_BIC") {

    SetBuilderList0();
    fHadronPhys.push_back( new G4HadronPhysicsFTF_BIC());
    fHadronPhys.push_back( new G4NeutronCrossSectionXS(verboseLevel));

  } else if (name == "QBBC") {

    AddPhysicsList("emstandard_opt0");
    SetBuilderList2();
    fHadronPhys.push_back( new G4HadronInelasticQBBC());

  } else if (name == "QGSP_BERT") {

    SetBuilderList1();
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_BERT());

  } else if (name == "QGSP_FTFP_BERT") {

    SetBuilderList1();
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_FTFP_BERT());

  } else if (name == "QGSP_FTFP_BERT_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP_FTFP_BERT");

  } else if (name == "QGSP_BERT_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP_BERT");

  } else if (name == "QGSP_BERT_EMX") {

    AddPhysicsList("emstandard_opt2");
    AddPhysicsList("QGSP_BERT");

  } else if (name == "QGSP_BERT_HP") {

    SetBuilderList1(true);
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_BERT_HP());

  } else if (name == "QGSP_BIC") {

    SetBuilderList0();
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());

  } else if (name == "QGSP_BIC_EMY") {

    AddPhysicsList("emstandard_opt3");
    SetBuilderList0();
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());

  } else if (name == "QGS_BIC") {

    SetBuilderList0();
    fHadronPhys.push_back( new G4HadronPhysicsQGS_BIC());
    fHadronPhys.push_back( new G4NeutronCrossSectionXS(verboseLevel));

  } else if (name == "QGSP_BIC_HP") {

    SetBuilderList0(true);
    fHadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());

  } else {

    G4cout << "A2PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::SetBuilderList0(G4bool flagHP)
{
  fHadronPhys.push_back( new G4EmExtraPhysics(verboseLevel));
  if(flagHP) {
    fHadronPhys.push_back( new G4HadronElasticPhysicsHP(verboseLevel) );
  } else {
    fHadronPhys.push_back( new G4HadronElasticPhysics(verboseLevel) );
  }
  fHadronPhys.push_back( new G4StoppingPhysics(verboseLevel));
  fHadronPhys.push_back( new G4IonBinaryCascadePhysics(verboseLevel));
  fHadronPhys.push_back( new G4NeutronTrackingCut(verboseLevel));
}

//void A2PhysicsList::SetPolList(G4bool flagHP, G4bool isNew)
//{
//  fIsNew = isNew;
//  fhadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
//  if( isNew ){
//    fPolHadronElasticPhysicsN =
//     new PolHadronElasticPhysicsN("NPelastic", verboseLevel, flagHP);
//   hadronPhys.push_back( fPolHadronElasticPhysicsN );
// }
// else{
//   fPolHadronElasticPhysics =
//     new PolHadronElasticPhysics("NPelastic", verboseLevel, flagHP);
//   fhadronPhys.push_back( fPolHadronElasticPhysics );
// }
//  fhadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
//  fhadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
//  fhadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
//}
//void A2PhysicsList::SetNoHadList()
//{
//  fhadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
//  fhadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
// fhadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::SetBuilderList1(G4bool flagHP)
{
  fHadronPhys.push_back( new G4EmExtraPhysics(verboseLevel));
  if(flagHP) {
    fHadronPhys.push_back( new G4HadronElasticPhysicsHP(verboseLevel) );
  } else {
    fHadronPhys.push_back( new G4HadronElasticPhysics(verboseLevel) );
  }
  fHadronPhys.push_back( new G4StoppingPhysics(verboseLevel));
  fHadronPhys.push_back( new G4IonPhysics(verboseLevel));
  fHadronPhys.push_back( new G4NeutronTrackingCut(verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::SetBuilderList2()
{
  fHadronPhys.push_back( new G4EmExtraPhysics(verboseLevel));
  fHadronPhys.push_back( new G4HadronElasticPhysicsXS(verboseLevel) );
  fHadronPhys.push_back( new G4StoppingPhysics(verboseLevel));
  fHadronPhys.push_back( new G4IonPhysics(verboseLevel));
  fHadronPhys.push_back( new G4NeutronTrackingCut(verboseLevel));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::SetCuts()
{

  if (verboseLevel >0){
    G4cout << "A2PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(fCutForGamma, "gamma");
  SetCutValue(fCutForElectron, "e-");
  SetCutValue(fCutForPositron, "e+");
  SetCutValue(fCutForProton, "proton");

  if (verboseLevel>0) { DumpCutValuesTable(); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void A2PhysicsList::SetCutForGamma(G4double cut)
{
  fCutForGamma = cut;
  SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void A2PhysicsList::SetCutForElectron(G4double cut)
{
  fCutForElectron = cut;
  SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void A2PhysicsList::SetCutForPositron(G4double cut)
{
  fCutForPositron = cut;
  SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void A2PhysicsList::SetCutForProton(G4double cut)
{
  fCutForProton = cut;
  SetParticleCuts(fCutForProton, G4Proton::Proton());
}

void A2PhysicsList::List()
{
  G4cout << "### A2PhysicsLists available: FTFP_BERT FTFP_BERT_EMV "
         << "FTFP_BERT_EMX FTFP_BERT_EMZ"
         << G4endl;
  G4cout << "                            FTF_BIC QBBC QGSP_BERT "
         << "QGSP_BERT_EMV QGSP_BERT_EMX"
         << G4endl; 
  G4cout << "                            QGSP_BERT_HP QGSP_FTFP_BERT "
         << "QGSP_FTFP_BERT_EMV"
         << G4endl; 
  G4cout << "                            QGS_BIC QGSP_BIC QGSP_BIC_EMY "
         << "QGSP_BIC_HP" 
         << G4endl; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void A2PhysicsList::SetCutForRegion(G4String reg,G4double cut)
{
  G4Region* region=G4RegionStore::GetInstance()->GetRegion(reg);
  G4ProductionCuts* pcuts=new G4ProductionCuts;
  // pcuts->SetProductionCut(0.1*mm,G4ProductionCuts::GetIndex("e-"));
  pcuts->SetProductionCut(cut);
  if(region)region->SetProductionCuts(pcuts);
  else G4cout<<"A2PhysicsList::SetCutForRegion region "<<reg<<" not constructed"<<G4endl;
}
