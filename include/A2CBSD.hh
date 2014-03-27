
#ifndef A2CBSD_h
#define A2CBSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4HCofThisEvent;
class G4Step;

#include "A2Hit.hh"

class A2CBSD : public G4VSensitiveDetector
{
public:
  
  A2CBSD(G4String);
  ~A2CBSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* astep,G4TouchableHistory* ROHist);
  void EndOfEvent(G4HCofThisEvent*);
  void clear();
  void DrawAll();
  void PrintAll();
  
private:
  
  A2HitsCollection*  fCollection;  

  G4int fNcrystals;
  G4int * fhitID;
  G4int * fHits;
  G4int *fCrystalConvert;
  G4int fNhits;

 
};

#endif






