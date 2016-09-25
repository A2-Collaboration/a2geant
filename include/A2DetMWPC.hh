#ifndef A2DetMWPC_h
#define A2DetMWPC_h 1

#include "A2Detector.hh"
#include "A2WCSD.hh"

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

class A2DetMWPC : public A2Detector {
public:

  A2DetMWPC();
  ~A2DetMWPC();

  void MakeInnerWire();
  void MakeOuterWire();
  void MakeChambers();
  void MakeSupports();
  void MakeDetector();
  void MakeSensitiveDetector();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);

  void UseAnodes(G4bool use=true){fuseAnodes=use;}
  
private:

//Chambers

  G4Tubs *fCHSI1;
  G4Tubs *fCHSO1;
  G4Tubs *fCHSI2;
  G4Tubs *fCHSO2;
  G4Tubs *fCHRI1;
  G4Tubs *fCHRO1;
  G4Tubs *fCHRI2;
  G4Tubs *fCHRO2;
  G4Tubs *fCHKI1;
  G4Tubs *fCHKO1;
  G4Tubs *fCHKI2;
  G4Tubs *fCHKO2;
  G4Tubs *fCHG1;
  G4Tubs *fCHG2;
  G4Tubs *fCHCI1;
  G4Tubs *fCHCO1;
  G4Tubs *fCHCI2;
  G4Tubs *fCHCO2;
 //  G4Tubs *fWS1U;
//   G4Tubs* fWS1D;
//   G4Tubs* fWS2U;
//   G4Tubs* fWS2D;
//   G4Cons* fSC1D;
//   G4Cons* fSC1U;
//   G4Tubs* fALTU;

  A2WCSD* fMWPCSD1;
  A2WCSD* fMWPCSD2;
  A2WCSD* fMWPCSD3;
  A2WCSD* fMWPCSD4;

  G4LogicalVolume *fCHSI1Logic;
  G4VPhysicalVolume *fCHSI1Physi;
  G4LogicalVolume *fCHSI2Logic;
  G4VPhysicalVolume *fCHSI2Physi;
  G4LogicalVolume *fCHSO1Logic;
  G4VPhysicalVolume *fCHSO1Physi;
  G4LogicalVolume *fCHSO2Logic;
  G4VPhysicalVolume *fCHSO2Physi;
  G4LogicalVolume *fCHRI1Logic;
  G4VPhysicalVolume *fCHRI1Physi;
  G4LogicalVolume *fCHRI2Logic;
  G4VPhysicalVolume *fCHRI2Physi;
  G4LogicalVolume *fCHRO1Logic;
  G4VPhysicalVolume *fCHRO1Physi;
  G4LogicalVolume *fCHRO2Logic;
  G4VPhysicalVolume *fCHRO2Physi;
  G4LogicalVolume *fCHKI1Logic;
  G4VPhysicalVolume *fCHKI1Physi;
  G4LogicalVolume *fCHKI2Logic;
  G4VPhysicalVolume *fCHKI2Physi;
  G4LogicalVolume *fCHKO1Logic;
  G4VPhysicalVolume *fCHKO1Physi;
  G4LogicalVolume *fCHKO2Logic;
  G4VPhysicalVolume *fCHKO2Physi;
  G4LogicalVolume *fCHG1Logic;
  G4VPhysicalVolume *fCHG1Physi;
  G4LogicalVolume *fCHG2Logic;
  G4VPhysicalVolume *fCHG2Physi;
  G4LogicalVolume *fCHCI1Logic;
  G4VPhysicalVolume *fCHCI1Physi;
  G4LogicalVolume *fCHCO1Logic;
  G4VPhysicalVolume *fCHCO1Physi;
  G4LogicalVolume *fCHCI2Logic;
  G4VPhysicalVolume *fCHCI2Physi;
  G4LogicalVolume *fCHCO2Logic;
  G4VPhysicalVolume *fCHCO2Physi;
  G4LogicalVolume *fWS1U2Logic;
  G4VPhysicalVolume *fWS1U2Physi;
  G4LogicalVolume *fWS1ULogic;
  G4VPhysicalVolume *fWS1UPhysi;
  G4LogicalVolume *fWS1DLogic;
  G4VPhysicalVolume *fWS1DPhysi;
  G4LogicalVolume *fWS2ULogic;
  G4VPhysicalVolume *fWS2UPhysi;
  G4LogicalVolume *fWS2DLogic;
  G4VPhysicalVolume *fWS2DPhysi;
  G4LogicalVolume *fSC1DLogic;
  G4VPhysicalVolume *fSC1DPhysi;
  G4LogicalVolume *fSC1ULogic;
  G4VPhysicalVolume *fSC1UPhysi;
  G4LogicalVolume *fALTULogic;
  G4VPhysicalVolume *fALTUPhysi;

//Anode Wires
  G4bool fuseAnodes;

  G4Tubs *fANOI;
  G4LogicalVolume* fANOILogic;
  G4VPhysicalVolume** fANOIPhysi;

  G4Tubs *fANOO;
  G4LogicalVolume* fANOOLogic;
  G4VPhysicalVolume** fANOOPhysi;


//Parameters
  G4int fNIWires;
  G4int fNOWires;

  G4double fz;
  G4double fdz;
  G4double fchsi1_rmin;
  G4double fchsi1_rmax;
  G4double fchso1_rmin;
  G4double fchso1_rmax;
  G4double fchri1_rmin;
  G4double fchri1_rmax;
  G4double fchro1_rmin;
  G4double fchro1_rmax;
  G4double fchki1_rmin;
  G4double fchki1_rmax;
  G4double fchko1_rmin;
  G4double fchko1_rmax;
  G4double fchg1_rmin;
  G4double fchg1_rmax;
  G4double fchsi2_rmin;
  G4double fchsi2_rmax;
  G4double fchso2_rmin;
  G4double fchso2_rmax;
  G4double fchri2_rmin;
  G4double fchri2_rmax;
  G4double fchro2_rmin;
  G4double fchro2_rmax;
  G4double fchki2_rmin;
  G4double fchki2_rmax;
  G4double fchko2_rmin;
  G4double fchko2_rmax;
  G4double fchg2_rmin;
  G4double fchg2_rmax;
  G4double fanoi_theta;
  G4double fanoo_theta;
  G4double fanode_rmin;
  G4double fanode_rmax;
  G4double fchci1_rmin;
  G4double fchci1_rmax;
  G4double fchco1_rmin;
  G4double fchco1_rmax;
  G4double fchci2_rmin;
  G4double fchci2_rmax;
  G4double fchco2_rmin;
  G4double fchco2_rmax;

  G4Region* fregionMWPC;
};
#endif
