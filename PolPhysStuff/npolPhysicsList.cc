// SBS a Geant-4 Based Model of Hall-A Experiments with 11 GeV
// J.R.M Annand, University of Glasgow
// Class npolPhysicsList
// Control of physics used to describe particle interactions in matter
// 20/06/09 JRMA adapted from A2 equivalent, under construction
// 03/02/10 JRMA add polarised nucleon control
// 09/04/10 JRMA NPol classes
// 17/04/10 JRMA Pol classes
// 15/01/11 JRMA Remove obsolete (as from G4.9.4) physics classes
//  7/09/11 JRMA Add new polarised elastic class

#include "npolPhysicsList.hh"
#include "npolPhysicsListMessenger.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronDElasticPhysics.hh"
#include "G4HadronQElasticPhysics.hh"
#include "G4HadronHElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4QStoppingPhysics.hh"
#include "G4LHEPStoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"
#include "PolHadronQuasiFreeProcess.hh"
// Remove obsolete (G4.9.4) physics lists
//#include "HadronPhysicsFTFC.hh"
//#include "HadronPhysicsFTFP.hh"
//#include "HadronPhysicsFTFP_BERT.hh"
#include "HadronPhysicsLHEP.hh"
//#include "HadronPhysicsLHEP_BERT.hh"
#include "HadronPhysicsLHEP_EMV.hh"
//#include "HadronPhysicsLHEP_PRECO_HP.hh"
#include "G4HadronInelasticQBBC.hh"
//#include "HadronPhysicsQGSC.hh"
#include "HadronPhysicsQGSC_BERT.hh"
//#include "HadronPhysicsQGSC_EFLOW.hh"
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#include "HadronPhysicsQGSP_BIC.hh"
#include "HadronPhysicsQGSP_BIC_HP.hh"

#include "G4HadronInelasticQLHEP.hh"
#include "G4IonPhysics.hh"
//#include "G4HadronProcessStore.hh"

#include "G4LossTableManager.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "PolHadronElasticPhysics.hh"
#include "PolHadronElasticPhysicsN.hh"
#include "PolHadronInelasticPhysics.hh"
#include "PolWHadronElasticProcess.hh"
#include "PolNucleonRotate.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

