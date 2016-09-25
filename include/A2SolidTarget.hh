
#ifndef A2SolidTarget_h
#define A2SolidTarget_h 1

#include "A2Target.hh"

class A2SolidTarget: public  A2Target
{

public:
  A2SolidTarget();
  ~A2SolidTarget();

  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);

};
#endif
