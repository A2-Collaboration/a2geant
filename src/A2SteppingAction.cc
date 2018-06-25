

#include "A2SteppingAction.hh"

#include "A2DetectorConstruction.hh"
#include "A2EventAction.hh"

#include "G4Track.hh"
#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4SteppingManager.hh"

////#include "G4RunManager.hh"



A2SteppingAction::A2SteppingAction(A2DetectorConstruction* det,
                                         A2EventAction* evt)
:detector(det), eventaction(evt)					 
{ }



A2SteppingAction::~A2SteppingAction()
{ }



void A2SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  //  return;
  G4Track* track = aStep->GetTrack();
//   G4VPhysicalVolume* volume = track->GetVolume();
  
//   // collect energy and track length step by step
//   G4double edep = aStep->GetTotalEnergyDeposit();
  
//   G4double stepl = 0.;
//stop tracking after the trigger time
  if(aStep->GetPreStepPoint()->GetGlobalTime()>2*CLHEP::ms)track->SetTrackStatus(fStopAndKill);
//   if(track->GetDefinition()->GetParticleName()==G4String("pi0"))
//     {G4cout<<"Got a pi0 "<<aStep->GetPreStepPoint()->GetGlobalTime()/ns<<" "<<track->GetKineticEnergy()/MeV<<" "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;track->SetTrackStatus(fStopAndKill);}
//  if(track->GetDefinition()->GetParticleName()==G4String("pi+"))
//    {G4cout<<"Got a pi+ "<<aStep->GetPreStepPoint()->GetGlobalTime()/ns<<" "<<track->GetKineticEnergy()/MeV<<" "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;track->SetTrackStatus(fStopAndKill);}
//  if(track->GetDefinition()->GetParticleName()==G4String("mu+"))
//    {G4cout<<"Got a mu+ "<<aStep->GetPreStepPoint()->GetGlobalTime()/ns<<" "<<track->GetKineticEnergy()/MeV<<" "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;track->SetTrackStatus(fStopAndKill);}
 //  if(track->GetDefinition()->GetParticleName()==G4String("pi0"))
//     {track->SetTrackStatus(fStopAndKill);}
//  if(track->GetDefinition()->GetParticleName()==G4String("pi+"))
//    {track->SetTrackStatus(fStopAndKill);}
//  if(track->GetDefinition()->GetParticleName()==G4String("pi-"))
//    {track->SetTrackStatus(fStopAndKill);}
//  if(track->GetDefinition()->GetParticleName()==G4String("mu+"))
//    {track->SetTrackStatus(fStopAndKill);}
  //if (track->GetDefinition()->GetPDGCharge() != 0.)
  // stepl = aStep->GetStepLength();
  //if(track->GetDefinition()->GetParticleName()==G4Gamma::Gamma()->GetParticleName()){

  //G4cout<<track->GetDefinition()->GetParticleName()<< track->GetTrackID()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;
  // }
  //  if(fpSteppingManager->GetfCurrentVolume()->GetName()==G4String("ANOIP"))G4cout<<"OK "<<track->GetDefinition()->GetParticleName()<<G4endl;
  //if(track->GetDefinition()->GetParticleName()==G4Proton::Proton()->GetParticleName()&&!(fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("msc"))&&!(fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("hIoni"))){
  // if(fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("msc"))
  // if(track->GetTrackID()==1&&fpSteppingManager->GetfCurrentVolume()->GetName()!=G4String("World")) G4cout<<track->GetDefinition()->GetParticleName()<< track->GetTrackID()<< " "<<track->GetParentID()<< " "<<track->GetKineticEnergy()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<<G4endl;
    //G4cout<<"Secondaries "<<aStep->GetSecondary()->size()<<" "<<aStep->GetfSecondary()<<G4endl;
  //}
  //G4cout <<" STEPPING ACTION "<<eventaction->GetNEvent()
  //  if(eventaction->GetNEvent()==1317){
  // G4cout<<track->GetDefinition()->GetParticleName()<< track->GetTrackID()<<" process " <<fpSteppingManager->GetfCurrentProcess()->GetProcessName()<<" in "<< fpSteppingManager->GetfCurrentVolume()->GetName()<< " "<<track->GetKineticEnergy()/MeV<<G4endl;
  // }

  //bug in phot process, can't get rid of gamma with energy 1.2E-5MeV
  //goes into infinite loop!
  if(track->GetDefinition()->GetParticleName()==G4Gamma::Gamma()->GetParticleName()&&track->GetKineticEnergy()/CLHEP::MeV<1E-4&&fpSteppingManager->GetfCurrentProcess()->GetProcessName()==G4String("phot"))track->SetTrackStatus(fStopAndKill);
}