npolPhysicsList::npolPhysicsList() : G4VModularPhysicsList()
{
  G4LossTableManager::Instance();
  defaultCutValue = 1.*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
  dump            = false;
  fPolHadronElasticPhysics = NULL;
  fPolHadronElasticPhysicsN = NULL;
  fPolHadronInelasticPhysics = NULL;
  fIsNew = true;
  fSaveScat=true;
  pMessenger = new npolPhysicsListMessenger(this);

  // Particles
  particleList = new G4DecayPhysics("decays");

  // EM physics
  emPhysicsList = new G4EmStandardPhysics();
  verboseLevel=2;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

npolPhysicsList::~npolPhysicsList()
{
  delete pMessenger;
  delete particleList;
  delete emPhysicsList;
  for(size_t i=0; i<hadronPhys.size(); i++) {
    delete hadronPhys[i];
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

PolNucleonRotate* npolPhysicsList::GetPolNucleonRotate(G4String type)
{
  if(type==G4String("n-elastic"))return  fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRot();
  else if(type==G4String("p-elastic"))return fPolHadronElasticPhysicsN->GetpElPolProc()->GetPolRot();
  else if(type==G4String("p-inelastic"))return fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRot();
  else if(type==G4String("n-inelastic"))return fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRot();
  //else if(type==G4String("p-inelastic-qf"))return ((PolHadronQuasiFreeProcess*)fPolHadronInelasticPhysics->GettheProtonInelastic())->GetPolRotNP();//MHS, Nov 14, 2014, "p-inelastic-qf" not listed in npolPhysicsListMessenger
  //else if(type==G4String("n-inelastic-qf"))return ((PolHadronQuasiFreeProcess*)fPolHadronInelasticPhysics->GettheNeutronInelastic())->GetPolRotNP();MHS, Nov 14, 2014,"n-inelastic-qf" not listed in npolPhysicsListMessenger
  else {G4cerr<<" npolPhysicsList::GetPolNucleonRotate() No process "<<type<<G4endl;return NULL;}
}
void npolPhysicsList::ResetEvent(){
  if(!fSaveScat) return;
  // if(fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent())fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent()->ResetEvent();
  // if(fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent())fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent()->ResetEvent();
  // if(fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent())fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent()->ResetEvent();
    // G4String process[]={ "p-elastic", "n-elastic", "p-inelastic", "n-inelastic", "p-inelastic-qf", "n-inelastic-qf"};
  G4String process[]={ "p-elastic", "n-elastic", "p-inelastic", "n-inelastic"};//MHS Nov 14, 2014
    for(G4int i=0;i<4;i++){
      PolNucleonRotate* fPolRot = GetPolNucleonRotate(process[i]);
      if (fPolRot) fPolRot->ResetEvent();
      //if (fPolRot) fPolRot->ToggleDidScat(false);
    }
   
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4LorentzVector* npolPhysicsList::GetScatVec(){
  //This can be quite ambigous, particularly if there are more than 1 original nucleons
  //The highest energy incident nucleon that participates in a scatter is taken for
  //the saved nucleon
  if(!fSaveScat) return NULL;
  PolNucleonRotate* polRotE=fPolHadronElasticPhysicsN->GetnElPolProc()->GetPolRotEvent();
  PolNucleonRotate* polRotIN=fPolHadronInelasticPhysics->GettheNeutronInelastic()->GetPolRotEvent();
  PolNucleonRotate* polRotIP=fPolHadronInelasticPhysics->GettheProtonInelastic()->GetPolRotEvent();
  G4double NE,INE,IPE;
  NE=INE=IPE=0;
  if(polRotE)NE=polRotE->GetScatE();
  if(polRotIN)INE=polRotIN->GetScatE();
  if(polRotIP)IPE=polRotIP->GetScatE();

  if(NE&&NE>INE&&NE>IPE) return polRotE->GetScatVec();
  else if(INE&&INE>IPE)  return polRotIN->GetScatVec();
  else if(IPE) return polRotIP->GetScatVec();
  else return NULL;
}
void npolPhysicsList::ConstructParticle()
{
  particleList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::ConstructProcess()
{
  AddTransportation();
  emPhysicsList->ConstructProcess();
  particleList->ConstructProcess();
  for(size_t i=0; i<hadronPhys.size(); i++) {
    hadronPhys[i]->ConstructProcess();
  }

  //  if(dump) G4HadronProcessStore::Instance()->Dump(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>0)
    G4cout << "npolPhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  G4cout<<"****************npolphysicslist***************"<<G4endl;
  if (name == "emstandard_opt2") {

    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option2();

  } else if (name == "emstandard_opt1") {

    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option1();
    
  } /* else if (name == "FTFC") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFC("hadron",true));
    dump = true;

  } else if (name == "FTFP") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFP("hadron",true));
    dump = true;

  } else if (name == "FTFP_BERT") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsFTFP_BERT("hadron",true));
    dump = true;
    } */
  else if (name == "FTFP_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("FTFP");

  } else if (name == "LHEP") {

    SetBuilderList2();
    hadronPhys.push_back( new HadronPhysicsLHEP("hadron"));
    dump = true;

  } /* else if (name == "LHEP_BERT") {

    SetBuilderList3();
    hadronPhys.push_back( new HadronPhysicsLHEP_BERT("hadron"));
    dump = true;
    } */
    else if (name == "LHEP_EMV") {

    AddPhysicsList("emstandard_opt1");
    SetBuilderList3();
    hadronPhys.push_back( new HadronPhysicsLHEP_EMV("hadron"));
    dump = true;

    /* } else if (name == "LHEP_PRECO_HP") {

    SetBuilderList3(true);
    hadronPhys.push_back( new HadronPhysicsLHEP_PRECO_HP("hadron"));
    dump = true;
    */
  } else if (name == "QBBC") {

    SetBuilderList0();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,false,true));

  } else if (name == "QBBC_DEL") {

    SetBuilderList5();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,false,true));

  } else if (name == "QBBC_HEL") {

    SetBuilderList6();
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,false,true));

  } else if (name == "QBBC_HP") {

    SetBuilderList0(true);
    hadronPhys.push_back( new G4HadronInelasticQBBC("QBBC",verboseLevel,
						    false,false,false,true,true));
    /*  } else if (name == "QGSC") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC("hadron",true));
    dump = true;
    */
  } else if (name == "QGSC_BERT") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC_BERT("hadron",true));
    dump = true;

    /*  } else if (name == "QGSC_EFLOW") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSC_EFLOW("hadron",true));
    dump = true;
    */
  } else if (name == "QGSC_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSC");

  } else if (name == "QGSP") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",true));
    dump = true;

  } else if (name == "QGSP_BERT") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP_BERT("hadron",true));
    dump = true;

  } else if (name == "QGSP_BERT_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP_BERT");

  } else if (name == "QGSP_BERT_HP") {

    SetBuilderList1(true);
    hadronPhys.push_back( new HadronPhysicsQGSP_BERT_HP("hadron",true));

  } else if (name == "QGSP_BIC") {

    SetBuilderList0();
    hadronPhys.push_back( new HadronPhysicsQGSP_BIC("hadron",true));
    dump = true;

  } else if (name == "NoHadronic") {

    SetNoHadList();
    //hadronPhys.push_back( new HadronPhysicsQGSP_BIC("hadron",true));
    dump = true;

  } else if (name == "QGSP_BIC_POL") {

    SetPolList();
    fPolHadronInelasticPhysics = new PolHadronInelasticPhysics();
    hadronPhys.push_back( fPolHadronInelasticPhysics );
    dump = true;

  } else if (name == "QGSP_BIC_POLN") {

    SetPolList(false,true);
    fPolHadronInelasticPhysics = new PolHadronInelasticPhysics();
    hadronPhys.push_back( fPolHadronInelasticPhysics );
    dump = true;
  } else if (name == "QGSP_BIC_POLQF") {

    SetPolList(false,true);
    fPolHadronInelasticPhysics = new PolHadronInelasticPhysics("hadronQF");
    hadronPhys.push_back( fPolHadronInelasticPhysics );
    dump = true;

  } else if (name == "QGSP_BIC_HP") {
      G4cout<<"**********************************"<<G4endl;
    SetBuilderList0(true);
    hadronPhys.push_back( new HadronPhysicsQGSP_BIC_HP("hadron",true));
    dump = true;

  } else if (name == "QGSP_DIF") {

    SetBuilderList1();
    HadronPhysicsQGSP * hp = new HadronPhysicsQGSP("hadron");
    hp->SetQuasiElastic(true);
    hp->SetProjectileDiffraction(true);
    hadronPhys.push_back(hp);
    dump = true;

  } else if (name == "QGSP_EMV") {

    AddPhysicsList("emstandard_opt1");
    AddPhysicsList("QGSP");
    dump = true;

  } else if (name == "QGSP_EMX") {

    AddPhysicsList("emstandard_opt2");
    AddPhysicsList("QGSP");
    dump = true;

  } else if (name == "QGSP_NQE") {

    SetBuilderList1();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",false));
    dump = true;

  } else if (name == "QGSP_QEL") {

    SetBuilderList4();
    hadronPhys.push_back( new HadronPhysicsQGSP("hadron",true));
    dump = true;

  } else {

    G4cout << "npolPhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList0(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("elastic",verboseLevel,
						    flagHP));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

