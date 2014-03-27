
#ifndef A2SteppingAction_h
#define A2SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class A2DetectorConstruction;
class A2EventAction;


class A2SteppingAction : public G4UserSteppingAction
{
  public:
    A2SteppingAction(A2DetectorConstruction*, A2EventAction*);
   ~A2SteppingAction();

    void UserSteppingAction(const G4Step*);
    
  private:
    A2DetectorConstruction* detector;
    A2EventAction*          eventaction;  
};


#endif
