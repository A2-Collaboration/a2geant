
#ifndef A2DetCrystalBall_h
#define A2DetCrystalBall_h 1

#include "A2Detector.hh"
#include "A2SD.hh"
#include "A2VisSD.hh"

#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"


class A2DetCrystalBall : public A2Detector
{
public:
  
  A2DetCrystalBall();
  ~A2DetCrystalBall();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);

  void MakeCrystals();  //Make the different crystal shapes
  void MakeBall();  //Make the ball
   void MakeOther1();  //Make other non-sensitive materials associated with the ball
  void MakeOther2();  //Make other non-sensitive materials associated with the ball

  void MakeRotationMatrices();  //Use ROOT matrices to vcreate G4 rotations
  void MakeTransformVectors();  //Make the transform vectors using the ROOT AddNode parameters
  void SetGap(G4ThreeVector gap) {fGap=gap;}
private:
  G4int *fCrystalConvert;  //convert copy # to AcquRoot id
  G4int fNcrystals;

  G4int fNCrystTypes;   //Number of different crystal shapes
  G4Trap** fCrystal;    //Array of different crystal shapes
  G4LogicalVolume** fCrystLogic;  //Logical volume for crystals
  G4VPhysicalVolume** fCrystPhysi;  //Physical volumes for crystals

  // G4int fNMinorTypes;   //Number of different minor triangles
  // G4Trap* fMinorTrap;            //Trapezium shape of Minor triangles
  // G4Polyhedra* fMinorPolG;       //Polygon shape of Minor traingles
  // G4LogicalVolume** fMinorLogic;  //Logical volumes of Minor triangles
  // G4VPhysicalVolume** fMinorPhysi;  //Physical volumes of Minor triangles

  // G4int fNMajorTypes;   //Number of different major triangles
  // G4Polyhedra* fMajorPolG;       //Polygon shape of Major traingles
  // G4LogicalVolume** fMajorLogic;  //Logical volumes of Major triangles
  // G4VPhysicalVolume** fMajorPhysi;  //Physical volumes of Major triangles

//   G4Sphere* fHemisphere;       //Sphere for hemisphere
//   G4LogicalVolume** fHemiLogic;  //Logical volumes of hemispheres
//   G4VPhysicalVolume** fHemiPhysi;  //Physical volumes the hemispheres

  // G4Sphere* fBall;       //Sphere for ball
  // G4LogicalVolume* fBallLogic;  //Logical volumes of the  ball
  // G4VPhysicalVolume* fBallPhysi;  //Physical volumes the ball

  //Non-Sensitive volumes
  G4Sphere* fCNIN;   //Inner can
  G4LogicalVolume* fCNINLogic;
  G4VPhysicalVolume *fCNINPhysi[2]; //For upper and lower hemi

  G4Tubs* fCCUT;   //Volume that cut's crystals
  G4LogicalVolume* fCCUTLogic;
  G4VPhysicalVolume *fCCUTPhysi; 

  G4Tubs* fTUNL;   //iron tunnel
  G4LogicalVolume* fTUNLLogic;
  G4VPhysicalVolume *fTUNLPhysi[4]; 

  G4Tubs* fBTMM;   //bottom of ball
  G4LogicalVolume* fBTMMLogic;
  G4VPhysicalVolume *fBTMMPhysi[4]; 

  G4Tubs* fRIMS;   //RIMS
  G4LogicalVolume* fRIMSLogic;
  G4VPhysicalVolume *fRIMSPhysi[2]; 

  G4Trd* fSKI[3];  //Skirts
  G4LogicalVolume* fSKILogic[3];
  G4VPhysicalVolume* fSKIPhysi[12];

  // G4Cons* fPIDW;   //PID wires
  // G4LogicalVolume* fPIDWLogic;
  // G4VPhysicalVolume *fPIDWPhysi; 

  // G4Cons* fMWPW;   //MWP wires
  // G4LogicalVolume* fMWPWLogic;
  // G4VPhysicalVolume *fMWPWPhysi; 

  G4RotationMatrix** fRot;   //List of all rotation matrixes from ugeom.F converted to G4
  G4ThreeVector* fTrans;     // And the transformation vectors from ugeom.F converted to G4

  G4ThreeVector fGap; //x component is up, y is down, z is not used yet 

  A2SD* fCBSD;
  A2VisSD* fVisCBSD;

  G4VisAttributes* fCrystVisAtt;
  G4Region* fregionCB;
} ;

#endif
