#include "A2DetPol.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"

// 16/08/16 Need to confirm position of cradle and dimensions of pipe

A2DetPol::A2DetPol(){
  //fregionPol=NULL;
  //fregionPol=new G4Region("Pol");
  //Polarimeter
  //  G4Tubs* npol=new G4Tubs("NPOL",10/2*mm,60*mm,40/2*mm,0*deg,360*deg);
  // fZ0=0*cm;

}

A2DetPol::~A2DetPol(){
  //delete Rot;
}

G4VPhysicalVolume* A2DetPol::Construct1(G4LogicalVolume* MotherLogical,G4double Z0){
  //Build the Phase 1 polarimeter (2009)

  fMotherLogic=MotherLogical;

  //some parameters
  Xoff=0*CLHEP::mm;
  Yoff=0*CLHEP::mm;     //original
  fPol_Z = 207.5*CLHEP::mm;
  fPol_rin = 69.5*CLHEP::mm;
  fPol_rout = 92.5*CLHEP::mm;
  fPol_Z0 = Z0;

  //Make the polarimeter shape
  MakeTube();
  MakeCap();
  //MakeSupports1();

  G4cout<<"Made Phase I Polarimeter (2009)"<<G4endl;

  return fMyPhysi;
}

G4VPhysicalVolume* A2DetPol::Construct2(G4LogicalVolume* MotherLogical,G4double Z0, G4int PCap){
  //Build the phase 2 polarimeter (2015/2016) 2.6cm thick, this is as the polarimeter is in reality

  fMotherLogic=MotherLogical;
  //some parameters
  Xoff=0*CLHEP::mm;
  Yoff=0*CLHEP::mm;     //original
  fPol_Z = 300*CLHEP::mm;
  fPol_rin = 39*CLHEP::mm;
  fPol_rout = 65*CLHEP::mm;
  fCap_rin = 24*CLHEP::mm;
  fPol_Thick = fPol_rout - fPol_rin;
  fPol_Z0 = Z0;
  fUseCap = PCap;

  if (PCap == 0){
    //Make the polarimeter shape
    fCapThick = 0*CLHEP::mm;
    MakeTube2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016, 2.6cm)"<<G4endl;
  }

  else if (PCap == 1){
    //Make the polarimeter shape
    fCapThick = fPol_Thick;
    MakeTube2();
    MakeCap2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016, 2.6cm) with cap"<<G4endl;
 }

return fMyPhysi; // Need to define this, what is it and why does it matter?

}

G4VPhysicalVolume* A2DetPol::Construct3(G4LogicalVolume* MotherLogical,G4double Z0, G4int PCap){
  //Build the phase 2 polarimeter (2015/2016) backup tube 2.25cm thick

  fMotherLogic=MotherLogical;
  //some parameters
  Xoff=0*CLHEP::mm;
  Yoff=0*CLHEP::mm;     //original
  fPol_Z = 300*CLHEP::mm;
  fPol_rin = 42.5*CLHEP::mm;
  fPol_rout = 66*CLHEP::mm;
  fCap_rin = 24*CLHEP::mm;
  fPol_Thick = fPol_rout - fPol_rin;
  fPol_Z0 = Z0;
  fUseCap = PCap;

  if (PCap == 0){
    //Make the polarimeter shape
    fCapThick = 0*CLHEP::mm;
    MakeTube2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016, Backup Tube 2.25cm)"<<G4endl;
  }

  else if (PCap == 1){
    //Make the polarimeter shape
    fCapThick = 2.6*CLHEP::cm;
    MakeTube2();
    MakeCap2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016, Backup Tube 2.25cm)"<<G4endl;
 }

return fMyPhysi; // Need to define this, what is it and why does it matter?

}

void A2DetPol::MakeTube(){
  // Make polarimeter tube

  //*************carbon cylinder
  G4double tubez0= fPol_Z0-10*CLHEP::mm+fPol_Z/2;
  G4Tubs* npol2=new G4Tubs("NPOL2", fPol_rin, fPol_rout , fPol_Z/2,0*CLHEP::deg,360*CLHEP::deg); // Main barrel of polarimeter
  G4LogicalVolume* npolLogic2=new G4LogicalVolume(npol2,G4NistManager::Instance()->FindOrBuildMaterial("A2_G348GRAPHITE"),"NPOL2");
  G4VPhysicalVolume* npolPhysi2=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff,tubez0),npolLogic2,"NPOL2",fMotherLogic,false,998);
  G4cout<<"Weight of tube "<<npolLogic2->GetMass()/CLHEP::kg<<"kg"<<G4endl;
}

