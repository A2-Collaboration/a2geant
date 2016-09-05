#include "A2CryoTarget.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

A2CryoTarget::A2CryoTarget()
{
  fTargetLength=5*CLHEP::cm;
}
A2CryoTarget::~A2CryoTarget()
{

}

// Default construction is old one if nothing specified

G4VPhysicalVolume* A2CryoTarget::Construct(G4LogicalVolume *MotherLogic, G4double Z0){
  fMotherLogic=MotherLogic;
  //c Target volume parameters
  //All parameters copied directly from ugeom_target.F
  G4double l_trgt = (14.63 + 4 + 9.721)*CLHEP::cm; //! See target manual
  G4double r_trgt = 4.1*CLHEP::cm; // NB 4.1cm is the standard target length! Add in an option for this new smaller 3.1cm target later
  //c Outer flange parameters
  G4double l_flangeo = 0.26*CLHEP::cm;
  G4double t_flangeo = 1.1*CLHEP::cm;
  G4double r_flangeo = r_trgt;
  //c CFK tube parameters
  G4double l_CFK = l_trgt - l_flangeo;//  ! Consider the outer flange
  G4double r_CFK = r_trgt;
  G4double t_CFK = 0.1*CLHEP::cm;
  //c Inner flange parameters
  G4double l_flangei = 0.4*CLHEP::cm;
  G4double t_flangei = 0.1*CLHEP::cm;
  G4double r_flangei = r_CFK - t_CFK;
  //c Parameters for the copper holding tube
  G4double r_CU64 = 1.3*CLHEP::cm;
  G4double t_CU64 = 0.1*CLHEP::cm;
  //G4double l_CU64 = 16.6*CLHEP::cm - (r_CU64/tan(31.*CLHEP::deg));// ! See target manual
  //G4double l_CU64 = 14.1*CLHEP::cm - 5.0*CLHEP::cm; // for 10-CLHEP::cm target, distance from back to middle of cell - distance between middle of cell and copper tube
  G4double l_CU64 = 19.0*CLHEP::cm - fTargetLength;
  //c Parameters for the spherical cell part upstream
  G4double t_lD2C = 0.14*CLHEP::cm;//    ! Deformation of window = Thickness of cell part
  G4double r_lD2C = ((r_CU64-t_CU64)*(r_CU64-t_CU64)-t_lD2C*t_lD2C)/(2*t_lD2C)+t_lD2C;
  //c Parameters for the other cell parts
  G4double r_lD2B = 4.543*CLHEP::cm;   //! See target manual
  G4double r_lD2A = 2.*CLHEP::cm;
  //G4double l_cell = 4.76*CLHEP::cm ;  // ! Length of cell at beam axis
  //G4double l_cell = 10.0*CLHEP::cm ;  // For 10-CLHEP::cm cell
  G4double l_cell = fTargetLength;
  fLength=l_cell;
  fRadius=r_lD2A;
  G4double t_lD2B = r_lD2B - sqrt(r_lD2B*r_lD2B-r_lD2A*r_lD2A);
  G4double l_lD2A = l_cell - t_lD2C - t_lD2B;
 //c Parameters for the tmat supply tube
  G4double r_lD2D = r_lD2A;
  G4double t_lD2D = r_lD2A - r_CU64;
  G4double l_lD2D = l_CU64;
  //c Parameters for the kapton exit window
  G4double r_kapd = r_lD2B;
  G4double t_KAPT = 0.0125*CLHEP::cm;//   ! Valid for all kapton parts

  G4double tm = -l_trgt/2. + l_CU64 + l_cell/2.;// ! Middle of target cell

  //Construct the volumes
  /////////////////////////
  //Target Mother Volume
  ////////////////////////
  if(!fMaterial){
    G4cerr<<"A2CryoTarget:Construct Target Material not defined!"<<G4endl;
    exit(1);
    //fMaterial=fNistManager->FindOrBuildMaterial("LH2");
  }
  G4Tubs* MyShape=new G4Tubs("TRGT",0.,r_trgt+0.1*CLHEP::mm,l_trgt/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(MyShape,fNistManager->FindOrBuildMaterial("G4_AIR"),"TRGT");
  fMyPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,Z0-tm+0.25*CLHEP::cm),fMyLogic,"TRGT",fMotherLogic,false,1);
  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);
  //////////////////////
  //CFK tube + window
  /////////////////////
  G4Tubs* CFKT=new G4Tubs("CFKT",r_CFK-t_CFK,r_CFK,l_CFK/2,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CFKTLogic=new G4LogicalVolume(CFKT,fNistManager->FindOrBuildMaterial("G4_GRAPHITE"),"CFKT");
  new G4PVPlacement(0,G4ThreeVector(0,0,- l_flangeo/2.),CFKTLogic,"CFKT",fMyLogic,false,1);
  G4VisAttributes* TarVisAtt= new G4VisAttributes(G4Colour(.0,.0,1.0));
  TarVisAtt->SetForceWireframe(true);
  CFKTLogic->SetVisAttributes(TarVisAtt);

  G4Tubs* FLA1=new G4Tubs("FLA1",r_flangeo - t_flangeo,r_flangeo,l_flangeo/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA1Logic=new G4LogicalVolume(FLA1,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA1");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo/2.),FLA1Logic,"FLA1",fMyLogic,false,2);

  G4Tubs* FLA2=new G4Tubs("FLA2",r_flangei - t_flangei,r_flangei,l_flangei/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA2Logic=new G4LogicalVolume(FLA2,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA2");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo - l_flangei/2.),FLA2Logic,"FLA2",fMyLogic,false,3);

  //Note sphere constructor, ("name"rmin,rmax,phimin,deltaphi,thetamin,deltatheta)
  G4Sphere* KAPD=new G4Sphere("KAPD",r_kapd,r_kapd+t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg - asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad,asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad);
  G4LogicalVolume* KAPDLogic=new G4LogicalVolume(KAPD,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPD");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. + sqrt(r_kapd*r_kapd-(r_flangeo-t_flangeo)*(r_flangeo-t_flangeo))),KAPDLogic,"KAPD",fMyLogic,false,4);

  //////////////////////
  //Kapton tube+windows
  ////////////////////////
  G4Tubs* KAPA=new G4Tubs("KAPA",r_lD2A,r_lD2A + t_KAPT,(l_CU64 + l_lD2A)/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* KAPALogic=new G4LogicalVolume(KAPA,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPA");
  new G4PVPlacement(0,G4ThreeVector(0,0,(l_CU64 + l_lD2A - l_trgt)/2.),KAPALogic,"KAPA",fMyLogic,false,5);
  KAPALogic->SetVisAttributes(TarVisAtt);

  G4Sphere* KAPB=new G4Sphere("KAPB",r_lD2B,r_lD2B + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,0,asin((r_lD2A+t_KAPT)/(r_lD2B+t_KAPT))*CLHEP::rad);
  G4LogicalVolume* KAPBLogic=new G4LogicalVolume(KAPB,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPB");
 new G4PVPlacement(0,G4ThreeVector(0,0,- l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),KAPBLogic,"KAPB",fMyLogic,false,6);


  G4Sphere* KAPC=new G4Sphere("KAPC",r_lD2C,r_lD2C + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
  G4LogicalVolume* KAPCLogic=new G4LogicalVolume(KAPC,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPC");
  new G4PVPlacement(0,G4ThreeVector(0,0,  - l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),KAPCLogic,"KAPC",fMyLogic,false,7);

  ////////////////////////////////
  //LH2 or LD2
  ///////////////////////////////
    G4Tubs* LD2A=new G4Tubs("LD2A",0,r_lD2A,l_lD2A/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2ALogic=new G4LogicalVolume(LD2A,fMaterial,"LD2A");
    new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64 + (l_lD2A - l_trgt)/2.),LD2ALogic,"LD2A",fMyLogic,false,8);

    G4Sphere* LD2B=new G4Sphere("LD2B",r_lD2B - t_lD2B,r_lD2B,0*CLHEP::deg,360*CLHEP::deg,0,asin(r_lD2A/r_lD2B)*CLHEP::rad);
    G4LogicalVolume* LD2BLogic=new G4LogicalVolume(LD2B,fMaterial,"LD2B");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),LD2BLogic,"LD2B",fMyLogic,false,9);
    G4cout<<"Target centre "<<(-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B)/CLHEP::cm <<" "<<l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm<<G4endl;
    fCenter.set(0,0,l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm);//position of cell in world volume
    G4Sphere* LD2C=new G4Sphere("LD2C",r_lD2C - t_lD2C,r_lD2C,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
    G4LogicalVolume* LD2CLogic=new G4LogicalVolume(LD2C,fMaterial,"LD2C");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),LD2CLogic,"LD2C",fMyLogic,false,10);

    G4Tubs* LD2D=new G4Tubs("LD2D",r_lD2D - t_lD2D,r_lD2D,l_lD2D/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2DLogic=new G4LogicalVolume(LD2D,fMaterial,"LD2D");
    new G4PVPlacement(0,G4ThreeVector(0,0,+l_lD2D/2. - l_trgt/2.),LD2DLogic,"LD2D",fMyLogic,false,11);

  G4VisAttributes* LD2VisAtt= new G4VisAttributes(G4Colour(1.0,.0,0.0));
  LD2VisAtt->SetForceSolid(true);
  LD2ALogic->SetVisAttributes(LD2VisAtt);
  LD2BLogic->SetVisAttributes(LD2VisAtt);
  LD2CLogic->SetVisAttributes(LD2VisAtt);
  LD2DLogic->SetVisAttributes(TarVisAtt);

  //////////////////////////////
  //Copper tube
  ////////////////////////////
  G4Tubs* CU64=new G4Tubs("CU64",r_CU64 - t_CU64,r_CU64,l_CU64/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CU64Logic=new G4LogicalVolume(CU64,fNistManager->FindOrBuildMaterial("G4_Cu"),"CU64");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64/2. - l_trgt/2.),CU64Logic,"CU64",fMyLogic,false,12);

  return fMyPhysi;

}

G4VPhysicalVolume* A2CryoTarget::Construct1(G4LogicalVolume *MotherLogic, G4double Z0){
  fMotherLogic=MotherLogic;
  //c Target volume parameters
  //All parameters copied directly from ugeom_target.F
  G4double l_trgt = (14.63 + 4 + 9.721)*CLHEP::cm; //! See target manual
  G4double r_trgt = 4.1*CLHEP::cm; // NB 4.1cm is the standard target length! Add in an option for this new smaller 3.1cm target later
  //c Outer flange parameters
  G4double l_flangeo = 0.26*CLHEP::cm;
  G4double t_flangeo = 1.1*CLHEP::cm;
  G4double r_flangeo = r_trgt;
  //c CFK tube parameters
  G4double l_CFK = l_trgt - l_flangeo;//  ! Consider the outer flange
  G4double r_CFK = r_trgt;
  G4double t_CFK = 0.1*CLHEP::cm;
  //c Inner flange parameters
  G4double l_flangei = 0.4*CLHEP::cm;
  G4double t_flangei = 0.1*CLHEP::cm;
  G4double r_flangei = r_CFK - t_CFK;
  //c Parameters for the copper holding tube
  G4double r_CU64 = 1.3*CLHEP::cm;
  G4double t_CU64 = 0.1*CLHEP::cm;
  //G4double l_CU64 = 16.6*CLHEP::cm - (r_CU64/tan(31.*CLHEP::deg));// ! See target manual
  //G4double l_CU64 = 14.1*CLHEP::cm - 5.0*CLHEP::cm; // for 10-CLHEP::cm target, distance from back to middle of cell - distance between middle of cell and copper tube
  G4double l_CU64 = 19.0*CLHEP::cm - fTargetLength;
  //c Parameters for the spherical cell part upstream
  G4double t_lD2C = 0.14*CLHEP::cm;//    ! Deformation of window = Thickness of cell part
  G4double r_lD2C = ((r_CU64-t_CU64)*(r_CU64-t_CU64)-t_lD2C*t_lD2C)/(2*t_lD2C)+t_lD2C;
  //c Parameters for the other cell parts
  G4double r_lD2B = 4.543*CLHEP::cm;   //! See target manual
  G4double r_lD2A = 2.*CLHEP::cm;
  //G4double l_cell = 4.76*CLHEP::cm ;  // ! Length of cell at beam axis
  //G4double l_cell = 10.0*CLHEP::cm ;  // For 10-CLHEP::cm cell
  G4double l_cell = fTargetLength;
  fLength=l_cell;
  fRadius=r_lD2A;
  G4double t_lD2B = r_lD2B - sqrt(r_lD2B*r_lD2B-r_lD2A*r_lD2A);
  G4double l_lD2A = l_cell - t_lD2C - t_lD2B;
 //c Parameters for the tmat supply tube
  G4double r_lD2D = r_lD2A;
  G4double t_lD2D = r_lD2A - r_CU64;
  G4double l_lD2D = l_CU64;
  //c Parameters for the kapton exit window
  G4double r_kapd = r_lD2B;
  G4double t_KAPT = 0.0125*CLHEP::cm;//   ! Valid for all kapton parts

  G4double tm = -l_trgt/2. + l_CU64 + l_cell/2.;// ! Middle of target cell

  //Construct the volumes
  /////////////////////////
  //Target Mother Volume
  ////////////////////////
  if(!fMaterial){
    G4cerr<<"A2CryoTarget:Construct Target Material not defined!"<<G4endl;
    exit(1);
    //fMaterial=fNistManager->FindOrBuildMaterial("LH2");
  }
  G4Tubs* MyShape=new G4Tubs("TRGT",0.,r_trgt+0.1*CLHEP::mm,l_trgt/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(MyShape,fNistManager->FindOrBuildMaterial("G4_AIR"),"TRGT");
  fMyPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,Z0-tm+0.25*CLHEP::cm),fMyLogic,"TRGT",fMotherLogic,false,1);
  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);
  //////////////////////
  //CFK tube + window
  /////////////////////
  G4Tubs* CFKT=new G4Tubs("CFKT",r_CFK-t_CFK,r_CFK,l_CFK/2,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CFKTLogic=new G4LogicalVolume(CFKT,fNistManager->FindOrBuildMaterial("G4_GRAPHITE"),"CFKT");
  new G4PVPlacement(0,G4ThreeVector(0,0,- l_flangeo/2.),CFKTLogic,"CFKT",fMyLogic,false,1);
  G4VisAttributes* TarVisAtt= new G4VisAttributes(G4Colour(.0,.0,1.0));
  TarVisAtt->SetForceWireframe(true);
  CFKTLogic->SetVisAttributes(TarVisAtt);

  G4Tubs* FLA1=new G4Tubs("FLA1",r_flangeo - t_flangeo,r_flangeo,l_flangeo/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA1Logic=new G4LogicalVolume(FLA1,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA1");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo/2.),FLA1Logic,"FLA1",fMyLogic,false,2);

  G4Tubs* FLA2=new G4Tubs("FLA2",r_flangei - t_flangei,r_flangei,l_flangei/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA2Logic=new G4LogicalVolume(FLA2,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA2");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo - l_flangei/2.),FLA2Logic,"FLA2",fMyLogic,false,3);

  //Note sphere constructor, ("name"rmin,rmax,phimin,deltaphi,thetamin,deltatheta)
  G4Sphere* KAPD=new G4Sphere("KAPD",r_kapd,r_kapd+t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg - asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad,asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad);
  G4LogicalVolume* KAPDLogic=new G4LogicalVolume(KAPD,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPD");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. + sqrt(r_kapd*r_kapd-(r_flangeo-t_flangeo)*(r_flangeo-t_flangeo))),KAPDLogic,"KAPD",fMyLogic,false,4);

  //////////////////////
  //Kapton tube+windows
  ////////////////////////
  G4Tubs* KAPA=new G4Tubs("KAPA",r_lD2A,r_lD2A + t_KAPT,(l_CU64 + l_lD2A)/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* KAPALogic=new G4LogicalVolume(KAPA,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPA");
  new G4PVPlacement(0,G4ThreeVector(0,0,(l_CU64 + l_lD2A - l_trgt)/2.),KAPALogic,"KAPA",fMyLogic,false,5);
  KAPALogic->SetVisAttributes(TarVisAtt);

  G4Sphere* KAPB=new G4Sphere("KAPB",r_lD2B,r_lD2B + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,0,asin((r_lD2A+t_KAPT)/(r_lD2B+t_KAPT))*CLHEP::rad);
  G4LogicalVolume* KAPBLogic=new G4LogicalVolume(KAPB,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPB");
 new G4PVPlacement(0,G4ThreeVector(0,0,- l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),KAPBLogic,"KAPB",fMyLogic,false,6);


  G4Sphere* KAPC=new G4Sphere("KAPC",r_lD2C,r_lD2C + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
  G4LogicalVolume* KAPCLogic=new G4LogicalVolume(KAPC,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPC");
  new G4PVPlacement(0,G4ThreeVector(0,0,  - l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),KAPCLogic,"KAPC",fMyLogic,false,7);

  ////////////////////////////////
  //LH2 or LD2
  ///////////////////////////////
    G4Tubs* LD2A=new G4Tubs("LD2A",0,r_lD2A,l_lD2A/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2ALogic=new G4LogicalVolume(LD2A,fMaterial,"LD2A");
    new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64 + (l_lD2A - l_trgt)/2.),LD2ALogic,"LD2A",fMyLogic,false,8);

    G4Sphere* LD2B=new G4Sphere("LD2B",r_lD2B - t_lD2B,r_lD2B,0*CLHEP::deg,360*CLHEP::deg,0,asin(r_lD2A/r_lD2B)*CLHEP::rad);
    G4LogicalVolume* LD2BLogic=new G4LogicalVolume(LD2B,fMaterial,"LD2B");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),LD2BLogic,"LD2B",fMyLogic,false,9);
    G4cout<<"Target centre "<<(-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B)/CLHEP::cm <<" "<<l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm<<G4endl;
    fCenter.set(0,0,l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm);//position of cell in world volume
    G4Sphere* LD2C=new G4Sphere("LD2C",r_lD2C - t_lD2C,r_lD2C,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
    G4LogicalVolume* LD2CLogic=new G4LogicalVolume(LD2C,fMaterial,"LD2C");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),LD2CLogic,"LD2C",fMyLogic,false,10);

    G4Tubs* LD2D=new G4Tubs("LD2D",r_lD2D - t_lD2D,r_lD2D,l_lD2D/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2DLogic=new G4LogicalVolume(LD2D,fMaterial,"LD2D");
    new G4PVPlacement(0,G4ThreeVector(0,0,+l_lD2D/2. - l_trgt/2.),LD2DLogic,"LD2D",fMyLogic,false,11);

  G4VisAttributes* LD2VisAtt= new G4VisAttributes(G4Colour(1.0,.0,0.0));
  LD2VisAtt->SetForceSolid(true);
  LD2ALogic->SetVisAttributes(LD2VisAtt);
  LD2BLogic->SetVisAttributes(LD2VisAtt);
  LD2CLogic->SetVisAttributes(LD2VisAtt);
  LD2DLogic->SetVisAttributes(TarVisAtt);

  //////////////////////////////
  //Copper tube
  ////////////////////////////
  G4Tubs* CU64=new G4Tubs("CU64",r_CU64 - t_CU64,r_CU64,l_CU64/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CU64Logic=new G4LogicalVolume(CU64,fNistManager->FindOrBuildMaterial("G4_Cu"),"CU64");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64/2. - l_trgt/2.),CU64Logic,"CU64",fMyLogic,false,12);

  return fMyPhysi;

}

G4VPhysicalVolume* A2CryoTarget::Construct2(G4LogicalVolume *MotherLogic, G4double Z0){
  fMotherLogic=MotherLogic;
  //c Target volume parameters
  //All parameters copied directly from ugeom_target.F
  G4double l_trgt = (14.63 + 4 + 9.721)*CLHEP::cm; //! See target manual
  G4double r_trgt = 3.1*CLHEP::cm; // NB 4.1cm is the standard target length! Add in an option for this new smaller 3.1cm target later
  //c Outer flange parameters
  G4double l_flangeo = 0.26*CLHEP::cm;
  G4double t_flangeo = 1.1*CLHEP::cm;
  G4double r_flangeo = r_trgt;
  //c CFK tube parameters
  G4double l_CFK = l_trgt - l_flangeo;//  ! Consider the outer flange
  G4double r_CFK = r_trgt;
  G4double t_CFK = 0.1*CLHEP::cm;
  //c Inner flange parameters
  G4double l_flangei = 0.4*CLHEP::cm;
  G4double t_flangei = 0.1*CLHEP::cm;
  G4double r_flangei = r_CFK - t_CFK;
  //c Parameters for the copper holding tube
  G4double r_CU64 = 1.3*CLHEP::cm;
  G4double t_CU64 = 0.1*CLHEP::cm;
  //G4double l_CU64 = 16.6*CLHEP::cm - (r_CU64/tan(31.*CLHEP::deg));// ! See target manual
  //G4double l_CU64 = 14.1*CLHEP::cm - 5.0*CLHEP::cm; // for 10-CLHEP::cm target, distance from back to middle of cell - distance between middle of cell and copper tube
  G4double l_CU64 = 19.0*CLHEP::cm - fTargetLength;
  //c Parameters for the spherical cell part upstream
  G4double t_lD2C = 0.14*CLHEP::cm;//    ! Deformation of window = Thickness of cell part
  G4double r_lD2C = ((r_CU64-t_CU64)*(r_CU64-t_CU64)-t_lD2C*t_lD2C)/(2*t_lD2C)+t_lD2C;
  //c Parameters for the other cell parts
  G4double r_lD2B = 4.543*CLHEP::cm;   //! See target manual
  G4double r_lD2A = 2.*CLHEP::cm;
  //G4double l_cell = 4.76*CLHEP::cm ;  // ! Length of cell at beam axis
  //G4double l_cell = 10.0*CLHEP::cm ;  // For 10-CLHEP::cm cell
  G4double l_cell = fTargetLength;
  fLength=l_cell;
  fRadius=r_lD2A;
  G4double t_lD2B = r_lD2B - sqrt(r_lD2B*r_lD2B-r_lD2A*r_lD2A);
  G4double l_lD2A = l_cell - t_lD2C - t_lD2B;
 //c Parameters for the tmat supply tube
  G4double r_lD2D = r_lD2A;
  G4double t_lD2D = r_lD2A - r_CU64;
  G4double l_lD2D = l_CU64;
  //c Parameters for the kapton exit window
  G4double r_kapd = r_lD2B;
  G4double t_KAPT = 0.0125*CLHEP::cm;//   ! Valid for all kapton parts

  G4double tm = -l_trgt/2. + l_CU64 + l_cell/2.;// ! Middle of target cell

  //Construct the volumes
  /////////////////////////
  //Target Mother Volume
  ////////////////////////
  if(!fMaterial){
    G4cerr<<"A2CryoTarget:Construct Target Material not defined!"<<G4endl;
    exit(1);
    //fMaterial=fNistManager->FindOrBuildMaterial("LH2");
  }
  G4Tubs* MyShape=new G4Tubs("TRGT",0.,r_trgt+0.1*CLHEP::mm,l_trgt/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(MyShape,fNistManager->FindOrBuildMaterial("G4_AIR"),"TRGT");
  fMyPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,Z0-tm+0.25*CLHEP::cm),fMyLogic,"TRGT",fMotherLogic,false,1);
  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);
  //////////////////////
  //CFK tube + window
  /////////////////////
  G4Tubs* CFKT=new G4Tubs("CFKT",r_CFK-t_CFK,r_CFK,l_CFK/2,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CFKTLogic=new G4LogicalVolume(CFKT,fNistManager->FindOrBuildMaterial("G4_GRAPHITE"),"CFKT");
  new G4PVPlacement(0,G4ThreeVector(0,0,- l_flangeo/2.),CFKTLogic,"CFKT",fMyLogic,false,1);
  G4VisAttributes* TarVisAtt= new G4VisAttributes(G4Colour(.0,.0,1.0));
  TarVisAtt->SetForceWireframe(true);
  CFKTLogic->SetVisAttributes(TarVisAtt);

  G4Tubs* FLA1=new G4Tubs("FLA1",r_flangeo - t_flangeo,r_flangeo,l_flangeo/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA1Logic=new G4LogicalVolume(FLA1,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA1");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo/2.),FLA1Logic,"FLA1",fMyLogic,false,2);

  G4Tubs* FLA2=new G4Tubs("FLA2",r_flangei - t_flangei,r_flangei,l_flangei/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA2Logic=new G4LogicalVolume(FLA2,fNistManager->FindOrBuildMaterial("G4_Al"),"FLA2");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. - l_flangeo - l_flangei/2.),FLA2Logic,"FLA2",fMyLogic,false,3);

  //Note sphere constructor, ("name"rmin,rmax,phimin,deltaphi,thetamin,deltatheta)
  G4Sphere* KAPD=new G4Sphere("KAPD",r_kapd,r_kapd+t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg - asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad,asin((r_flangeo-t_flangeo)/r_kapd)*CLHEP::rad);
  G4LogicalVolume* KAPDLogic=new G4LogicalVolume(KAPD,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPD");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_trgt/2. + sqrt(r_kapd*r_kapd-(r_flangeo-t_flangeo)*(r_flangeo-t_flangeo))),KAPDLogic,"KAPD",fMyLogic,false,4);

  //////////////////////
  //Kapton tube+windows
  ////////////////////////
  G4Tubs* KAPA=new G4Tubs("KAPA",r_lD2A,r_lD2A + t_KAPT,(l_CU64 + l_lD2A)/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* KAPALogic=new G4LogicalVolume(KAPA,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPA");
  new G4PVPlacement(0,G4ThreeVector(0,0,(l_CU64 + l_lD2A - l_trgt)/2.),KAPALogic,"KAPA",fMyLogic,false,5);
  KAPALogic->SetVisAttributes(TarVisAtt);

  G4Sphere* KAPB=new G4Sphere("KAPB",r_lD2B,r_lD2B + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,0,asin((r_lD2A+t_KAPT)/(r_lD2B+t_KAPT))*CLHEP::rad);
  G4LogicalVolume* KAPBLogic=new G4LogicalVolume(KAPB,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPB");
 new G4PVPlacement(0,G4ThreeVector(0,0,- l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),KAPBLogic,"KAPB",fMyLogic,false,6);


  G4Sphere* KAPC=new G4Sphere("KAPC",r_lD2C,r_lD2C + t_KAPT,0*CLHEP::deg,360*CLHEP::deg,180*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
  G4LogicalVolume* KAPCLogic=new G4LogicalVolume(KAPC,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPC");
  new G4PVPlacement(0,G4ThreeVector(0,0,  - l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),KAPCLogic,"KAPC",fMyLogic,false,7);

  ////////////////////////////////
  //LH2 or LD2
  ///////////////////////////////
    G4Tubs* LD2A=new G4Tubs("LD2A",0,r_lD2A,l_lD2A/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2ALogic=new G4LogicalVolume(LD2A,fMaterial,"LD2A");
    new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64 + (l_lD2A - l_trgt)/2.),LD2ALogic,"LD2A",fMyLogic,false,8);

    G4Sphere* LD2B=new G4Sphere("LD2B",r_lD2B - t_lD2B,r_lD2B,0*CLHEP::deg,360*CLHEP::deg,0,asin(r_lD2A/r_lD2B)*CLHEP::rad);
    G4LogicalVolume* LD2BLogic=new G4LogicalVolume(LD2B,fMaterial,"LD2B");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B),LD2BLogic,"LD2B",fMyLogic,false,9);
    G4cout<<"Target centre "<<(-l_trgt/2. + l_CU64 + l_lD2A + t_lD2B - r_lD2B)/CLHEP::cm <<" "<<l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm<<G4endl;
    fCenter.set(0,0,l_CU64 + (l_lD2A - l_trgt)/2.-tm+0.25*CLHEP::cm);//position of cell in world volume
    G4Sphere* LD2C=new G4Sphere("LD2C",r_lD2C - t_lD2C,r_lD2C,0*CLHEP::deg,360*CLHEP::deg,180.*CLHEP::deg-asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad,asin((r_CU64-t_CU64)/r_lD2C)*CLHEP::rad);
    G4LogicalVolume* LD2CLogic=new G4LogicalVolume(LD2C,fMaterial,"LD2C");
    new G4PVPlacement(0,G4ThreeVector(0,0,-l_trgt/2. + l_CU64 - t_lD2C + r_lD2C),LD2CLogic,"LD2C",fMyLogic,false,10);

    G4Tubs* LD2D=new G4Tubs("LD2D",r_lD2D - t_lD2D,r_lD2D,l_lD2D/2.,0*CLHEP::deg,360*CLHEP::deg);
    G4LogicalVolume* LD2DLogic=new G4LogicalVolume(LD2D,fMaterial,"LD2D");
    new G4PVPlacement(0,G4ThreeVector(0,0,+l_lD2D/2. - l_trgt/2.),LD2DLogic,"LD2D",fMyLogic,false,11);

  G4VisAttributes* LD2VisAtt= new G4VisAttributes(G4Colour(1.0,.0,0.0));
  LD2VisAtt->SetForceSolid(true);
  LD2ALogic->SetVisAttributes(LD2VisAtt);
  LD2BLogic->SetVisAttributes(LD2VisAtt);
  LD2CLogic->SetVisAttributes(LD2VisAtt);
  LD2DLogic->SetVisAttributes(TarVisAtt);

  //////////////////////////////
  //Copper tube
  ////////////////////////////
  G4Tubs* CU64=new G4Tubs("CU64",r_CU64 - t_CU64,r_CU64,l_CU64/2.,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CU64Logic=new G4LogicalVolume(CU64,fNistManager->FindOrBuildMaterial("G4_Cu"),"CU64");
  new G4PVPlacement(0,G4ThreeVector(0,0,l_CU64/2. - l_trgt/2.),CU64Logic,"CU64",fMyLogic,false,12);

  return fMyPhysi;

}
