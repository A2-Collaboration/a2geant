#include "A2DetMWPC.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"

A2DetMWPC::A2DetMWPC(){

  fregionMWPC=NULL;
  fregionMWPC=new G4Region("MWPC");//allows seperate cuts to be defined for crystal

  fNIWires = 232;
  fNOWires = 296;

  fCHSI1Physi = NULL;
  fCHSO1Physi = NULL;
  fCHSI2Physi = NULL;
  fCHSO2Physi = NULL;

  fCHRI1Physi = NULL;
  fCHRO1Physi = NULL;
  fCHRI2Physi = NULL;
  fCHRO2Physi = NULL;

  fCHKI1Physi = NULL;
  fCHKO1Physi = NULL;
  fCHKI2Physi = NULL;
  fCHKO2Physi = NULL;

  fCHCI1Physi = NULL;
  fCHCO1Physi = NULL;
  fCHCI2Physi = NULL;
  fCHCO2Physi = NULL;

  fCHG1Physi = NULL;
  fCHG2Physi = NULL;


  fCHSI1Logic = NULL;
  fCHSO1Logic = NULL;
  fCHSI2Logic = NULL;
  fCHSO2Logic = NULL;

  fCHRI1Logic = NULL;
  fCHRO1Logic = NULL;
  fCHRI2Logic = NULL;
  fCHRO2Logic = NULL;

  fCHKI1Logic = NULL;
  fCHKO1Logic = NULL;
  fCHKI2Logic = NULL;
  fCHKO2Logic = NULL;

  fCHCI1Logic = NULL;
  fCHCO1Logic = NULL;
  fCHCI2Logic = NULL;
  fCHCO2Logic = NULL;

  fCHG1Logic = NULL;
  fCHG2Logic = NULL;

  fMWPCSD1 = NULL;
  fMWPCSD2 = NULL;
  fMWPCSD3 = NULL;
  fMWPCSD4 = NULL;

  fuseAnodes=false;
  fANOIPhysi = new G4VPhysicalVolume* [fNIWires]; //Inner Wire Physical Volume
  for (G4int i = 0; i < fNIWires + 1; i++)
    fANOIPhysi[i] = NULL;

  fANOOPhysi = new G4VPhysicalVolume* [fNOWires]; //Outer Wire Physical Volume
  for (G4int i = 0; i < fNOWires + 1; i++)
    fANOOPhysi[i] = NULL;

}

A2DetMWPC::~A2DetMWPC(){}