void A2DetPol::MakeTube2(){
  // Make 2016 polarimeter tube

  //*************carbon cylinder
  G4double tubez0= fPol_Z0;
  fPol_GrooveR = fPol_rout - (7*CLHEP::mm);
  G4Tubs* npol2=new G4Tubs("NPOL2", fPol_rin, fPol_rout , fPol_Z/2,0*CLHEP::deg,360*CLHEP::deg);     //small gap between cylinder and pipe wall where styrofoam was placed
  G4Tubs* SupGroove=new G4Tubs("SupGroove", 0*CLHEP::mm, 3.025*CLHEP::mm, (fPol_Z/2) + 5*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg); // Grooves cut in polarimeter for support
  G4SubtractionSolid *npol3=new G4SubtractionSolid("NPOL3",npol2,SupGroove,new G4RotationMatrix(),G4ThreeVector(0*CLHEP::mm, fPol_GrooveR, 0*CLHEP::mm ));
  G4SubtractionSolid *npol4=new G4SubtractionSolid("NPOL4",npol3,SupGroove,new G4RotationMatrix(),G4ThreeVector(fPol_GrooveR * cos(30*CLHEP::deg), -(fPol_GrooveR * sin(30*CLHEP::deg)) , 0*CLHEP::mm));
  G4SubtractionSolid *npol5=new G4SubtractionSolid("NPOL5",npol4,SupGroove,new G4RotationMatrix(),G4ThreeVector(-(fPol_GrooveR * cos(30*CLHEP::deg)), -(fPol_GrooveR * sin(30*CLHEP::deg)), 0*CLHEP::mm));
  G4LogicalVolume* npolLogic2=new G4LogicalVolume(npol5,G4NistManager::Instance()->FindOrBuildMaterial("A2_G348GRAPHITE"),"NPOL5");
  G4VPhysicalVolume* npolPhysi2=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff,tubez0),npolLogic2,"NPOL3",fMotherLogic,false,998);
  G4cout<<"Weight of tube "<<npolLogic2->GetMass()/CLHEP::kg<<"kg"<<G4endl;
}

void A2DetPol::MakeCap(){
  // Make cap for polarimeter

  //********forward cap
  G4Tubs* npol=new G4Tubs("NPOL",40./2*CLHEP::mm,190*CLHEP::mm/2,70./2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
  G4LogicalVolume* npolLogic=new G4LogicalVolume(npol,G4NistManager::Instance()->FindOrBuildMaterial("A2_G348GRAPHITE"),"NPOL");
  // G4VPhysicalVolume* npolPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,12*cm+40/2*mm),npolLogic,"NPOL",fWorldLogic,false,999);  //this stays commented out
  G4VPhysicalVolume* npolPhysi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff,31.5*CLHEP::cm),npolLogic,"NPOL",fMotherLogic,false,999);     //put this one back
  G4cout<<"Weight of cap "<<npolLogic->GetMass()/CLHEP::kg<<"kg"<<G4endl;
}

void A2DetPol::MakeCap2(){
  // Make cap for 2016 polarimeter

  //********forward cap
  fPol_HoleR = fPol_rout - (7*CLHEP::mm);
  G4Tubs* npolc1=new G4Tubs("NPOLC1",fCap_rin,fPol_rout,fPol_Thick/2,0*CLHEP::deg,360*CLHEP::deg);
  G4Tubs* CapHole=new G4Tubs("CapHole", 0*CLHEP::mm, 3.025*CLHEP::mm, fPol_Thick + 5*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg); // Grooves cut in polarimeter for support
  G4SubtractionSolid *npolc2=new G4SubtractionSolid("NPOLC2",npolc1,CapHole,new G4RotationMatrix(),G4ThreeVector(0*CLHEP::mm, fPol_HoleR, 0*CLHEP::mm ));
  G4SubtractionSolid *npolc3=new G4SubtractionSolid("NPOLC3",npolc2,CapHole,new G4RotationMatrix(),G4ThreeVector(fPol_HoleR * cos(30*CLHEP::deg), -(fPol_HoleR * sin(30*CLHEP::deg)) , 0*CLHEP::mm));
  G4SubtractionSolid *npolc4=new G4SubtractionSolid("NPOLC4",npolc3,CapHole,new G4RotationMatrix(),G4ThreeVector(-(fPol_HoleR * cos(30*CLHEP::deg)), -(fPol_HoleR * sin(30*CLHEP::deg)), 0*CLHEP::mm));
  G4LogicalVolume* npolLogic=new G4LogicalVolume(npolc4,G4NistManager::Instance()->FindOrBuildMaterial("A2_G348GRAPHITE"),"NPOL");
  G4VPhysicalVolume* npolPhysi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + (fPol_Thick/2)),npolLogic,"NPOL",fMotherLogic,false,999);
  G4cout<<"Weight of cap "<<npolLogic->GetMass()/CLHEP::kg<<"kg"<<G4endl;
}


