
#ifndef A2DetPol_h
#define A2DetPol_h 1

#include "A2Detector.hh"
#include "A2SD.hh"

#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4UnionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"


class A2DetPol : public A2Detector
{
public:

  A2DetPol();
  ~A2DetPol();

  G4VPhysicalVolume* Construct(G4LogicalVolume *){return NULL;}
  G4VPhysicalVolume* Construct1(G4LogicalVolume *MotherLogic,G4double Z0);
  G4VPhysicalVolume* Construct2(G4LogicalVolume *MotherLogic,G4double Z0);
  G4VPhysicalVolume* Construct3(G4LogicalVolume *MotherLogic,G4double Z0);
  G4VPhysicalVolume* Construct4(G4LogicalVolume *MotherLogic,G4double Z0);
  G4VPhysicalVolume* Construct5(G4LogicalVolume *MotherLogic,G4double Z0);

  void MakeTube(); // Original
  void MakeTube2(); // 2016
  void MakeCap(); // Original
  void MakeCap2(); // 2016
  //void MakeSupports1(); //for Polarimeter 1
  //void MakeSupports2(); //for Polarimeter 2

private:

  G4Trap *fPolarimeter;  //Polarimeter scintillator shape
  G4LogicalVolume *fPolarimeterLogic;
  G4VPhysicalVolume** fPolarimeterPhysi;

  G4double Xoff;
  G4double Yoff;
  G4double fPol_Z;
  G4double fPol_rin;
  G4double fPol_rout;
  G4double fCap_rin;
  G4double fPol_Thick;
  G4double fPol_Z0;

} ;

#endif