G4VPhysicalVolume* A2DetMWPC::ConstructMWPC(G4LogicalVolume* MotherLogical, G4double Z0){
  fMotherLogic = MotherLogical;

//Positioning Parameters

   fz = Z0 + 0*CLHEP::mm;
   fdz = 64.5*CLHEP::cm/2;

//Parameters for the First (Inner) Chamber
  //Geometric Parameters for the inner Cu Shielding

   fchsi1_rmin = 136./2*CLHEP::mm;
   fchsi1_rmax = fchsi1_rmin+0.01*CLHEP::mm;
  //Geometric Parameters for the inner Rohacell Wall

   fchri1_rmin = fchsi1_rmax;
   fchri1_rmax = fchsi1_rmax+2*CLHEP::mm-0.01*CLHEP::mm-0.05*CLHEP::mm;//subtract off Cu and Kapton thickness
  //Geometric Parameters for the inner Kapton Strip

   fchki1_rmin = fchri1_rmax;
   fchki1_rmax = fchri1_rmax+0.05*CLHEP::mm;

  //Geometric Parameters for the inner Cathode Strip (Approximation of the Wires)

  // fchci1_rmin = 69.99*CLHEP::mm;
   //   fchci1_rmax = 70.0*CLHEP::mm;
   fchci1_rmin = fchki1_rmax;
   fchci1_rmax = fchci1_rmin+0.01*CLHEP::mm;

  //Geometric Parameters for the outer Cathode Strip (Approximation of the Wires)

   //fchco1_rmin = 78.0*CLHEP::mm;
   //fchco1_rmax = 78.01*CLHEP::mm;
   fchco1_rmin = 156./2*CLHEP::mm-0.01*CLHEP::mm;
   fchco1_rmax = 156./2*CLHEP::mm;

  //Geometric Parameters for the outer Kapton Strip

   fchko1_rmin = fchco1_rmax;
   fchko1_rmax = fchko1_rmin+0.05*CLHEP::mm;

  //Geometric Parameters for the outer Rohacell Wall

   fchro1_rmin = fchko1_rmax;
   fchro1_rmax = fchro1_rmin+2*CLHEP::mm;

  //Geometric Parameters for the outer Shielding

   fchso1_rmin = fchro1_rmax;
   fchso1_rmax = fchso1_rmin+0.01*CLHEP::mm;

  //Geometric Parameters for the First Chamber Gas

   fchg1_rmin = fchci1_rmax;
   fchg1_rmax = fchco1_rmin;

   ///////////////////////////////////////////////////////////////////////////////////
   //Parameters for the Second  Chamber
  // Inner Cu Shielding

   fchsi2_rmin = 177./2*CLHEP::mm;
   fchsi2_rmax = fchsi2_rmin+0.01*CLHEP::mm;
  // inner Rohacell Wall

   fchri2_rmin = fchsi2_rmax;
   fchri2_rmax = fchsi2_rmax+2*CLHEP::mm-0.01*CLHEP::mm-0.05*CLHEP::mm;//subtract off Cu and Kapton thickness
  // inner Kapton Strip

   fchki2_rmin = fchri2_rmax;
   fchki2_rmax = fchri2_rmax+0.05*CLHEP::mm;

  // inner Cathode Strip (Approximation of the Wires)

   fchci2_rmin = fchki2_rmax;
   fchci2_rmax = fchci2_rmin+0.01*CLHEP::mm;

  //Geometric Parameters for the outer Cathode Strip (Approximation of the Wires)

   fchco2_rmin = 197./2*CLHEP::mm-0.01*CLHEP::mm;
   fchco2_rmax = 197./2*CLHEP::mm;

  // outer Kapton Strip

   fchko2_rmin = fchco2_rmax;
   fchko2_rmax = fchko2_rmin+0.05*CLHEP::mm;

  // outer Rohacell Wall

   fchro2_rmin = fchko2_rmax;
   fchro2_rmax = fchro2_rmin+2*CLHEP::mm;

  // outer Shielding

   fchso2_rmin = fchro2_rmax;
   fchso2_rmax = fchso2_rmin+0.01*CLHEP::mm;

  //Geometric Parameters for the Second Chamber Gas

   fchg2_rmin = fchci2_rmax;
   fchg2_rmax = fchco2_rmin;







   ///////////////////////////////////////////////////
  //Angular Wire SPacing for Chambers

   fanoi_theta = 360*CLHEP::deg/fNIWires;
   fanoo_theta = 360*CLHEP::deg/fNOWires;

  //Geometric Parameters for a single wire

   fanode_rmin = 0*CLHEP::mm;
   fanode_rmax = 0.01*CLHEP::mm;


  MakeChambers();
  MakeInnerWire();
  MakeOuterWire();
  MakeSupports();

//Mother Volume Parameters

//  G4double moth_rmin = 55.6*CLHEP::mm;
//  G4double moth_rmax = 100.562*CLHEP::mm;
  //G4double moth_rmin = fchsi1_rmin-0.1*CLHEP::mm;
  //G4double moth_rmax = fchso2_rmax+0.1*CLHEP::mm;
  G4double moth_rmin = fchsi1_rmin-0.1*CLHEP::mm;//werthmeuller correst for overlap
  G4double moth_rmax = fchso2_rmax+0.1*CLHEP::mm;//werthmeuller correst for overlap
  // G4Tubs *MWPCMother = new G4Tubs("MWPC", moth_rmin, moth_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg); dglazier make sure mv is large enough to incude supports
  G4Tubs *MWPCMother = new G4Tubs("MWPC", moth_rmin, moth_rmax, fdz+1*CLHEP::cm, 0*CLHEP::deg, 360*CLHEP::deg);

  fMyLogic = new G4LogicalVolume(MWPCMother, fNistManager->FindOrBuildMaterial("G4_AIR"),"MWPC");
  fregionMWPC->AddRootLogicalVolume(fMyLogic);

  fMyPhysi = new G4PVPlacement(0, G4ThreeVector(0,0,0), fMyLogic, "MWPC", fMotherLogic, false, 1);
  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(1,0,0,1));
  visatt->SetForceWireframe(true);
  fMyLogic->SetVisAttributes(visatt);
  //  fMyLogic->SetVisAttributes(G4VisAttributes::Invisible);