//void A2DetPol::MakeSupports1(){
  // Make the supporting structure

  //**********orange support tube
  //G4Tubs* nptube=new G4Tubs("NPTUBE",195./2*CLHEP::mm,200*CLHEP::mm/2,280./2*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
  //G4LogicalVolume* nptubeLogic=new G4LogicalVolume(nptube,G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"NPTUBE");
  //G4VPhysicalVolume* nptubePhysi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff-0.*CLHEP::mm,-9.4*CLHEP::cm),nptubeLogic,"NPTUBE",fWorldLogic,false,997);

//}

void A2DetPol::MakeSupports2(){

  // Make the supporting structure
  // See wiki for diagrams of cradle pieces

  //Define some parameters to be used in construction

  fPol_HoleR = fPol_rout - (7*CLHEP::mm); // Radius of drilled hole for steel rods
  fPolSC_Thick = 10*CLHEP::mm; // Thickness of support caps on polarimeter
  fPolST_Thick = 1.6*CLHEP::mm; // Thickness of support tube
  fPolSR_Length = (30*CLHEP::cm) + (2*fPolSC_Thick) + (fCapThick) + (1*CLHEP::cm); // Length of polarimeter + cap and 2x support caps + some leeway
  fPolCradleThick = 12*CLHEP::mm; // Thickness of each piece of cradle
  fPolCradleOR = 95*CLHEP::mm; // Outer radius of cradle pieces
  fPolCradleIR1 = 24.5*CLHEP::mm; // Inner radius of first cradle piece (piece pipe rests on)
  fPolCradleIR2 = 66*CLHEP::mm; // Inner radius of other part of cradle
  fPolCradleSep = 250*CLHEP::mm; // Separation between two ends of cradle and length of rods
  fPolCradlePlacement = 200*CLHEP::mm; // How far from second end cap is cradle? Edit this parameter to move cradle along support pipe

  //Support end caps for carbon polarimeter

  // Downstream cap
  G4Tubs* npolsc1DS=new G4Tubs("NPOLSC1DS",fPol_rin,fPol_rout,fPolSC_Thick/2,0*CLHEP::deg,360*CLHEP::deg);
  G4Tubs* SupHole=new G4Tubs("CapHole", 0*CLHEP::mm, 3.025*CLHEP::mm, (fPolSC_Thick/2) + 0.2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg); // Holes cut in support caps for steel rods
  G4Tubs* SupRod=new G4Tubs("SupRod", 0* CLHEP::mm, 3*CLHEP::mm, fPolSR_Length/2, 0*CLHEP::deg, 360*CLHEP::deg); // Steel rods through polarimeter
  G4SubtractionSolid *npolsc2DS=new G4SubtractionSolid("NPOLSC2DS",npolsc1DS,SupHole,new G4RotationMatrix(),G4ThreeVector(0*CLHEP::mm, fPol_HoleR, 0*CLHEP::mm ));
  G4SubtractionSolid *npolsc3DS=new G4SubtractionSolid("NPOLSC3DS",npolsc2DS,SupHole,new G4RotationMatrix(),G4ThreeVector(fPol_HoleR * cos(30*CLHEP::deg), -(fPol_HoleR * sin(30*CLHEP::deg)) , 0*CLHEP::mm));
  G4SubtractionSolid *npolsc4DS=new G4SubtractionSolid("NPOLSC4DS",npolsc3DS,SupHole,new G4RotationMatrix(),G4ThreeVector(-(fPol_HoleR * cos(30*CLHEP::deg)), -(fPol_HoleR * sin(30*CLHEP::deg)), 0*CLHEP::mm));
  G4LogicalVolume* npolscDSLogic=new G4LogicalVolume(npolsc4DS,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLSCDS");
  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.1,0.5,0.0));
  G4VisAttributes* SupRodVisAtt= new G4VisAttributes(G4Colour(0.75,0.0,0.0));
  npolscDSLogic->SetVisAttributes(SupVisAtt);

  // Upstream cap that attaches to support rod
  G4Tubs* npolsc1US=new G4Tubs("NPOLSC1US",fCap_rin,fPol_rout,fPolSC_Thick/2,0*CLHEP::deg,360*CLHEP::deg);
  G4SubtractionSolid *npolsc2US=new G4SubtractionSolid("NPOLSC2US",npolsc1US,SupHole,new G4RotationMatrix(),G4ThreeVector(0*CLHEP::mm, fPol_HoleR, 0*CLHEP::mm ));
  G4SubtractionSolid *npolsc3US=new G4SubtractionSolid("NPOLSC3US",npolsc2US,SupHole,new G4RotationMatrix(),G4ThreeVector(fPol_HoleR * cos(30*CLHEP::deg), -(fPol_HoleR * sin(30*CLHEP::deg)) , 0*CLHEP::mm));
  G4SubtractionSolid *npolsc4US=new G4SubtractionSolid("NPOLSC4US",npolsc3US,SupHole,new G4RotationMatrix(),G4ThreeVector(-(fPol_HoleR * cos(30*CLHEP::deg)), -(fPol_HoleR * sin(30*CLHEP::deg)), 0*CLHEP::mm));
  G4LogicalVolume* npolscUSLogic=new G4LogicalVolume(npolsc4US,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLSCDS");
  npolscUSLogic->SetVisAttributes(SupVisAtt);

  G4VPhysicalVolume* npolsc1Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (-(fPol_Z/2)) + fPol_Z0 - (fPolSC_Thick/2)),npolscDSLogic,"NPOLSCDS",fMotherLogic,false,999); // Cap at downstream (PMT) end
  G4VPhysicalVolume* npolsc2Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + (fPolSC_Thick/2)),npolscUSLogic,"NPOLSCUS",fMotherLogic,false,999); // Cap at upstream (TAPS) end

  // Support tube for polarimeter
  // Need to adjust parameters of this slightly but need drawing first

  G4Tubs* npolst1 = new G4Tubs("NPOLST1", fCap_rin, fCap_rin + fPolST_Thick, 0.4*CLHEP::m, 0*CLHEP::deg, 360*CLHEP::deg); // 80cm long support tube
  G4LogicalVolume* npolstLogic=new G4LogicalVolume(npolst1,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLST"); // Build tube from aluminium
  npolstLogic->SetVisAttributes(SupVisAtt);
  G4VPhysicalVolume* npolst1Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolSC_Thick + (0.40*CLHEP::m)),npolstLogic,"NPOLST",fMotherLogic,false,999);

  // Cradle for polarimeter, support tube rests on this

  // Define basic shape of components

  G4Tubs* npolcr1 = new G4Tubs("NPOLCR1", fPolCradleIR1, fPolCradleOR, fPolCradleThick/2, 180*CLHEP::deg, 180*CLHEP::deg); //Outer piece of cradle (holds tube)
  G4Tubs* npolcr2 = new G4Tubs("NPOLCR2", fPolCradleIR2, fPolCradleOR, fPolCradleThick/2, 180*CLHEP::deg, 180*CLHEP::deg); // Inner piece of cradle (Attaches to rods)
  G4Tubs* npolcrrod = new G4Tubs("NPOLCRROD", 0, 6*CLHEP::mm, fPolCradleSep/2, 0*CLHEP::deg, 360*CLHEP::deg); // Silver steel rods
  G4Tubs* npolc1 = new G4Tubs("NPOLC1", 25.4*CLHEP::mm, 45*CLHEP::mm, fPolCradleThick/2, 0*CLHEP::deg, 180*CLHEP::deg); // Circular piece of clamp
  G4Box* npolc2 = new G4Box("NPOLC2", 25*CLHEP::mm, 10*CLHEP::mm, 6*CLHEP::mm); // Cuboidal section of clamp

  // Add clamp pieces together

  G4UnionSolid* npolc3 = new G4UnionSolid("NPOLC3", npolc1, npolc2, 0, G4ThreeVector ( -69*CLHEP::mm, 10*CLHEP::mm, 0*CLHEP::mm));
  G4UnionSolid* npolc4 = new G4UnionSolid("NPOLC4", npolc3, npolc2, 0, G4ThreeVector ( 69*CLHEP::mm, 10*CLHEP::mm, 0*CLHEP::mm));

  // Define materials and colour for each component

  G4LogicalVolume* npolcr1Logic = new G4LogicalVolume(npolcr1 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLCR1"); // Set both cradle pieces to be made of Aluminium
  G4LogicalVolume* npolcr2Logic = new G4LogicalVolume(npolcr2 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLCR2");
  G4LogicalVolume* npolcrrodLogic = new G4LogicalVolume(npolcrrod ,G4NistManager::Instance()->FindOrBuildMaterial("A2_SilverSteel"),"NPOLCRROD"); // Set rods to be made of silver steel
  G4LogicalVolume* npolcLogic = new G4LogicalVolume (npolc4, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLCLAMP"); // Set clamps to be made from Aluminium
  npolcr1Logic->SetVisAttributes(SupVisAtt); // Set colour of cradle pieces to green
  npolcr2Logic->SetVisAttributes(SupVisAtt);
  npolcrrodLogic->SetVisAttributes(SupVisAtt);
  npolcLogic->SetVisAttributes(SupVisAtt);

  // Place components into the volume

  G4VPhysicalVolume* npolcr1aPhysi = new G4PVPlacement(0, G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (fPolCradleThick/2)), npolcr1Logic, "NPOLCR1a", fMotherLogic, false, 999); //Place first large piece of cradle
  G4VPhysicalVolume* npolcr2aPhysi = new G4PVPlacement(0, G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (3*(fPolCradleThick/2))), npolcr2Logic, "NPOLCR2a", fMotherLogic, false, 999); //Place second piece of cradle
  G4VPhysicalVolume* npolcr2bPhysi = new G4PVPlacement(0, G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + fPolCradleSep + (5*(fPolCradleThick/2))), npolcr2Logic, "NPOLCR2b", fMotherLogic, false, 999);
  G4VPhysicalVolume* npolcr1bPhysi = new G4PVPlacement(0, G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + fPolCradleSep + (7*(fPolCradleThick/2))), npolcr1Logic, "NPOLCR1b", fMotherLogic, false, 999);
  G4VPhysicalVolume* npolcrord1Physi = new G4PVPlacement(0, G4ThreeVector(Xoff,Yoff-(71*CLHEP::mm), (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (2*fPolCradleThick) + (fPolCradleSep/2)), npolcrrodLogic, "NPOLCRROD1", fMotherLogic, false, 999); //Place first rod
  G4VPhysicalVolume* npolcrord2Physi = new G4PVPlacement(0, G4ThreeVector(Xoff-(71*CLHEP::mm),Yoff-(8*CLHEP::mm), (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (2*fPolCradleThick) + (fPolCradleSep/2)), npolcrrodLogic, "NPOLCRROD2", fMotherLogic, false, 999); //Place first rod
  G4VPhysicalVolume* npolcrord3Physi = new G4PVPlacement(0, G4ThreeVector(Xoff+(71*CLHEP::mm),Yoff-(8*CLHEP::mm), (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (2*fPolCradleThick) + (fPolCradleSep/2)), npolcrrodLogic, "NPOLCRROD3", fMotherLogic, false, 999); //Place first rod
  G4VPhysicalVolume* npolc1Physi = new G4PVPlacement(0, G4ThreeVector(Xoff, Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + (fPolCradleThick/2)), npolcLogic, "NPOLC1", fMotherLogic, false, 999); // Place first clamp
  G4VPhysicalVolume* npolc2Physi = new G4PVPlacement(0, G4ThreeVector(Xoff, Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolCradlePlacement + fPolCradleSep + (7*(fPolCradleThick/2))), npolcLogic, "NPOLC2", fMotherLogic, false, 999); // Place second clamp

}
