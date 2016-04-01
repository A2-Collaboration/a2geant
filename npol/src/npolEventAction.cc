
#include "npolEventAction.hh"
#include "npolHit.hh"
#include "PolPhysicsList.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

npolEventAction::npolEventAction()
{
  //eventMessenger = new npolEventActionMessenger(this);
  npolCollID=-1;
 
  //Construct Root output
  fnpolFile=NULL;
  fnpolTree=NULL;
  fIncMom=NULL;
  fRealScat=NULL;
  fSPMom=NULL;
  fSNMom=NULL;
  fPL=NULL;
  fnpolFile= new TFile("npol.root","RECREATE","npolrmanium Simulation");
  fnpolTree= new TTree("npol","npolillator Output");
 
  fIncMom=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fRealScat=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fSPMom=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fSNMom=new TLorentzVector(0,0,0,G4Neutron::Neutron()->GetPDGMass()/MeV);

  fnpolTree->Branch("Inc","TLorentzVector",&fIncMom);
  fnpolTree->Branch("SP","TLorentzVector",&fSPMom);
  fnpolTree->Branch("SN","TLorentzVector",&fSNMom);
  fnpolTree->Branch("RealS","TLorentzVector",&fRealScat);
  fnpolTree->Branch("edep",&fedep,"fedep/F",64000);
  fnpolTree->Branch("time",&ftime,"ftime/F",64000);

  totEvents=0; 
  
}
npolEventAction::npolEventAction(G4String filename)
{
  //eventMessenger = new npolEventActionMessenger(this);
  npolCollID=-1;
 
  //Construct Root output
  fnpolFile=NULL;
  fnpolTree=NULL;
  fIncMom=NULL;
  fRealScat=NULL;
  fSPMom=NULL;
  fSNMom=NULL;
  fPL=NULL;
  
  fnpolFile= new TFile(filename.data(),"RECREATE","npolrmanium Simulation");
  fnpolTree= new TTree("npol","npolillator Output");
 
  fIncMom=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fRealScat=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fSPMom=new TLorentzVector(0,0,0,G4Proton::Proton()->GetPDGMass()/MeV);
  fSNMom=new TLorentzVector(0,0,0,G4Neutron::Neutron()->GetPDGMass()/MeV);

  fnpolTree->Branch("Inc","TLorentzVector",&fIncMom);
  fnpolTree->Branch("SP","TLorentzVector",&fSPMom);
  fnpolTree->Branch("SN","TLorentzVector",&fSNMom);
  fnpolTree->Branch("RealS","TLorentzVector",&fRealScat);
  fnpolTree->Branch("edep",&fedep,"fedep/F",64000);
  fnpolTree->Branch("time",&ftime,"ftime/F",64000);

  totEvents=0; 
  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

npolEventAction::~npolEventAction()
{

  if(fnpolTree&&fnpolFile){fnpolFile->cd();fnpolTree->Write();}
  if(fnpolFile)fnpolFile->Close();
  // delete fnpolNtuple;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void npolEventAction::BeginOfEventAction(const G4Event* evt)
{
  //fIncMom->SetXYZM(0,0,0,fIncMom->M());
  fSPMom->SetXYZM(0,0,0,fSPMom->M());
  fSNMom->SetXYZM(0,0,0,fSNMom->M());
 if (npolCollID==-1)
   {
    G4SDManager * SDman = G4SDManager::GetSDMpointer();
    npolCollID = SDman->GetCollectionID("npolCollection");
   } 
 //if(fPL) {fPL->ResetEvent();fRealScat->SetXYZT(0,0,0,0);}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void npolEventAction::EndOfEventAction(const G4Event* evt)
{

  totEvents++;

  G4RunManager *runMan = G4RunManager::GetRunManager();
  G4int runID=runMan->GetCurrentRun()->GetRunID();
  
  G4int evtNb = evt->GetEventID();

  if(evtNb%1000==0) G4cout<<evtNb<<G4endl;
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  npolHitsCollection* nHC = NULL;
  G4int n_hit = 0;
  fedep=0; 
  ftime=0; 
 if (HCE) nHC = (npolHitsCollection*)(HCE->GetHC(npolCollID));

 // G4cout<<HCE<<" "<<nHC<<" "<<npolCollID<<G4endl;
  if(nHC){
     n_hit = nHC->entries();
     // G4cout<<n_hit<<G4endl;
     if(n_hit==1){
       fedep = (*nHC)[0]->GetEdepnpol()/MeV;
       ftime = (*nHC)[0]->GetTimenpol()/ns;
       //G4cout<<fedep<<G4endl;
     }
  }
  if(fPL){
    G4LorentzVector* scatvec=NULL; 
    if(fPL->IsSaveScat())
      scatvec=fPL->GetScatVec();
    if(scatvec) {
      fRealScat->SetXYZT(scatvec->x(),scatvec->y(),scatvec->z(),scatvec->t());
      //fPL->ResetEvent();
    }
  }  
  fnpolTree->Fill();
  
  //G4cout<<"Energy deposited "<<fedep<<G4endl;
  
 if (G4VVisManager::GetConcreteInstance())
    {
     G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
     G4int n_trajectories = 0;
     if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

     for (G4int i=0; i<n_trajectories; i++) 
        { 
	  G4Trajectory* trj = (G4Trajectory*)((*(evt->GetTrajectoryContainer()))[i]);
	  trj->DrawTrajectory();
          
        }
  }


}
