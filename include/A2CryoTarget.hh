
#ifndef A2CryoTarget_h
#define A2CryoTarget_h 1

#include "A2Target.hh"

class A2CryoTarget: public  A2Target
{

public:
  A2CryoTarget();
  ~A2CryoTarget();

  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic, G4double Z0);
  virtual G4VPhysicalVolume* Construct1(G4LogicalVolume *MotherLogic, G4double Z0);
  virtual G4VPhysicalVolume* Construct2(G4LogicalVolume *MotherLogic, G4double Z0);
  void SetTargetLength(G4double Len){fTargetLength=Len;}

private:
 G4double fTargetLength;

};
#endif
