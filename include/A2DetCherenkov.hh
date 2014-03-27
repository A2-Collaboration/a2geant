
#ifndef A2DetCherenkov_h
#define A2DetCherenkov_h 1

#include "A2Detector.hh"

class A2DetCherenkov : public A2Detector
{
public:
  
  A2DetCherenkov();
  ~A2DetCherenkov();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);

};

#endif