//Make Functions

  MakeDetector();
  MakeSensitiveDetector();

  return fMyPhysi;
}

void A2DetMWPC::MakeInnerWire(){
//Make individual wire for the inner chamber

  fANOI = new G4Tubs ("Inner Wire", fanode_rmin, fanode_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fANOILogic = new G4LogicalVolume (fANOI, fNistManager->FindOrBuildMaterial("G4_W"), "ANOIL");

  G4VisAttributes* ani_visatt = new G4VisAttributes();
  ani_visatt->SetColor(G4Color(1,1,1));
  fANOILogic->SetVisAttributes(ani_visatt);


}

void A2DetMWPC::MakeOuterWire(){
//Make individual wire for the outer chamber

  fANOO = new G4Tubs ("Outer Wire", fanode_rmin, fanode_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fANOOLogic = new G4LogicalVolume (fANOO, fNistManager->FindOrBuildMaterial("G4_W"), "ANOOL");


  G4VisAttributes* ano_visatt = new G4VisAttributes();
  ano_visatt->SetColor(G4Color(1,1,1));
  fANOILogic->SetVisAttributes(ano_visatt);

}

void A2DetMWPC::MakeChambers(){
//Makes materials in the chambers (Copper Shields, Rohacell Walls, Kapton Strips, Cathode Approximation and Chamber Gas.

//Shielding
  fCHSI1 = new G4Tubs("Inner Shield 1", fchsi1_rmin, fchsi1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHSI1Logic = new G4LogicalVolume(fCHSI1, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHSI1L");

  G4VisAttributes* is1_visatt = new G4VisAttributes();
  is1_visatt->SetColor(G4Color(0.0,1.0,0.0));
  fCHSI1Logic->SetVisAttributes(is1_visatt);

  fCHSO1 = new G4Tubs("Outer Shield 1", fchso1_rmin, fchso1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHSO1Logic = new G4LogicalVolume(fCHSO1, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHSO1L");

  G4VisAttributes* os1_visatt = new G4VisAttributes();
  os1_visatt->SetColor(G4Color(0.0,1.0,0.0));
  fCHSO1Logic->SetVisAttributes(os1_visatt);

  fCHSI2 = new G4Tubs("Inner Shield 2", fchsi2_rmin, fchsi2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHSI2Logic = new G4LogicalVolume(fCHSI2, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHSI2L");

  G4VisAttributes* is2_visatt = new G4VisAttributes();
  is2_visatt->SetColor(G4Color(0.0,1.0,0.0));
  fCHSI2Logic->SetVisAttributes(is2_visatt);

  fCHSO2 = new G4Tubs("Outer Shield 2", fchso2_rmin, fchso2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHSO2Logic = new G4LogicalVolume(fCHSO2, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHSO2L");

  G4VisAttributes* os2_visatt = new G4VisAttributes();
  os2_visatt->SetColor(G4Color(0.0,1.0,0.0));
  fCHSO2Logic->SetVisAttributes(os2_visatt);


//Rohacell

  fCHRI1 = new G4Tubs("Inner Rohacell 1", fchri1_rmin, fchri1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHRI1Logic = new G4LogicalVolume(fCHRI1, fNistManager->FindOrBuildMaterial("A2_ROHACELL"), "CHRI1L");

  G4VisAttributes* ir1_visatt = new G4VisAttributes();
  ir1_visatt->SetColor(G4Color(1.0,0.0,0.0));
  fCHRI1Logic->SetVisAttributes(ir1_visatt);

  fCHRO1 = new G4Tubs("Outer Rohacell 1", fchro1_rmin, fchro1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHRO1Logic = new G4LogicalVolume(fCHRO1, fNistManager->FindOrBuildMaterial("A2_ROHACELL"), "CHRO1L");

  G4VisAttributes* or1_visatt = new G4VisAttributes();
  or1_visatt->SetColor(G4Color(1.0,0.0,0.0));
  fCHRO1Logic->SetVisAttributes(or1_visatt);

  fCHRI2 = new G4Tubs("Inner Rohacell 2", fchri2_rmin, fchri2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHRI2Logic = new G4LogicalVolume(fCHRI2, fNistManager->FindOrBuildMaterial("A2_ROHACELL"), "CHRI2L");

  G4VisAttributes* ir2_visatt = new G4VisAttributes();
  ir2_visatt->SetColor(G4Color(1.0,0.0,0.0));
  fCHRI2Logic->SetVisAttributes(ir2_visatt);

  fCHRO2 = new G4Tubs("Outer Rohacell 2", fchro2_rmin, fchro2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHRO2Logic = new G4LogicalVolume(fCHRO2, fNistManager->FindOrBuildMaterial("A2_ROHACELL"), "CHRO2L");

  G4VisAttributes* or2_visatt = new G4VisAttributes();
  or2_visatt->SetColor(G4Color(1.0,0.0,0.0));
  fCHRO2Logic->SetVisAttributes(or2_visatt);


//Kapton Strips

  fCHKI1 = new G4Tubs("Inner Kapton 1", fchki1_rmin, fchki1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHKI1Logic = new G4LogicalVolume(fCHKI1, fNistManager->FindOrBuildMaterial("G4_KAPTON"), "CHKI1L");

  G4VisAttributes* ik1_visatt = new G4VisAttributes();
  ik1_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHKI1Logic->SetVisAttributes(ik1_visatt);

  fCHKO1 = new G4Tubs("Outer Kapton 1", fchko1_rmin, fchko1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHKO1Logic = new G4LogicalVolume(fCHKO1, fNistManager->FindOrBuildMaterial("G4_KAPTON"), "CHKO1L");

  G4VisAttributes* ok1_visatt = new G4VisAttributes();
  ok1_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHKO1Logic->SetVisAttributes(ok1_visatt);

  fCHKI2 = new G4Tubs("Inner Kapton 2", fchki2_rmin, fchki2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHKI2Logic = new G4LogicalVolume(fCHKI2, fNistManager->FindOrBuildMaterial("G4_KAPTON"), "CHKI2L");

  G4VisAttributes* ik2_visatt = new G4VisAttributes();
  ik2_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHKI2Logic->SetVisAttributes(ik2_visatt);

  fCHKO2 = new G4Tubs("Outer Kapton 2", fchko2_rmin, fchko2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHKO2Logic = new G4LogicalVolume(fCHKO2, fNistManager->FindOrBuildMaterial("G4_KAPTON"), "CHKO2L");

  G4VisAttributes* ok2_visatt = new G4VisAttributes();
  ok2_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHKO2Logic->SetVisAttributes(ok2_visatt);

//Cathodes

  fCHCI1 = new G4Tubs("Inner Cathode 1", fchci1_rmin, fchci1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHCI1Logic = new G4LogicalVolume(fCHCI1, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHCI1L");

  G4VisAttributes* ic1_visatt = new G4VisAttributes();
  ic1_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHCI1Logic->SetVisAttributes(ic1_visatt);

  fCHCO1 = new G4Tubs("Outer Cathode 1", fchco1_rmin, fchco1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHCO1Logic = new G4LogicalVolume(fCHCO1, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHCO1L");

  G4VisAttributes* oc1_visatt = new G4VisAttributes();
  oc1_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHCO1Logic->SetVisAttributes(oc1_visatt);

  fCHCI2 = new G4Tubs("Inner Cathode 2", fchci2_rmin, fchci2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHCI2Logic = new G4LogicalVolume(fCHCI2, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHCI2L");

  G4VisAttributes* ic2_visatt = new G4VisAttributes();
  ic2_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHCI2Logic->SetVisAttributes(ic2_visatt);

  fCHCO2 = new G4Tubs("Outer Cathode 2", fchco2_rmin, fchco2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHCO2Logic = new G4LogicalVolume(fCHCO2, fNistManager->FindOrBuildMaterial("G4_Cu"), "CHCO2L");

  G4VisAttributes* oc2_visatt = new G4VisAttributes();
  oc2_visatt->SetColor(G4Color(0.0,0.0,1.0));
  fCHCO2Logic->SetVisAttributes(oc2_visatt);

//Gas

  fCHG1 = new G4Tubs("Gas 1", fchg1_rmin, fchg1_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHG1Logic = new G4LogicalVolume(fCHG1, fNistManager->FindOrBuildMaterial("A2_WCMAT"), "CHG1L");

//  fCHG1Logic->SetVisAttributes(ok2_visatt);
  fCHG1Logic->SetVisAttributes(G4VisAttributes::Invisible);


  fCHG2 = new G4Tubs("Gas 2", fchg2_rmin, fchg2_rmax, fdz, 0*CLHEP::deg, 360*CLHEP::deg);

  fCHG2Logic = new G4LogicalVolume(fCHG2, fNistManager->FindOrBuildMaterial("A2_WCMAT"), "CHG2L");

//  fCHG2Logic->SetVisAttributes(ok2_visatt);
  fCHG2Logic->SetVisAttributes(G4VisAttributes::Invisible);
}

void A2DetMWPC::MakeSupports(){
//Functions to make the Fibre Glass Supports for the Wire Chambers

//   G4Tubs* fWS1U=new G4Tubs("fWS1U",5.56*cm,8.16*cm,0.95*cm,0*deg,360*deg);
//   fWS1ULogic = new G4LogicalVolume(fWS1U,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"WS1UL");

//   G4Tubs* fWS1D=new G4Tubs("fWS1D",5.56*cm,8.16*cm,0.95*cm,0*deg,360*deg);
//   fWS1DLogic = new G4LogicalVolume(fWS1D,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"WS1DL");

//   G4Cons* fSC1U = new G4Cons("fSC1U",5.56*cm,8.16*cm,5.56*cm,7.16*cm,1.275*cm,0*deg,360*cm);
//   fSC1ULogic = new G4LogicalVolume(fSC1U,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"SC1UL");

//   G4Cons* fSC1D= new G4Cons("fSC1D",5.56*cm,7.16*cm,5.56*cm,8.16*cm,1.275*cm,0*deg,360*cm);
//   fSC1DLogic = new G4LogicalVolume(fSC1D,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"SC1DL");

//   G4Tubs *fWS2U = new G4Tubs("fWS2U", 8.45*cm,9.98*cm,2.525*cm,0*deg,360*deg);
//   fWS2ULogic = new G4LogicalVolume(fWS2U,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"WS2UL");

//   G4Tubs *fWS2D = new G4Tubs("fWS2D", 8.45*cm,9.98*cm,2.525*cm,0*deg,360*deg);
//   fWS2DLogic = new G4LogicalVolume(fWS2D,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"WS2DL");

  //aluminium tube linking MWPCs
  //This was only in the old design
//   G4Tubs *fALTU = new G4Tubs("ALTU",8.2*cm,8.4*cm,6.05*cm,0*deg,360*deg);
//   fALTULogic = new G4LogicalVolume(fALTU,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"ALTUL");

//  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.7,0.4,0.6));
//   fWS1ULogic->SetVisAttributes(SupVisAtt);
//   fWS1DLogic->SetVisAttributes(SupVisAtt);
//   fSC1ULogic->SetVisAttributes(SupVisAtt);
//   fSC1DLogic->SetVisAttributes(SupVisAtt);
//   fWS2ULogic->SetVisAttributes(SupVisAtt);
//   fWS2DLogic->SetVisAttributes(SupVisAtt);
  //  fALTULogic->SetVisAttributes(SupVisAtt);

//  fWS1ULogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fWS1DLogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fSC1ULogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fSC1DLogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fWS2ULogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fWS2DLogic->SetVisAttributes(G4VisAttributes::Invisible);
//  fALTULogic->SetVisAttributes(G4VisAttributes::Invisible);

  G4Tubs* fWS1U=new G4Tubs("fWS1U",fchsi1_rmin-1.5*CLHEP::mm,fchsi1_rmin,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fWS1ULogic = new G4LogicalVolume(fWS1U,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"WS1UL");

  G4Tubs* fWS1U2=new G4Tubs("fWS1U2",fchso2_rmax,fchso2_rmax+1.5*CLHEP::mm,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fWS1U2Logic = new G4LogicalVolume(fWS1U2,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"WS1UL2");

  G4Tubs* fWS1D=new G4Tubs("fWS1D",fchg1_rmin,(fchg1_rmin+fchg1_rmax)/2-1*CLHEP::mm,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fWS1DLogic = new G4LogicalVolume(fWS1D,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"WS1DL");

  G4Tubs* fSC1U = new G4Tubs("fSC1D",(fchg1_rmin+fchg1_rmax)/2+1*CLHEP::mm,fchg1_rmax,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fSC1ULogic = new G4LogicalVolume(fSC1U,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"SC1UL");

  G4Tubs* fSC1D= new G4Tubs("fSC1D",fchso1_rmax,fchsi2_rmin,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fSC1DLogic = new G4LogicalVolume(fSC1D,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"SC1DL");

  G4Tubs *fWS2U = new G4Tubs("fWS2U",fchg2_rmin,(fchg2_rmin+fchg2_rmax)/2-1*CLHEP::mm ,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fWS2ULogic = new G4LogicalVolume(fWS2U,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"WS2UL");

  G4Tubs *fWS2D = new G4Tubs("fWS2D",(fchg2_rmin+fchg2_rmax)/2+1*CLHEP::mm ,fchg2_rmax,3.5*CLHEP::cm/2,0*CLHEP::deg,360*CLHEP::deg);
  fWS2DLogic = new G4LogicalVolume(fWS2D,fNistManager->FindOrBuildMaterial("G4_POLYETHYLENE"),"WS2DL");

}

void A2DetMWPC::MakeDetector() {

//Placement of Materials into the Mother Volume
//Shielding
  fCHSI1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHSI1Logic, "CHSI1P", fMyLogic, false, 0);

  fCHSO1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHSO1Logic, "CHSO1P", fMyLogic, false, 0);

  fCHSI2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHSI2Logic, "CHSI2P", fMyLogic, false, 0);

  fCHSO2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHSO2Logic, "CHSO2P", fMyLogic, false, 0);

//Rohacell
  fCHRI1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHRI1Logic, "CHRI1P", fMyLogic, false, 0);

  fCHRO1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHRO1Logic, "CHRO1P", fMyLogic, false, 0);

  fCHRI2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHRI2Logic, "CHRI2P", fMyLogic, false, 0);

  fCHRO2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHRO2Logic, "CHRO2P", fMyLogic, false, 0);

//Kapton
  fCHKI1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHKI1Logic, "CHKI1P", fMyLogic, false, 0);

  fCHKO1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHKO1Logic, "CHKO1P", fMyLogic,
  false, 0);

  fCHKI2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHKI2Logic, "CHKI2P", fMyLogic, false, 0);

  fCHKO2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHKO2Logic, "CHKO2P", fMyLogic, false, 0);

//Cathodes

  fCHCI1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHCI1Logic, "CHCI1P", fMyLogic, false, 0);

  fCHCO1Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHCO1Logic, "CHCO1P", fMyLogic, false, 1);

  fCHCI2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHCI2Logic, "CHCI2P", fMyLogic, false, 2);

  fCHCO2Physi = new G4PVPlacement(0, G4ThreeVector(0,0,fz), fCHCO2Logic, "CHCO2P", fMyLogic, false, 3);

//Anode Wires

  if(fuseAnodes){
    for(G4int i = 0; i < fNIWires; i++) {
      G4double fanode1_r = (fchg1_rmin + fchg1_rmax)/2;
      G4double fanode1_angle = fanoi_theta * i;
      G4double xpos = fanode1_r * cos(fanode1_angle);
      G4double ypos = fanode1_r * sin(fanode1_angle);
      G4ThreeVector dpos(xpos,ypos,fz);


      fANOIPhysi[i] = new G4PVPlacement(0, G4ThreeVector(xpos,ypos,fz), fANOILogic, "ANOIP", fMyLogic, false, i);
    }

    for(G4int i = 0; i < fNIWires; i++) {
      G4double fanode2_r = (fchg2_rmin + fchg2_rmax)/2;
      G4double fanode2_angle = fanoi_theta * i;
      G4double xpos = fanode2_r * cos(fanode2_angle);
      G4double ypos = fanode2_r * sin(fanode2_angle);
      G4ThreeVector dpos(xpos,ypos,fz);


      fANOOPhysi[i] = new G4PVPlacement(0, G4ThreeVector(xpos,ypos,fz), fANOOLogic, "ANOOP", fMyLogic, false, i);
    }
  }
//Supports and Linking
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fWS1DLogic,"WS1DPa",fMyLogic,false,101);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fWS1DLogic,"WS1DPb",fMyLogic,false,102);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fWS1ULogic,"WS1UPa",fMyLogic,false,103);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fWS1U2Logic,"WS1UP2b",fMyLogic,false,104);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fWS1U2Logic,"WS1UP2a",fMyLogic,false,103);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fWS1ULogic,"WS1UPb",fMyLogic,false,104);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fSC1ULogic,"SC1UPa",fMyLogic,false,105);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fSC1ULogic,"SC1UPb",fMyLogic,false,106);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fSC1DLogic,"SC1DPa",fMyLogic,false,107);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fSC1DLogic,"SC1DPb",fMyLogic,false,108);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fWS2DLogic,"WS2DPa",fMyLogic,false,109);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fWS2DLogic,"WS2DPb",fMyLogic,false,110);
  new G4PVPlacement(0,G4ThreeVector(0,0,fdz),fWS2ULogic,"WS2UPa",fMyLogic,false,111);
  new G4PVPlacement(0,G4ThreeVector(0,0,-fdz),fWS2ULogic,"WS2UPb",fMyLogic,false,112);

}

//Electronics

//Sensitive Detector

void A2DetMWPC::MakeSensitiveDetector(){
  if(!fMWPCSD1){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fMWPCSD1 = new A2WCSD("A2MWPCSD1", 100);
    SDman->AddNewDetector(fMWPCSD1);
  }
  if(!fMWPCSD2){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fMWPCSD2 = new A2WCSD("A2MWPCSD2", 100);
    SDman->AddNewDetector(fMWPCSD2);
  }
  if(!fMWPCSD3){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fMWPCSD3 = new A2WCSD("A2MWPCSD3", 100);
    SDman->AddNewDetector(fMWPCSD3);
  }
  if(!fMWPCSD4){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fMWPCSD4 = new A2WCSD("A2MWPCSD4", 100);
    SDman->AddNewDetector(fMWPCSD4);
  }


  fCHCI1Logic->SetSensitiveDetector(fMWPCSD1);
  fCHCO1Logic->SetSensitiveDetector(fMWPCSD2);
  fCHCI2Logic->SetSensitiveDetector(fMWPCSD3);
  fCHCO2Logic->SetSensitiveDetector(fMWPCSD4);
}
