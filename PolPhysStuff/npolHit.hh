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
// $Id: ExN03CalorHit.hh,v 1.6 2003/05/28 10:08:52 gcosmo Exp $
// GEANT4 tag $Name: geant4-05-02-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef npolHit_h
#define npolHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class npolHit : public G4VHit
{
 public:

   npolHit();
   ~npolHit();
   npolHit(const npolHit&);
   const npolHit& operator=(const npolHit&);
   G4int operator==(const npolHit&) const;

   inline void* operator new(size_t);
   inline void  operator delete(void*);

   void Draw();
   void Print();
      
 public:
  
   void Addnpol(G4double de, G4double dl) {Edepnpol += de; TrackLengthnpol += dl;};
  void SetTimenpol(G4double time){Timenpol=time;}
  //void AddGap(G4double de, G4double dl) {EdepGap += de; TrackLengthGap += dl;};
                 
   G4double GetEdepnpol()     { return Edepnpol; };
   G4double GetTraknpol()     { return TrackLengthnpol; };
   G4double GetTimenpol()     { return Timenpol; };
     
  void SetPos(G4ThreeVector xyz)       {pos=xyz;};
  G4ThreeVector GetPos()               const {return pos;};

  //G4double GetEdepGap()     { return EdepGap; };
  // G4double GetTrakGap()     { return TrackLengthGap; };
    
 private:
  
   G4double Edepnpol, TrackLengthnpol, Timenpol;
   G4ThreeVector pos;
//G4double EdepGap, TrackLengthGap;     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<npolHit> npolHitsCollection;

extern G4Allocator<npolHit> npolHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* npolHit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) npolHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void npolHit::operator delete(void* aHit)
{
  npolHitAllocator.FreeSingle((npolHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
