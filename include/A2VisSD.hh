
#ifndef A2VisSD_h
#define A2VisSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4HCofThisEvent;
class G4Step;

#include "A2VisHit.hh"

class A2VisSD : public G4VSensitiveDetector
{
public:
  
  A2VisSD(G4String,G4int);
  ~A2VisSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* astep,G4TouchableHistory* ROHist);
  void EndOfEvent(G4HCofThisEvent*);
  void clear();
  void DrawAll();
  void PrintAll();
  
private:
  
  A2VisHitsCollection*  fCollection;  

  G4int fHCID;

  G4int fNelements;
  G4int * fhitID;
  G4int * fHits;
  G4int fNhits;

 
};

#endif






