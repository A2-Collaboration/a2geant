

#include "npolSteppingAction.hh"

#include "npolDetCon.hh"
#include "npolEventAction.hh"

#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4SteppingManager.hh"

////#include "G4RunManager.hh"



npolSteppingAction::npolSteppingAction(npolDetectorConstruction* det,
                                         npolEventAction* evt)
:detector(det), eventaction(evt)					 
{ fInScatYet=false;fProtYet=true;fNeutYet=true;}



npolSteppingAction::~npolSteppingAction()
{ }



void npolSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  //  return;
  G4Track* track = aStep->GetTrack();
   G4VPhysicalVolume* volume = track->GetVolume();
   // G4cout<<track->GetDefinition()->GetParticleName()<<" "<<track->GetKineticEnergy()<<" "<<volume->GetName()<<G4endl;
  if(volume->GetName()==G4String("Scat")){fInScatYet=true;fProtYet=false;fNeutYet=false;}
  if(volume->GetName()==G4String("World")&&fInScatYet){
    G4double ek = track->GetKineticEnergy();
    if(ek>1*MeV){ //cut out recoil particles
      if(track->GetDefinition()->GetParticleName()==G4Proton::Proton()->GetParticleName()&&!fProtYet)
	{eventaction->SetSPMom(track->GetMomentum());fProtYet=true;}
      if(track->GetDefinition()->GetParticleName()==G4Neutron::Neutron()->GetParticleName()&&!fNeutYet)
	{eventaction->SetSNMom(track->GetMomentum());fNeutYet=true;}
      //if(track->GetCreatorProcess())G4cout<<track->GetDefinition()->GetParticleName()<<" "<<track->GetCreatorProcess()->GetProcessName()<<G4endl;
    }
    //track->SetTrackStatus(fStopAndKill);
    if(!fNeutYet||!fProtYet)fInScatYet=true;
    else {fInScatYet=false;    track->SetTrackStatus(fStopAndKill);}
  }
  //  return;
  if(track->GetDefinition()->GetParticleName()==G4String("kaon+")&&fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("Decay")){

    G4cout<<"kaon+ "<< track->GetTrackID()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;
     }
  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  G4double stepl = 0.;
  if (track->GetDefinition()->GetPDGCharge() != 0.)
    stepl = aStep->GetStepLength();
    if(track->GetDefinition()->GetParticleName()==G4Gamma::Gamma()->GetParticleName()){
      // if(track->GetDefinition()->GetParticleName()==G4String("kaon+")){

      //G4cout<<"gamma "<< track->GetTrackID()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;
     }
  if(track->GetDefinition()->GetParticleName()==G4Proton::Proton()->GetParticleName()&&!(fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("msc"))&&!(fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("hIoni"))){

    // G4cout<<"Proton "<< track->GetTrackID()<< " "<<track->GetParentID()<< " "<<track->GetKineticEnergy()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;
    //G4cout<<"Secondaries "<<aStep->GetSecondary()->size()<<" "<<aStep->GetfSecondary()<<G4endl;
  }


}





