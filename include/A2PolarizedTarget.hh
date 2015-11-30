
#ifndef A2PolarizedTarget_h
#define A2PolarizedTarget_h 1

#include "A2Target.hh"
#include "A2MagneticField.hh"

class A2PolarizedTarget: public  A2Target
{

public:
  A2PolarizedTarget();
  ~A2PolarizedTarget();

  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic, G4double Z0);
  virtual G4VPhysicalVolume* Construct1(G4LogicalVolume *MotherLogic, G4double Z0);
  virtual G4VPhysicalVolume* Construct2(G4LogicalVolume *MotherLogic, G4double Z0);

  // Set magnetic field according to the field map
  virtual void SetMagneticField(G4String&);

  // Set magnetic coils type (solenoidal/saddle)
  virtual void SetMagneticCoils(G4String &type) { fTypeMagneticCoils = type; }

private:
  A2MagneticField* fMagneticField;
  G4String fTypeMagneticCoils;

};
#endif
