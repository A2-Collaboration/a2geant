
#ifndef A2DetPID_h
#define A2DetPID_h 1

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


class A2DetPID : public A2Detector
{
public:
  
  A2DetPID();
  ~A2DetPID();

  G4VPhysicalVolume* Construct(G4LogicalVolume *){return NULL;}
  G4VPhysicalVolume* Construct1(G4LogicalVolume *MotherLogic,G4double Z0);
  G4VPhysicalVolume* Construct2(G4LogicalVolume *MotherLogic,G4double Z0);

  void MakeDetector();
  void MakeSingleDetector();
  void MakeLightGuide();
  void MakePhotomultipliers();
  void MakeSupports1(); //for PID1;
  void MakeSupports2(); //for PID2

private:
  G4int fNPids;  //Number of Pid scintillators
  G4double fZ0;

  A2SD* fPIDSD;

  G4Trap *fPID;  //PID scintillator shape
  G4LogicalVolume *fPIDLogic;
  G4VPhysicalVolume** fPIDPhysi;

  G4Trap *fLGFO;  //part of light guide
  G4Trap *fLGFI;  //part of light guide
  G4Trd *fLGFM;  //part of light guide
  G4Tubs *fLGTU;  //part of light guide
  G4UnionSolid *fLG1;  //part of light guide
  G4UnionSolid *fLG2;  //part of light guide
  G4UnionSolid *fLG3;  //part of light guide
  G4LogicalVolume *fLGLogic;
  G4VPhysicalVolume **fLGPhysi;
  G4LogicalVolume *fUPS1Logic;
  G4LogicalVolume *fUPS2Logic;
  G4LogicalVolume *fUPS3Logic;
  G4LogicalVolume *fPMTRLogic;
  G4LogicalVolume *fBRTULogic;
  
  G4Tubs *fTPMT;
  G4LogicalVolume *fTPMTLogic;
  G4VPhysicalVolume **fTPMTPhysi;

  G4Tubs *fBASE;
  G4LogicalVolume *fBASELogic;
  G4VPhysicalVolume **fBASEPhysi;

  G4Tubs *fMUME;
  G4LogicalVolume *fMUMELogic;
  G4VPhysicalVolume **fMUMEPhysi;
  //geometric parameters
  G4double fzpos;
  G4double fpid_z;
  G4double fpid_rin;
  G4double fpid_thick;
  G4double fpid_rout;
  G4double fpid_theta;
  G4double fpid_xs;
  G4double fpid_xl;
  G4double flg12_z;
  G4double flg_z;
  G4double fpmt_z;
  G4double fbase_z;
  G4double fmume_z;
  G4double fpmtr_z;
  G4Region* fregionPID;

} ;

#endif
