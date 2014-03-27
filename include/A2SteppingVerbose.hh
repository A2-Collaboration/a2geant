
class A2SteppingVerbose;

#ifndef A2SteppingVerbose_h
#define A2SteppingVerbose_h 1

#include "G4SteppingVerbose.hh"


class A2SteppingVerbose : public G4SteppingVerbose
{
 public:   

   A2SteppingVerbose();
  ~A2SteppingVerbose();

   void StepInfo();
   void TrackingStarted();

};

#endif
