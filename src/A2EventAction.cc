
#include "A2EventAction.hh"

#include "A2Hit.hh"
#include "A2VisHit.hh"
#include "A2RunAction.hh"
#include "A2EventActionMessenger.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "G4Trajectory.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
#include "TString.h"
#include <iomanip>


#include "Compression.h"


A2EventAction::A2EventAction(A2RunAction* run)
:frunAct(run),fdrawFlag("all"),fprintModulo(1),feventMessenger(0)
{
  feventMessenger = new A2EventActionMessenger(this);
  fIsInteractive=1;
  // hits collections
  fCBCollID = -1;
  fHitDrawOpt="edep";

  fOutFile=NULL;
  fOutTree=NULL;
  fOutFileName=TString("");

  fprintModulo=1000;
  fCBOut=NULL;
  fOverwriteFile=false;
}



A2EventAction::~A2EventAction()
{
  CloseOutput();
  delete feventMessenger;
}



void A2EventAction::BeginOfEventAction(const G4Event* evt)
{  
 G4int evtNb = evt->GetEventID();
 if (evtNb%fprintModulo ==0) { 
   G4cout <<evtNb%fprintModulo<< "---> Begin of event: " << evtNb << G4endl;
   //   CLHEP::HepRandom::showEngineStatus();
 }
}


void A2EventAction::EndOfEventAction(const G4Event* evt)
{
  //   G4cout<<" A2EventAction::EndOfEventAction"<<G4endl;
  //  A2VisHitsCollection* VisCBHC = NULL;
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  //In montecarlo mode
  //write to the output ntuple if it exists
  //if not need to set file via /A2/event/setOutputFile XXX.root
  if(fCBOut){
    fCBOut->WriteHit(HCE);
    fCBOut->WriteGenInput();
    fCBOut->GetTree()->Fill();
  }

  //Draw hits for interactive mode
  //Not for CB will change colour of each crystal hit
  //Currently all of TAPS and PID will change colour
  else if(fIsInteractive==1&&HCE){
    G4int CollSize=HCE->GetNumberOfCollections();
    G4int hci=0;
    //G4cout<<CollSize<<G4endl;
    for(G4int i=0;i<CollSize;i++){
      //Get the ball hit info to be written to output
      A2VisHitsCollection* hc;
    //Look for the hits collections with entries 
      while(!(hc=static_cast<A2VisHitsCollection*>(HCE->GetHC(hci++))));
      //G4cout<<hc->GetName()<<G4endl;
      //if(!hc)continue; //no hits in that detector
      G4int hc_nhits=hc->entries();
      //      if(hc->GetName()=="A2SDHitsVisCBSD"){
      if(hc->GetName().contains("Vis")){
	//	G4cout<<hc_nhits <<" hits"<<G4endl;
	for(G4int i=0;i<hc_nhits;i++){
	  A2VisHit* hit=static_cast<A2VisHit*>(hc->GetHit(i));
	  // G4cout<<"Got hit "<<hit<<G4endl;
      hit->Draw(1*CLHEP::MeV,fHitDrawOpt);
	  
	}
	
      }
    }
  } 
  




  // extract the trajectories and draw them

  // You can get a default drawing without this code by using, e.g.,
  // /vis/scene/add/trajectories 1000
  // The code here adds sophistication under control of drawFlag.

  // See comments in G4VTrajectory::DrawTrajectory for the
  // interpretation of the argument, 1000.
  //G4cout <<"Draw Trajectories"<<G4endl; 
  if(G4VVisManager::GetConcreteInstance()) {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");    
    G4TrajectoryContainer* trajContainer = evt->GetTrajectoryContainer();
    G4int n_trajectories = 0;

    if(trajContainer) n_trajectories = trajContainer->entries();
    for (G4int i=0; i<n_trajectories; i++) {
      G4Trajectory* trj = (G4Trajectory*)(*trajContainer)[i];
      if (fdrawFlag == "all") 
        trj->DrawTrajectory();
      else if ((fdrawFlag == "charged") && (trj->GetCharge() != 0.))
        trj->DrawTrajectory();
      else if ((fdrawFlag == "neutral") && (trj->GetCharge() == 0.))
        trj->DrawTrajectory();
    }
    
    // G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");    
  } 
  // G4VVisManager* pVisManager = G4VVisManager::GetConcreteInstance();
  // if (pVisManager)
  //   {
  //    G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
  //    G4int n_trajectories = 0;
  //    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  //    for (G4int i=0; i<n_trajectories; i++) 
  //       { G4VTrajectory* trj = ((*(evt->GetTrajectoryContainer()))[i]);
  // 	  //trj->ShowTrajectory();
  // 	  //G4cout<<G4endl;
  // 	  if (fdrawFlag == "all") pVisManager->Draw(*trj,100);
  //         else if ((fdrawFlag == "charged")&&(trj->GetCharge() != 0.))
  //                                 pVisManager->Draw(*trj,100);
  //         else if ((fdrawFlag == "neutral")&&(trj->GetCharge() == 0.))
  //                                 pVisManager->Draw(*trj,100);
  //       }
  // }
}  

G4int A2EventAction::PrepareOutput(){
  //If no filename don't save output

  if(fOutFileName == "") {
    G4cout<<"G4int A2EventAction::PrepareOutput() no out put file to be written"<<G4endl;
    G4cout<<"Please supply filename if output required, try :"<<G4endl;
    G4cout<<"/A2/event/SetOutputFile XXX.root"<<G4endl;
    return 0;
  }

  fOutFile=new TFile(fOutFileName,"RECREATE");
  //if file aready exists make a new name by adding XXXA2copy#.root
  if(!fOutFile->IsOpen()) {
      G4cerr << "unable to open output file " << fOutFileName << endl;
      exit(1);
  }

  fOutFile->SetCompressionAlgorithm(ROOT::kLZMA);
  fOutFile->SetCompressionLevel(5);

  G4cout<<"A2EventAction::PrepareOutput() Output will be written to " << fOutFileName << G4endl;

  //Create output tree
  //This is curently made in the same format as the cbsim output
  fCBOut = new A2CBOutput();
  fCBOut->SetFile(fOutFile);
  fCBOut->SetBranches();

  return 1;
}

void  A2EventAction::CloseOutput(){

    if(fCBOut) {

        fOutFile->cd();
        fCBOut->CopyTIDTree();
        fCBOut->WriteTree();

        delete fCBOut;
        fCBOut=nullptr;
    }

    if(fOutFile) {
        fOutFile->Close();
        delete fOutFile;
        fOutFile = nullptr;
    }

}
