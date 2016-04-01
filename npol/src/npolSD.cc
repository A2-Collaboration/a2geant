//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: ExN03CalorimeterSD.cc,v 1.7 2003/05/28 10:08:53 gcosmo Exp $
// GEANT4 tag $Name: geant4-05-02-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "npolSD.hh"

#include "npolHit.hh"
#include "npolDetCon.hh"
#include "npolSteppingAction.hh"

//#include "G4VPhysicalVolume.hh"
#include "G4PVReplica.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolSD::npolSD(G4String name,
                                   npolDetectorConstruction* det)
:G4VSensitiveDetector(name),Detector(det)
{
  collectionName.insert("npolCollection");
  HitID = new G4int[1];
  // HitID = new G4int[Detector->GetNbOfnpol()];
    verboseLevel =0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

npolSD::~npolSD()
{
  delete [] HitID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolSD::Initialize(G4HCofThisEvent*)
{
  npolCollection = new npolHitsCollection
                      (SensitiveDetectorName,collectionName[0]); 
  //  for (G4int j=0;j<Detector->GetNbOfnpol();j++) {HitID[j] = -1;};
  for (G4int j=0;j<1;j++) {HitID[j] = -1;};
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool npolSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4RunManager * runMan = G4RunManager::GetRunManager();
  npolSteppingAction* ssa=(npolSteppingAction*)runMan->GetUserSteppingAction();
  // G4VProcess* curProcess=ssa->GetCurrentProcess();
  G4Track* aTrack = aStep->GetTrack();

  //must get edep first to make sure it is a real hit
  G4double time = aTrack->GetGlobalTime();

  G4double edep = aStep->GetTotalEnergyDeposit();
  //if(verboseLevel>1) G4cout<< "Particle "<<aStep->GetTrack()->GetDefinition()->GetParticleName()<<" Process "<<curProcess->GetProcessName()<<" Kinetic Energy "<<aStep->GetTrack()->GetKineticEnergy()/keV<< G4endl;

  G4double stepl = 0.;
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
      stepl = aStep->GetStepLength();
      
  if ((edep==0.)&&(stepl==0.)) return false;    
  if (aStep->GetTrack()->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    
  G4VPhysicalVolume* physVol = theTouchable->GetVolume(); 
  //theTouchable->MoveUpHistory();
  G4int npolNumber = 0;
  //if(Detector->GetNbOfnpol()>1) npolNumber= physVol->GetCopyNo();

  if (HitID[npolNumber]==-1)
    { 
      npolHit* sciHit = new npolHit();
      //  if (physVol == Detector->GetnpolPhys(npolNumber))
	sciHit->Addnpol(edep,stepl);
 	sciHit->SetTimenpol(time);
        sciHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
	HitID[npolNumber] = npolCollection->insert(sciHit) - 1;
       if (verboseLevel>0)
        G4cout << " New npol Hit on bar: " << npolNumber << G4endl;
    }
  else
    { 
      // if (physVol == Detector->GetnpolPhys(npolNumber))
         (*npolCollection)[HitID[npolNumber]]->Addnpol(edep,stepl);
      //if (physVol == Detector->GetGap())
      // (*CalCollection)[HitID[LayerNumber]]->AddGap(edep,stepl);
      if (verboseLevel>0)
        G4cout << " Energy added to npol: " <<edep << G4endl; 
    }
    
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,npolCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolSD::clear()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolSD::DrawAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void npolSD::PrintAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

