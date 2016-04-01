
#ifndef npolSteppingAction_h
#define npolSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class npolDetectorConstruction;
class npolEventAction;


class npolSteppingAction : public G4UserSteppingAction
{
  public:
    npolSteppingAction(npolDetectorConstruction*, npolEventAction*);
   ~npolSteppingAction();

    void UserSteppingAction(const G4Step*);
    
  private:
    npolDetectorConstruction* detector;
    npolEventAction*          eventaction;  
  bool fInScatYet;
  bool fNeutYet;
  bool fProtYet;
};


#endif
