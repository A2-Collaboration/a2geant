#include "G4RunManager.hh"
#include "A2WCSD.hh"
#include "A2Hit.hh"
#include "A2EventAction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

#include "stdio.h"

A2WCSD::A2WCSD(G4String name,G4int Nelements):G4VSensitiveDetector(name)
{
  collectionName.insert(G4String("A2WCSDHits")+name);
  fCollection=NULL;

  fNelements=Nelements+1;//numbering starts from 1 not 0
  //fNelements=100;//now total number of hit positions stored
  fhitID=new G4int[fNelements];
  for(G4int i=0;i<fNelements;i++)fhitID[i]=-1;
  fHits=new G4int[fNelements];
  for(G4int i=0;i<fNelements;i++)fHits[i]=0;
 
  fNhits=0;
  fHCID=-1;
}


A2WCSD::~A2WCSD()
{

}


void A2WCSD::Initialize(G4HCofThisEvent*)
{
  //Hits collections are deleted in G4RunManager, DoEventLoop
  //This calls StackPreviousEvent which deletes the G4Event
  //This deletes the hit collection for the event and thus A2Hit s
  //G4cout<<"void A2WCSD::Initialize "<<SensitiveDetectorName<<G4endl;
  fCollection = new A2HitsCollection(SensitiveDetectorName,collectionName[0]);


}


G4bool A2WCSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{ 
  
  G4double edep = aStep->GetTotalEnergyDeposit();
  if ((edep/keV == 0.)) return false;      
  if (aStep->GetPreStepPoint()->GetGlobalTime()>2*ms)return false; 
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() == 0.) return false;
  // This TouchableHistory is used to obtain the physical volume
  // of the hit
  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  // G4Track* track = aStep->GetTrack();
  G4VPhysicalVolume* volume=theTouchable->GetVolume();
  //G4VPhysicalVolume* mothervolume=theTouchable->GetVolume(1);
  G4int id;
  //Get element copy number
  id = volume->GetCopyNo();

  //  G4cout<<volume->GetName()<<" id "<<id <<" edep "<<edep/MeV<<" "<<track->GetDefinition()->GetParticleName()<<" "<<track->GetParentID()<<G4endl;
  if(fNhits>fNelements){G4cout<<"Too many hits stored in G4bool A2WCSD::ProcessHits(G4Step* aStep,G4TouchableHistory*) need to increase fNelements from A2DetMWPC.cc if 100 is not enough!!!"<<G4endl;return false;}

  //check to see if this hit is close to a previous hit
  // e.g. ionised electrons should not create a new hit
  G4ThreeVector vhit=aStep->GetPreStepPoint()->GetPosition();
  G4int oldid=-1;
  for(G4int i=0;i<fNhits;i++){
    if((fabs((vhit-((*fCollection)[i]->GetPos())).r()))<3*mm) {oldid=i;}
    // G4cout<<" diff " <<i<<" "<<abs((vhit-((*fCollection)[i]->GetPos())).r())/mm<<G4endl;
  }
  if (oldid<0){
    //if this crystal has already had a hit
    //don't make a new one, add on to old one.   
    //  G4cout<<"Make hit "<<fNhits<<" "<<vhit<<G4endl;    
    A2Hit* Hit = new A2Hit;
    Hit->AddEnergy(edep);
    Hit->SetPos(aStep->GetPreStepPoint()->GetPosition());
    Hit->SetID(id);
    Hit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    fCollection->insert(Hit);
    fNhits++;
  }
  else // This is not new
    {
      //  G4cout<<"Add to prvoius hit "<<G4endl;
    (*fCollection)[oldid]->AddEnergy(edep);
    }
  //G4cout<<"done "<<fNhits<<G4endl;
  return true;
}


void A2WCSD::EndOfEvent(G4HCofThisEvent* HCE)
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
  //G4cout<<"EndOfEvent( done"<<G4endl;
}



void A2WCSD::clear()
{} 


void A2WCSD::DrawAll()
{} 


void A2WCSD::PrintAll()
{} 















