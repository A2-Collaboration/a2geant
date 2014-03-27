
#ifndef A2RunAction_h
#define A2RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "A2EventAction.hh"

class G4Run;

class A2RunAction : public G4UserRunAction
{
  public:
    A2RunAction();
   ~A2RunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void   EndOfRunAction(const G4Run*);
    
 
  private:
  A2EventAction *fEventAction;
};

#endif

