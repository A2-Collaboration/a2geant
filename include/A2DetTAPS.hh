
#ifndef A2DetTAPS_h
#define A2DetTAPS_h 1

#include "A2Detector.hh"
#include "A2SD.hh"
#include "A2VisSD.hh"

#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"


class A2DetTAPS : public A2Detector
{
public:
  
  A2DetTAPS();
  A2DetTAPS(G4String setupfile, G4int Ntaps, G4int NPbWORings,G4double tZ);

  ~A2DetTAPS();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);

  void MakeCrystals();
  void PlaceCrystals();
  void MakeVeto();
  void MakeForwardWallMother();
private:
  G4int fNTaps;  //Total capacity of TAPS wall
  G4int fNRealTaps; //Number of BaF2 crystals placed in wall
  G4double fZ0;
  G4String fSetupFile; //taps configuration file
  G4int fNPbWO4;
  G4int fNPbWORings;

  A2SD* fTAPSSD;
  A2SD* fTAPSVSD;
  A2VisSD* fTAPSVisSD;
  A2VisSD* fTAPSVVisSD;

  G4Polyhedra *fCOVR;
  G4LogicalVolume *fCOVRLogic;
  G4VPhysicalVolume** fCOVRPhysi;
  G4LogicalVolume *fCOVRPbLogic;
  G4VPhysicalVolume** fCOVRPbPhysi;

  G4Polyhedra *fDUMM;
  G4LogicalVolume *fDUMMLogic;
  G4VPhysicalVolume** fDUMMPhysi;

  G4Polyhedra *fTAPS;
  G4LogicalVolume *fTAPSLogic;
  G4VPhysicalVolume* fTAPSPhysi;

  G4SubtractionSolid *fPbWO;
  G4LogicalVolume *fPbWOLogic;
  G4VPhysicalVolume* fPbWOPhysi;

  G4Tubs *fTEND;
  G4LogicalVolume *fTENDLogic;
  G4VPhysicalVolume* fTENDPhysi;

  G4Tubs *fAIRC;
  G4LogicalVolume *fAIRCLogic;
  G4VPhysicalVolume* fAIRCPhysi;

  G4Polyhedra *fABOX;
  G4LogicalVolume *fABOXLogic;
  G4VPhysicalVolume* fABOXPhysi;

  G4Polyhedra *fFWMV;
  G4LogicalVolume *fFWMVLogic;
  G4VPhysicalVolume* fFWMVPhysi;

  G4Polyhedra *fTVET;
  G4LogicalVolume *fTVETLogic;
  G4VPhysicalVolume** fTVETPhysi;

  G4Polyhedra *fVDB1;
  G4LogicalVolume *fVDB1Logic;
  G4VPhysicalVolume* fVDB1Physi;

  G4Polyhedra *fVDB2;
  G4LogicalVolume *fVDB2Logic;
  G4VPhysicalVolume* fVDB2Physi;

  G4Region* fregionTAPS;
  G4Region* fregionTAPSV;
} ;

#endif