void npolPhysicsList::SetPolList(G4bool flagHP, G4bool isNew)
{
  fIsNew = isNew;
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  if( isNew ){
    fPolHadronElasticPhysicsN =
      new PolHadronElasticPhysicsN("NPelastic", verboseLevel, flagHP);
    hadronPhys.push_back( fPolHadronElasticPhysicsN );
  }
  else{
    fPolHadronElasticPhysics =
      new PolHadronElasticPhysics("NPelastic", verboseLevel, flagHP);
    hadronPhys.push_back( fPolHadronElasticPhysics );
  }
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}
void npolPhysicsList::SetNoHadList()
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList1(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("elastic",verboseLevel,
						    flagHP));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList2(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("LElastic",verboseLevel,
						    flagHP));
  hadronPhys.push_back( new G4IonPhysics("ion"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList3(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronElasticPhysics("LElastic",verboseLevel,
						    flagHP));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList4(G4bool)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronQElasticPhysics("elastic",verboseLevel));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonPhysics("ion"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList5(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronDElasticPhysics(verboseLevel,flagHP));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetBuilderList6(G4bool flagHP)
{
  hadronPhys.push_back( new G4EmExtraPhysics("extra EM"));
  hadronPhys.push_back( new G4HadronHElasticPhysics(verboseLevel,flagHP));
  hadronPhys.push_back( new G4QStoppingPhysics("stopping",verboseLevel));
  hadronPhys.push_back( new G4IonBinaryCascadePhysics("ionBIC"));
  hadronPhys.push_back( new G4NeutronTrackingCut("Neutron tracking cut"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetCuts()
{

  if (verboseLevel >0){
    G4cout << "npolPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void npolPhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolPhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolPhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

void npolPhysicsList::List()
{
  G4cout << "### npolyPhysicsLists available: FTFC FTFP FTFP_BERT FTFP_EMV LHEP LHEP_BERT LHEP_EMV "
	 << G4endl;
  G4cout << "                            LHEP_PRECO_HP QBBC QBBC_DEL QBBC_HEL QBBC_HP QGSC "
	 << G4endl; 
  G4cout << "                            QGSC_BERT QGSC_EFLOW QGSC_EMV QGSP QGSP_BERT QGSP_BER_EMV "
	 << G4endl; 
  G4cout << "                            QGSP_BERT_HP QGSP_BIC QGSP_BIC_HP QGSP_DIF " 
	 << G4endl; 
  G4cout << "                            QGSP_EMV QGSP_EMX QGSP_NQE QGSP_QEL  "
	 << G4endl; 
  G4cout << "                            QGSP_BIC_POL QGSP_BIC_POLN NoHadronic"
	 << G4endl; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

