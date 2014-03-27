#include "G4RunManager.hh"
#include "A2CBSD.hh"
#include "A2Hit.hh"
#include "A2EventAction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "stdio.h"

A2CBSD::A2CBSD(G4String name):G4VSensitiveDetector(name)
{
  collectionName.insert("A2CBSDHits");
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
    G4cerr<<"Couldn't open CrystalConvert.in in A2CBSD::A2CBSD(G4String name)"<<G4endl;
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
  if(fCrystalConvert[719]!=621) {G4cerr<<"A2CBSD::A2CBSD Might have a problem with crystal convert array!!!"<<G4endl;exit(0);}

}


A2CBSD::~A2CBSD()
{

}


void A2CBSD::Initialize(G4HCofThisEvent*)
{

  // G4cout<<"void A2CBSD::Initialize"<<G4endl;
  fCollection = new A2HitsCollection(SensitiveDetectorName,collectionName[0]);

  //Set the hit collection id in the A2EventAction class
  A2EventAction *EA=const_cast<A2EventAction*>(static_cast<const A2EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction()));
  G4SDManager *SDman = G4SDManager::GetSDMpointer();
  EA->SetCBCollID(SDman->GetCollectionID(collectionName[0]));
}


G4bool A2CBSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
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

  // G4cout<<"id "<<id <<" edep "<<edep/MeV<<G4endl;
  if (fhitID[id]==-1){
    //if this crystal has already had a hit
    //don't make a new one, add on to old one.   
    //G4cout<<"Make hit "<<fCollection<<G4endl;    
    A2Hit* Hit = new A2Hit;
    Hit->AddEnergy(edep);
    Hit->SetPos(aStep->GetPreStepPoint()->GetPosition());
    Hit->SetID(id);
    Hit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    fhitID[id] = fCollection->insert(Hit) -1;
    fHits[fNhits++]=id;
  }
  else // This is not new
    {
    (*fCollection)[fhitID[id]]->AddEnergy(edep);
    }
  return true;
}


void A2CBSD::EndOfEvent(G4HCofThisEvent* HCE)
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



void A2CBSD::clear()
{} 


void A2CBSD::DrawAll()
{} 


void A2CBSD::PrintAll()
{} 















