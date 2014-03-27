
#ifndef A2WCSD_h
#define A2WCSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4HCofThisEvent;
class G4Step;

#include "A2Hit.hh"

class A2WCSD : public G4VSensitiveDetector
{
public:
  
  A2WCSD(G4String,G4int);
  ~A2WCSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* astep,G4TouchableHistory* ROHist);
  void EndOfEvent(G4HCofThisEvent*);
  void clear();
  void DrawAll();
  void PrintAll();
  
private:
  
  A2HitsCollection*  fCollection;  
  G4int fHCID;

  G4int fNelements;
  G4int * fhitID;
  G4int * fHits;
  G4int fNhits;

 
};

#endif






