#include "G4RunManager.hh"
#include "A2VisCBSD.hh"
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

A2VisCBSD::A2VisCBSD(G4String name):G4VSensitiveDetector(name)
{
  collectionName.insert("A2VisCBSDHits");
  fCollection=NULL;
  //HArd wire in crystal numbers
  //Note this cooresponds to the number of entries in 
  //the fcrystal_convert array and so includes "missing" crystals
  fNcrystals=720;
  fhitID=new G4int[fNcrystals];
  for(G4int i=0;i<fNcrystals;i++)fhitID[i]=-1;
  fHits=new G4int[fNcrystals];
  for(G4int i=0;i<fNcrystals;i++)fHits[i]=0;
  fCrystalConvert=new G4int[fNcrystals];
  fNhits=0;
  //Read in the AcquRoot id map
  FILE* CCfile=fopen("CrystalConvert.in","r");
  if(!CCfile){
    G4cerr<<"Couldn't open CrystalConvert.in in A2VisCBSD::A2VisCBSD(G4String name)"<<G4endl;
    exit(1);
  }
  G4int cc=0;
  for(G4int i=0;i<20;i++){//major loop
    char major[100];
    fscanf(CCfile,"%*s%*s%s",major);//take out Major string
    int id=0;
    for(G4int j=0;j<4;j++) //minor loop
      for(G4int k=0;k<9;k++){//crystal loop	
	fscanf(CCfile,"%d",&id);
	fCrystalConvert[cc++]=id;
      }
  }
  if(fCrystalConvert[719]!=621) {G4cerr<<"A2VisCBSD::A2VisCBSD Might have a problem with crystal convert array!!!"<<G4endl;exit(0);}

}


A2VisCBSD::~A2VisCBSD()
{

}


void A2VisCBSD::Initialize(G4HCofThisEvent*)
{

  G4cout<<"You are using A2VisCBSD which is not meant for serious number crunching"<<G4endl;
  fCollection = new A2VisHitsCollection(SensitiveDetectorName,collectionName[0]);

  //Set the hit collection id in the A2EventAction class
  A2EventAction *EA=const_cast<A2EventAction*>(static_cast<const A2EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction()));
  G4SDManager *SDman = G4SDManager::GetSDMpointer();
  EA->SetCBCollID(SDman->GetCollectionID(collectionName[0]));
}


G4bool A2VisCBSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{ 
 
  G4double edep = aStep->GetTotalEnergyDeposit();
  if ((edep/keV == 0.)) return false;      
  
  // This TouchableHistory is used to obtain the physical volume
  // of the hit
  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  
  //Get Crystal Volume copy number
  G4int crystal = theTouchable->GetVolume()->GetCopyNo();

  //Identify AcquRoot id of the crystal
  G4int id=fCrystalConvert[crystal];

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
    A2VisHit* Hit = new A2VisHit();
    //standard hit stuff
    Hit->AddEnergy(edep);
    Hit->SetPos(aStep->GetPreStepPoint()->GetPosition());
    Hit->SetID(id);
    Hit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    //visualisation hit stuff
    Hit->SetLogicalVolume(theTouchable->GetVolume()->GetLogicalVolume());
    Hit->SetCharge(charge);
    //insert hit
    fhitID[id] = fCollection->insert(Hit) -1;
    fHits[fNhits++]=id;
 
  }
  else // This is not new
    {
    (*fCollection)[fhitID[id]]->AddEnergy(edep);
    }
  return true;
}


void A2VisCBSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  if(HCID<0)
    { 
      HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }
  HCE->AddHitsCollection(HCID,fCollection);


  for (G4int i=0;i<fNhits;i++) 
    {
      fhitID[fHits[i]]=-1;
      fHits[i]=0;
    }
  fNhits=0;
}



void A2VisCBSD::clear()
{} 


void A2VisCBSD::DrawAll()
{} 


void A2VisCBSD::PrintAll()
{} 















