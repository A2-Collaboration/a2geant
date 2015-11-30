#include "A2SolidTarget.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

A2SolidTarget::A2SolidTarget()
{

}
A2SolidTarget::~A2SolidTarget()
{

}

G4VPhysicalVolume* A2SolidTarget::Construct(G4LogicalVolume *MotherLogic, G4double Z0){
  fMotherLogic=MotherLogic;
  //Parameters taken from ugeom_solid_target.F
  G4double zm = 20.*CLHEP::cm;   //      ! z for mother volume
  G4double rm = 3.94*CLHEP::cm; //        ! radius of mother volume
  G4double zpos = -(zm-14.5*CLHEP::cm); //        ! zpos of mother volume relative to centre of ball

  if(!fMaterial){G4cerr<<"A2SolidTarget::Construct() Solid target material not defined. Add in DetectorSetup.mac."<<G4endl;exit(1);}
  //c target lenght:
  G4double trgt_length;
  if(fMaterial==G4NistManager::Instance()->FindOrBuildMaterial("G4_Li"))trgt_length=5.*CLHEP::cm;
  // else if(fMaterial==G4NistManager::Instance()->FindOrBuildMaterial("G4_GRAPHITE"))trgt_length=1.5*CLHEP::cm;
  else if(fMaterial==G4NistManager::Instance()->FindOrBuildMaterial("G4_Ca"))trgt_length=1.*CLHEP::cm;
  else if(fMaterial==G4NistManager::Instance()->FindOrBuildMaterial("G4_GRAPHITE"))trgt_length=1.5*CLHEP::cm;
  else if(fMaterial==G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"))trgt_length=0.05*CLHEP::cm;
  else {G4cerr<<"A2SolidTarget::Construct() Solid target length not defined!!"<<G4endl;exit(1);}
  fLength=trgt_length;
  ///////////////////////////
  //Construct the volumes
  /////////////////////////
  //Target Mother Volume
  ////////////////////////
  if(!fMaterial){
    G4cerr<<"A2SolidTarget:Construct Target Material not defined! Please do so in DetectorSetup.mac"<<G4endl;
    exit(1);
    //fMaterial=G4NistManager::Instance()->FindOrBuildMaterial("LH2");
  }
  G4Tubs* MyShape=new G4Tubs("TRGT",0.,rm,zm,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(MyShape,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),"TRGT");
  fMyPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,zpos),fMyLogic,"TRGT",fMotherLogic,false,1);
  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);
  //////////////////////
  //CFK tube + window
  /////////////////////
  G4Tubs* CFKT=new G4Tubs("CFKT",0,3.6*CLHEP::cm,zm-0.25*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* CFKTLogic=new G4LogicalVolume(CFKT,G4NistManager::Instance()->FindOrBuildMaterial("G4_GRAPHITE"),"CFKT");
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.25*CLHEP::cm),CFKTLogic,"CFKT",fMyLogic,false,1);
  G4VisAttributes* TarVisAtt= new G4VisAttributes(G4Colour(.0,.0,1.0));
  TarVisAtt->SetForceWireframe(true);
  CFKTLogic->SetVisAttributes(TarVisAtt);
  CFKTLogic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Tubs* VAC1=new G4Tubs("VAC1",0,3.5*CLHEP::cm,zm-0.5*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* VAC1Logic=new G4LogicalVolume(VAC1,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"VAC1");
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.5*CLHEP::cm),VAC1Logic,"VAC1",CFKTLogic,false,1);
  VAC1Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Tubs* FLA1=new G4Tubs("FLA1",0,3.5*CLHEP::cm,0.5*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA1Logic=new G4LogicalVolume(FLA1,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"FLA1");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.5*CLHEP::cm-0.25*CLHEP::cm),FLA1Logic,"FLA1",CFKTLogic,false,2);
  FLA1Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Tubs* VAC2=new G4Tubs("VAC2",0,3.1*CLHEP::cm,0.3*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* VAC2Logic=new G4LogicalVolume(VAC2,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"VAC2");
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.2*CLHEP::cm),VAC2Logic,"VAC2",FLA1Logic,false,1);
  VAC2Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Cons *VAC3=new G4Cons("VAC3",0*CLHEP::cm,3.1*CLHEP::cm,0*CLHEP::cm,3.19*CLHEP::cm,0.2*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* VAC3Logic=new G4LogicalVolume(VAC3,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"VAC3");
  //G4VPhysicalVolume* VAC3Physi=new G4PVPlacement(0,G4ThreeVector(0,0,-0.2*CLHEP::cm),VAC3Logic,"VAC3",FLA1Logic,false,2);
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.2*CLHEP::cm),VAC3Logic,"VAC3",FLA1Logic,false,2);
  VAC3Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Tubs* FLA2=new G4Tubs("FLA2",0*CLHEP::cm,rm,0.15*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA2Logic=new G4LogicalVolume(FLA2,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"FLA2");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.15*CLHEP::cm-0.2*CLHEP::cm),FLA2Logic,"FLA2",fMyLogic,false,2);
  FLA2Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Cons *VAC4=new G4Cons("VAC4",0*CLHEP::cm,3.19*CLHEP::cm,0*CLHEP::cm,3.26*CLHEP::cm,0.15*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* VAC4Logic=new G4LogicalVolume(VAC4,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"VAC4");
  //G4VPhysicalVolume* VAC4Physi=new G4PVPlacement(0,G4ThreeVector(0,0,-0.2*CLHEP::cm),VAC4Logic,"VAC4",FLA2Logic,false,1);
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.2*CLHEP::cm),VAC4Logic,"VAC4",FLA2Logic,false,1);
  VAC4Logic->SetVisAttributes (G4VisAttributes::Invisible);

  G4Tubs* FLA3=new G4Tubs("FLA3",rm-0.68*CLHEP::cm,rm,0.1*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* FLA3Logic=new G4LogicalVolume(FLA3,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"FLA3");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.1*CLHEP::cm),FLA3Logic,"FLA3",fMyLogic,false,3);
  FLA3Logic->SetVisAttributes (G4VisAttributes::Invisible);

  //////////////////////////////////////
  //Kapton window
  //////////////////////////////////////
  G4Tubs* WIND=new G4Tubs("WIND",0,rm-0.68*CLHEP::cm,0.006*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* WINDLogic=new G4LogicalVolume(WIND,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),"WIND");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.2*CLHEP::cm-0.006*CLHEP::cm),WINDLogic,"WIND",fMyLogic,false,4);
  WINDLogic->SetVisAttributes (G4VisAttributes::Invisible);
  /////////////////////////////////////
    //Target cell
    ///////////////////////////////////////

  //  G4Tubs* CELL=new G4Tubs("CELL",0,1.5*CLHEP::cm,trgt_length/2,0*CLHEP::deg,360*CLHEP::deg);
  G4Tubs* CELL=new G4Tubs("CELL",0,2.1*CLHEP::cm,trgt_length/2,0*CLHEP::deg,360*CLHEP::deg);//ctarbert measurement
  G4LogicalVolume* CELLLogic=new G4LogicalVolume(CELL,fMaterial,"CELL");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.75*CLHEP::cm-13*CLHEP::cm),CELLLogic,"CELL",VAC1Logic,false,1);
  G4VisAttributes* CellVisAtt= new G4VisAttributes(G4Colour(1.0,.0,0.0));
  //TarVisAtt->SetForceWireframe(true);
  CELLLogic->SetVisAttributes(CellVisAtt);
  fRadius=2*CLHEP::cm;
  // G4cout<<"Target centre "<<(zpos+zm-0.75*CLHEP::cm-13*CLHEP::cm-0.5*CLHEP::cm)/CLHEP::cm<<"CLHEP::cm"<<G4endl;
  //  fCenter.set(0,0,zpos+zm-0.75*CLHEP::cm-13*CLHEP::cm-0.5*CLHEP::cm);//position of cell in world volume
  G4cout<<"Target centre "<<(zm-0.75*CLHEP::cm-13*CLHEP::cm+zpos-0.25*CLHEP::cm-0.5*CLHEP::cm)/CLHEP::cm<<"CLHEP::cm"<<G4endl;
  fCenter.set(0,0,zm-0.75*CLHEP::cm-13*CLHEP::cm+zpos-0.25*CLHEP::cm-0.5*CLHEP::cm);//position of cell in world volume

  /////////////////////////////////////
    //Target holder
    ///////////////////////////////////
  G4Tubs* HOLD=new G4Tubs("HOLD",2.1*CLHEP::cm,3.5*CLHEP::cm,0.5*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* HOLDLogic=new G4LogicalVolume(HOLD,G4NistManager::Instance()->FindOrBuildMaterial("A2_ROHACELL"),"HOLD");
  new G4PVPlacement(0,G4ThreeVector(0,0,zm-0.75*CLHEP::cm-13*CLHEP::cm),HOLDLogic,"HOLD",VAC1Logic,false,2);

  return NULL;

}

// Alternate constructors if different variants of Target wanted, currently unused

G4VPhysicalVolume* A2SolidTarget::Construct1(G4LogicalVolume *MotherLogic, G4double Z0)
{
 return 0;
}

G4VPhysicalVolume* A2SolidTarget::Construct2(G4LogicalVolume *MotherLogic, G4double Z0)
{
 return 0;
}
