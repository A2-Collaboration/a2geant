#include "G4RunManager.hh"
#include "A2VisSD.hh"
#include "A2VisHit.hh"
#include "A2EventAction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4Trajectory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"

#include "stdio.h"

A2VisSD::A2VisSD(G4String name,G4int Nelements):G4VSensitiveDetector(name)
{
  collectionName.insert(G4String("A2SDHits")+name);
  fCollection=NULL;

  fNelements=Nelements+1;//numbering starts from 1 not 0
  fhitID=new G4int[fNelements];
  for(G4int i=0;i<fNelements;i++)fhitID[i]=-1;
  fHits=new G4int[fNelements];
  for(G4int i=0;i<fNelements;i++)fHits[i]=0;
 
  fNhits=0;
  fHCID=-1; 
}


A2VisSD::~A2VisSD()
{

}


void A2VisSD::Initialize(G4HCofThisEvent*)
{

  //Hits collections are deleted in G4RunManager, DoEventLoop
  //This calls StackPreviousEvent which deletes the G4Event
  //This deletes the hit collection for the event and thus A2Hit s
  //G4cout<<"void A2SD::Initialize "<<SensitiveDetectorName<<G4endl;
  fCollection = new A2VisHitsCollection(SensitiveDetectorName,collectionName[0]);
}


G4bool A2VisSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{ 
 
  G4double edep = aStep->GetTotalEnergyDeposit();
  if ((edep/CLHEP::keV == 0.)) return false;      
  
  // This TouchableHistory is used to obtain the physical volume
  // of the hit
  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  G4VPhysicalVolume* volume=theTouchable->GetVolume();
  G4VPhysicalVolume* mothervolume=theTouchable->GetVolume(1);
  G4int id;
  //Get element copy number
  //TAPS volume  is contained in COVR which is the multiple placed volume!
  if(volume->GetName().contains("TAPS")||volume->GetName().contains("PbWO"))id=mothervolume->GetCopyNo()+volume->GetCopyNo();
  else id = volume->GetCopyNo();
  //  G4cout<<volume->GetName()<<" id "<<id <<" "<<volume->GetCopyNo()<<" "<<mothervolume->GetCopyNo()<<" edep "<<edep/MeV<<G4endl;

  //Get charge
  G4int charge=(G4int)aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  //G4int origID=aStep->GetTrack()->GetParentID();

//   //Try to get the original particle that created this track but G4Trajectory::GetParentID always returns 0!!!!
//   const G4Event *evt=(G4RunManager::GetRunManager())->GetCurrentEvent();
//   G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
//   G4int Ntracks=trajectoryContainer->size();
//   G4cout<<"number of trajs "<<trajectoryContainer->size()<<" parent ID "<<origID<<" track "<<aStep->GetTrack()->GetTrackID()<<G4endl;
//   G4int *trackIDs=new G4int[Ntracks];
//   for(G4int i=0;i<Ntracks;i++){
//     G4Trajectory* trj=static_cast<G4Trajectory*>((*trajectoryContainer)[i]);
//     trackIDs[i]=trj->GetTrackID();
//  }
//   G4int parentTrackIndex=-1;
//   for(G4int i=0;i<Ntracks;i++){
//     G4Trajectory* trj=static_cast<G4Trajectory*>((*trajectoryContainer)[i]);
//     //if(trj->GetParentID()==0) break; //doesn't have a parent
//     //G4cout<<"Parent "<<trj->GetParentID()<<" "<<trackIDs<<" "<<i<<G4endl;
//     //if(trj->GetParentID()==trackIDs[i]) parentTrackIndex=i;//Find parent track
//     if(aStep->GetTrack()->GetParentID()==trackIDs[i]) parentTrackIndex=i;//Find parent track
//   }
//   if(parentTrackIndex>-1){
//     G4Trajectory* trj=static_cast<G4Trajectory*>((*trajectoryContainer)[parentTrackIndex]);
//     charge=trj->GetCharge();
//     origID=trj->GetParentID();
//     //G4cout<<"id "<<trj->GetTrackID() <<" name "<<trj->GetParticleName()<<" edep "<<edep/MeV<<G4endl;
//   }


  if (fhitID[id]==-1){
    //if this crystal has already had a hit
    //don't make a new one, add on to old one.   
    //G4cout<<"Make hit "<<fCollection<<G4endl;    
    A2VisHit* myHit = new A2VisHit();
    //standard hit stuff
    myHit->AddEnergy(edep);
    myHit->SetPos(aStep->GetPreStepPoint()->GetPosition());
    myHit->SetID(id);
    myHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    //visualisation hit stuff
    myHit->SetLogicalVolume(theTouchable->GetVolume()->GetLogicalVolume());
    myHit->SetCharge(charge);
    //insert hit
    fhitID[id] = fCollection->insert(myHit) -1;
    fHits[fNhits++]=id;
 
  }
  else // This is not new
    {
    (*fCollection)[fhitID[id]]->AddEnergy(edep);
    }
  return true;
}


void A2VisSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  // G4cout<<"EndOfEvent( "<<fHCID<<" "<<fCollection<<" "<<fNhits<<G4endl;
  if(fHCID<0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  if(fNhits>0)  HCE->AddHitsCollection(fHCID,fCollection);
  //G4cout<<"EndOfEvent( "<<G4endl;
 
  //reset hit arrays
  for (G4int i=0;i<fNhits;i++) 
    {
      fhitID[fHits[i]]=-1;
      fHits[i]=0;
    }
  fNhits=0;
}



void A2VisSD::clear()
{} 


void A2VisSD::DrawAll()
{} 


void A2VisSD::PrintAll()
{} 















