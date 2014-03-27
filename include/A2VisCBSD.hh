
#ifndef A2VisCBSD_h
#define A2VisCBSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4HCofThisEvent;
class G4Step;

#include "A2VisHit.hh"

class A2VisCBSD : public G4VSensitiveDetector
{
public:
  
  A2VisCBSD(G4String);
  ~A2VisCBSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* astep,G4TouchableHistory* ROHist);
  void EndOfEvent(G4HCofThisEvent*);
  void clear();
  void DrawAll();
  void PrintAll();
  
private:
  
  A2VisHitsCollection*  fCollection;  

  G4int fNcrystals;
  G4int * fhitID;
  G4int * fHits;
  G4int *fCrystalConvert;
  G4int fNhits;

 
};

#endif






