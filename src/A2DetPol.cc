#include "A2DetPol.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"

// Need to update options 2-5 with new support structures once decided upon

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
  //Build the phase 2 polarimeter (2015/2016) 1.5 cm thick

  fMotherLogic=MotherLogical;
  //some parameters
  Xoff=0*CLHEP::mm;
  Yoff=0*CLHEP::mm;     //original
  fPol_Z = 300*CLHEP::mm;
  fPol_rin = 51*CLHEP::mm;
  fPol_rout = 66*CLHEP::mm;
  fCap_rin = 30*CLHEP::mm;
  fPol_Thick = fPol_rout - fPol_rin;
  fPol_Z0 = Z0;
  fUseCap = PCap;

  if (PCap == 0){
    //Make the polarimeter shape
    fCapThick = 0*CLHEP::mm;
    MakeTube2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016) option 1 (1.5cm)"<<G4endl;
  }

  else if (PCap == 1){
    //Make the polarimeter shape
    fCapThick = fPol_Thick;
    MakeTube2();
    MakeCap2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016) option 1 (1.5cm) with cap"<<G4endl;
 }

return fMyPhysi; // Need to define this, what is it and why does it matter?

}

G4VPhysicalVolume* A2DetPol::Construct3(G4LogicalVolume* MotherLogical,G4double Z0, G4int PCap){
  //Build the phase 2 polarimeter (2015/2016) 2.6cm thick, this is as the polarimeter is in reality

  fMotherLogic=MotherLogical;
  //some parameters
  Xoff=0*CLHEP::mm;
  Yoff=0*CLHEP::mm;     //original
  fPol_Z = 300*CLHEP::mm;
  fPol_rin = 39*CLHEP::mm;
  fPol_rout = 65*CLHEP::mm;
  fCap_rin = 30*CLHEP::mm;
  fPol_Thick = fPol_rout - fPol_rin;
  fPol_Z0 = Z0;
  fUseCap = PCap;

  if (PCap == 0){
    //Make the polarimeter shape
    fCapThick = 0*CLHEP::mm;
    MakeTube2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016) option 2 (2.5cm)"<<G4endl;
  }

  else if (PCap == 1){
    //Make the polarimeter shape
    fCapThick = fPol_Thick;
    MakeTube2();
    MakeCap2();
    MakeSupports2();
    G4cout<<"Made Phase II Polarimeter (2015/2016) option 2 (2.5cm) with cap"<<G4endl;
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

  //Support end caps for carbon polarimeter

  fPol_HoleR = fPol_rout - (7*CLHEP::mm); // Radius of drilled hole for steel rods
  fPolSC_Thick = 10*CLHEP::mm; // Thickness of support caps on polarimeter
  fPolST_Thick = 2*CLHEP::mm; // Thickness of support tube
  G4Tubs* npolsc1=new G4Tubs("NPOLSC1",fCap_rin,fPol_rout,fPolSC_Thick/2,0*CLHEP::deg,360*CLHEP::deg);
  G4Tubs* SupHole=new G4Tubs("CapHole", 0*CLHEP::mm, 3.025*CLHEP::mm, (fPolSC_Thick/2) + 0.2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg); // Holes cut in support caps for steel rods
  G4SubtractionSolid *npolsc2=new G4SubtractionSolid("NPOLSC2",npolsc1,SupHole,new G4RotationMatrix(),G4ThreeVector(0*CLHEP::mm, fPol_HoleR, 0*CLHEP::mm ));
  G4SubtractionSolid *npolsc3=new G4SubtractionSolid("NPOLSC3",npolsc2,SupHole,new G4RotationMatrix(),G4ThreeVector(fPol_HoleR * cos(30*CLHEP::deg), -(fPol_HoleR * sin(30*CLHEP::deg)) , 0*CLHEP::mm));
  G4SubtractionSolid *npolsc4=new G4SubtractionSolid("NPOLSC4",npolsc3,SupHole,new G4RotationMatrix(),G4ThreeVector(-(fPol_HoleR * cos(30*CLHEP::deg)), -(fPol_HoleR * sin(30*CLHEP::deg)), 0*CLHEP::mm));
  G4LogicalVolume* npolscLogic=new G4LogicalVolume(npolsc4,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLSC");
  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.1,0.5,0.0));
  npolscLogic->SetVisAttributes(SupVisAtt);
  G4VPhysicalVolume* npolsc1Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (-(fPol_Z/2)) + fPol_Z0 - (fPolSC_Thick/2)),npolscLogic,"NPOLSC",fMotherLogic,false,999); // Cap at downstream (PMT) end
  G4VPhysicalVolume* npolsc2Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + (fPolSC_Thick/2)),npolscLogic,"NPOLSC",fMotherLogic,false,999); // Cap at upstream (TAPS) end

  //Support tube for polarimeter

  G4Tubs* npolst1 = new G4Tubs("NPOLST1", fCap_rin, fCap_rin + fPolST_Thick, 0.45*CLHEP::m, 0*CLHEP::deg, 360*CLHEP::deg); // 90cm long support tube
  G4LogicalVolume* npolstLogic=new G4LogicalVolume(npolst1,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"NPOLST"); // Build tube from aluminium
  npolstLogic->SetVisAttributes(SupVisAtt);
  G4VPhysicalVolume* npolst1Physi=new G4PVPlacement(0,G4ThreeVector(Xoff,Yoff, (fPol_Z/2) + fPol_Z0 + fCapThick + fPolSC_Thick + (0.45*CLHEP::m)),npolstLogic,"NPOLST",fMotherLogic,false,999);

}
