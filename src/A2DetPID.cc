#include "A2DetPID.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"

A2DetPID::A2DetPID(){
  fregionPID=NULL;
  fregionPID=new G4Region("PID");//allows seperate cuts to be defined for crystal
  // fZ0=0*cm;
  fNPids=24;//including dummies

  fPIDPhysi=new G4VPhysicalVolume*[fNPids];   //Crystal physical volumes
  for(G4int i=0;i<fNPids+1;i++) fPIDPhysi[i]=NULL;
  fLGPhysi=new G4VPhysicalVolume*[fNPids];   //Crystal physical volumes
  for(G4int i=0;i<fNPids+1;i++) fLGPhysi[i]=NULL;
  fTPMTPhysi=new G4VPhysicalVolume*[fNPids];   //Crystal physical volumes
  for(G4int i=0;i<fNPids+1;i++) fTPMTPhysi[i]=NULL;
  fBASEPhysi=new G4VPhysicalVolume*[fNPids];   //Crystal physical volumes
  for(G4int i=0;i<fNPids+1;i++) fBASEPhysi[i]=NULL;
  fMUMEPhysi=new G4VPhysicalVolume*[fNPids];   //Crystal physical volumes
  for(G4int i=0;i<fNPids+1;i++) fMUMEPhysi[i]=NULL;
  fUPS1Logic=NULL;
  fUPS2Logic=NULL;
  fUPS3Logic=NULL;
  fPMTRLogic=NULL;
  fBRTULogic=NULL;
    
  fPIDSD=NULL;
}

A2DetPID::~A2DetPID(){
  //delete Rot;
}
G4VPhysicalVolume* A2DetPID::Construct1(G4LogicalVolume* MotherLogical,G4double Z0){
  //Build the original PID from 2003

  fMotherLogic=MotherLogical;
  
  //some parameters
  //Note it is full length not half length for this constructor!
  fzpos=0*CLHEP::cm;
  fpid_z=31.49*CLHEP::cm;
  fpid_rin=4.785*CLHEP::cm;
  fpid_thick=0.2*CLHEP::cm;
  fpid_rout=fpid_rin+fpid_thick;
  fpid_theta=360*CLHEP::deg/fNPids;
  fpid_xs=2*fpid_rin*tan(fpid_theta/2);//short lenght
  fpid_xl=2*fpid_rout*tan(fpid_theta/2);//long length

  fpmtr_z=20.565*CLHEP::cm;//zposition of the pmt supportring

  //Make the light guide shape
  MakeLightGuide();
  MakePhotomultipliers();
  //Make a single scint. detector
  MakeSingleDetector();
  MakeSupports1();

  //Make PID Logical Volume
  //Take the centre radius from the scintillators, thickness from the lightguides~1cm, and length from scintillators+lightguides+pmts+base
  //G4double moth_rin=fpid_rin+fpid_thick/2-8*mm;
  G4double moth_rin=4.585*CLHEP::cm-1*CLHEP::mm;// changed dglazier 26/01/09
  //G4double moth_rout=fpid_rin+fpid_thick/2+1*CLHEP::cm;
  G4double moth_rout=5.55*CLHEP::cm;//brass ring
  G4double moth_z=fpid_z+flg_z-flg12_z+fpmt_z*2+fbase_z*2+10*CLHEP::mm;//extra 10mm to for supports
  fzpos=(fpid_z-moth_z)/2+6*CLHEP::mm;//zposition of centre of pid relative to mother, 3mm is for supposrt ring
  G4RotationMatrix *Mrot=new G4RotationMatrix();
  G4Tubs *PIDMother=new G4Tubs("PIDD",moth_rin,moth_rout,moth_z/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(PIDMother,fNistManager->FindOrBuildMaterial("G4_AIR"),"PIDD");
  fMyPhysi =new G4PVPlacement(Mrot,G4ThreeVector(0,0,Z0-fzpos),fMyLogic,"PIDD",fMotherLogic,false,1);
  MakeDetector();

  fMyLogic->SetVisAttributes (G4VisAttributes::GetInvisible());
 
  return fMyPhysi;
}
G4VPhysicalVolume* A2DetPID::Construct2(G4LogicalVolume* MotherLogical,G4double Z0){
  //Build the new PID for 2007

  fMotherLogic=MotherLogical;
  //some parameters
  //Note it is full length not half length for this G4Trd constructor!
  fzpos=0*CLHEP::cm;
  fpid_z=50.00*CLHEP::cm;
  fpid_rin=11.65/2*CLHEP::cm;
  fpid_thick=0.4*CLHEP::cm;
  fpid_rout=fpid_rin+fpid_thick;
  fpid_theta=360*CLHEP::deg/fNPids;
  fpid_xs=2*fpid_rin*tan(fpid_theta/2);//short lenght
  fpid_xl=2*fpid_rout*tan(fpid_theta/2);//long length

 
  //Make the light guide shape
  MakeLightGuide();
  MakePhotomultipliers();
  //Make a single scint. detector
  MakeSingleDetector();
  MakeSupports2();

  //Make PID Logical Volume
  //Take the centre radius from the scintillators, thickness from the lightguides~1CLHEP::cm, and length from scintillators+lightguides+pmts+base
  // G4double moth_rin=fpid_rin+fpid_thick/2-8*mm;
  G4double moth_rin=54.2-0.1*CLHEP::mm; //aliminium ring
  //  G4double moth_rout=fpid_rin+fpid_thick/2+0.55*CLHEP::cm;
  G4double moth_rout=66.31*CLHEP::mm; //aliminium ring chnaged dglazier 26/01/09
  G4double moth_z=fpid_z+flg_z-flg12_z+fpmt_z*2+fbase_z*2+10*CLHEP::mm;//extra 6mm for supports
  fzpos=(fpid_z-moth_z)/2+6*CLHEP::mm;//zposition of centre of pid relative to mother, 3mm is for support ring
  fpmtr_z=fzpos+fpid_z/2+flg_z-flg12_z+2*fpmt_z+2*fbase_z-5/2*CLHEP::mm;//zposition of the pmt supportring
  G4RotationMatrix *Mrot=new G4RotationMatrix();
  Mrot->rotateY(180*CLHEP::deg);//pid2 is positioned in opposite orientation
  G4Tubs *PIDMother=new G4Tubs("PIDD",moth_rin,moth_rout,moth_z/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(PIDMother,fNistManager->FindOrBuildMaterial("G4_AIR"),"PIDD");
  //Note here position is +fzpos beause of rotation
  fMyPhysi =new G4PVPlacement(Mrot,G4ThreeVector(0,0,Z0+fzpos),fMyLogic,"PIDD",fMotherLogic,false,1);
  MakeDetector();
  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(0.5,0.5,1,1));
  visatt->SetForceWireframe(true);
  fMyLogic->SetVisAttributes(visatt);

  //  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);

  return fMyPhysi;
}
void A2DetPID::MakeDetector(){
 G4RotationMatrix** Rot=new G4RotationMatrix*[fNPids];;
 G4RotationMatrix** RotPMTR=new G4RotationMatrix*[fNPids];
  for(G4int i=0;i<fNPids;i++){
   // for(G4int i=0;i<1;i++){
    G4double pid_angle=fpid_theta*i;
    G4double pid_R=fpid_rin+fpid_thick/2; //radius to centre of scintillator
    G4double xpos=pid_R*cos(pid_angle);
    G4double ypos=pid_R*sin(pid_angle);
    Rot[i]=new G4RotationMatrix();
    Rot[i]->rotateZ(270*CLHEP::deg-pid_angle-fpid_theta/2);
    G4ThreeVector dpos1(xpos,ypos,fzpos);
    dpos1.rotateZ(fpid_theta/2); //rotate so flat at top
                 //this matches the pid positions with PID_MC.dat
    //Check the hit positions, should collate with AcquRoot setup 
    //G4cout<<"PID "<<i<<" "<<xpos<< " "<<ypos<<" "<<" "<<pid_R<<" "<<fzpos<<" "<<dpos1.phi()/CLHEP::deg<<G4endl;
    fPIDPhysi[i]=new G4PVPlacement(Rot[i],dpos1,fPIDLogic,"PID",fMyLogic,false,i+1);
     fLGPhysi[i]=new  G4PVPlacement(Rot[i],G4ThreeVector(xpos,ypos,fzpos+fpid_z/2+flg_z-flg12_z).rotateZ(fpid_theta/2),fLGLogic,"LG",fMyLogic,false,i);
    fBASEPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos,ypos,fzpos+fpid_z/2+flg_z-flg12_z+2*fpmt_z+fbase_z).rotateZ(fpid_theta/2),fBASELogic,"BASE",fMyLogic,false,i);
     fTPMTPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos,ypos,fzpos+fpid_z/2+flg_z-flg12_z+fpmt_z).rotateZ(fpid_theta/2),fTPMTLogic,"TPMT",fMyLogic,false,i);
      fMUMEPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos,ypos,fzpos+fpid_z/2+flg_z-flg12_z+fpmt_z+fbase_z).rotateZ(fpid_theta/2),fMUMELogic,"MUME",fMyLogic,false,i);
    RotPMTR[i]=new G4RotationMatrix();
    RotPMTR[i]->rotateZ(pid_angle);
    new G4PVPlacement(RotPMTR[i],G4ThreeVector(0,0,fpmtr_z),fPMTRLogic,"PMTR",fMyLogic,false,i);
    
 }
  new G4PVPlacement(0,G4ThreeVector(0,0,fzpos-(fpid_z/2-3/2*CLHEP::mm)),fUPS1Logic,"UPS1",fMyLogic,false,101); 
  new G4PVPlacement(0,G4ThreeVector(0,0,fzpos-(fpid_z/2+3/2*CLHEP::mm)),fUPS2Logic,"UPS2",fMyLogic,false,102); 
  new G4PVPlacement(0,G4ThreeVector(0,0,fzpos-(fpid_z/2+3*CLHEP::mm)),fUPS3Logic,"UPS3",fMyLogic,false,103);
  //only place brass tube for old pid
  if(fBRTULogic) new G4PVPlacement(0,G4ThreeVector(0,0,-19.575*CLHEP::cm),fBRTULogic,"UPS1",fMyLogic,false,104); 

}
void A2DetPID::MakeSingleDetector(){
 //Constructor for right angular wedge!
  fPID=new G4Trap("PID",fpid_z,fpid_thick,fpid_xl,fpid_xs);
  fPIDLogic=new G4LogicalVolume(fPID,fNistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"PID");
  if(!fPIDSD){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fPIDSD = new A2SD("PIDSD",fNPids);
    SDman->AddNewDetector( fPIDSD );		
  }
  fPIDLogic->SetSensitiveDetector(fPIDSD);
  fregionPID->AddRootLogicalVolume(fPIDLogic);

  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(0.5,0.5,1,1));
  //visatt->SetForceWireframe(true);
  fPIDLogic->SetVisAttributes(visatt);
}
void A2DetPID::MakePhotomultipliers(){

  //note sasha uses fglass with an inner radius of 0.24CLHEP::cm
  fpmt_z=2.315*CLHEP::cm;//half lenght
  G4double pmt_rin=0.44*CLHEP::cm;
  G4double pmt_rout=0.48*CLHEP::cm;
  fTPMT=new G4Tubs("TPMT",pmt_rin,pmt_rout,fpmt_z,0*CLHEP::deg,360*CLHEP::deg);
  fTPMTLogic=new G4LogicalVolume(fTPMT,fNistManager->FindOrBuildMaterial("A2_FGLASS"),"TPMT");

  fbase_z=2.3*CLHEP::cm;
  G4double base_rin=0.0*CLHEP::cm;
  G4double base_rout=0.50*CLHEP::cm;
  fBASE=new G4Tubs("BASE",base_rin,base_rout,fbase_z,0*CLHEP::deg,360*CLHEP::deg);
  fBASELogic=new G4LogicalVolume(fBASE,fNistManager->FindOrBuildMaterial("A2_BASEMAT"),"BASE");

  fmume_z=4.615*CLHEP::cm;
  G4double mume_rin=0.548*CLHEP::cm;//NEED TO CHECK Should be 0.5?
  G4double mume_rout=0.55*CLHEP::cm;
  fMUME=new G4Tubs("MUME",mume_rin,mume_rout,fmume_z,0*CLHEP::deg,360*CLHEP::deg);
  fMUMELogic=new G4LogicalVolume(fMUME,fNistManager->FindOrBuildMaterial("A2_MUMETAL"),"BASE");

  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(0,0.4,1,1));
  fMUMELogic->SetVisAttributes(visatt);
  fBASELogic->SetVisAttributes(visatt);
  fTPMTLogic->SetVisAttributes(visatt);

}
void A2DetPID::MakeLightGuide(){

  //Made up of 3 wedges and a tub
  //Note it is full length not half length for this constructor!
  G4double lg12_y=2*0.05*CLHEP::cm;
  flg12_z=2*0.4*CLHEP::cm;
  //longer one (LGFO)
  G4double lg1_xl=fpid_xl+lg12_y*tan(fpid_theta/2);
  G4double lg1_xs=fpid_xl;
  //shorter one (LGFI)
  G4double lg2_xl=fpid_xs;
  G4double lg2_xs=fpid_xs-lg12_y*tan(fpid_theta/2);
  //top one (LGFM)
  //only approximate shape
  G4double lg3_z=0.15*CLHEP::cm;//half length for G4Trd constructor!
  G4double lg3_y=(fpid_thick+2*lg12_y)/2;
  G4double lg3_xtop=0.485*CLHEP::cm;//also radius od tub
  G4double lg3_xbot=fpid_xl/2;
  //tub (LGTU)
  G4double lg4_z=0.1*CLHEP::cm;
  // G4double lg4_r=lg3_xbot;
  G4double lg4_r=9.7/2;//change dglazier 26/01/09, too big before!

  flg_z=2*lg3_z+lg4_z+flg12_z; //distance from z=0 (cenre of LGTU) to tip

  fLGFO=new G4Trap("LGFO",flg12_z,lg12_y,lg1_xl,lg1_xs);
  fLGFI=new G4Trap("LGFO",flg12_z,lg12_y,lg2_xl,lg2_xs);
  fLGFM=new G4Trd("LGFO",lg3_xbot,lg3_xtop,lg3_y,lg3_y,lg3_z);
  fLGTU=new G4Tubs("LGTU",0.,lg4_r,lg4_z,0.,360*CLHEP::deg);

  fLG1=new G4UnionSolid("LG1",fLGTU,fLGFM,0,G4ThreeVector(0,0,-(lg4_z+lg3_z)));
  fLG2=new G4UnionSolid("LG2",fLG1,fLGFO,0,G4ThreeVector(0,-(0.5*fpid_thick+0.5*lg12_y),-(lg4_z+2*lg3_z+flg12_z/2)));
  fLG3=new G4UnionSolid("LG3",fLG2,fLGFI,0,G4ThreeVector(0,(0.5*fpid_thick+0.5*lg12_y),-(lg4_z+2*lg3_z+flg12_z/2)));
  fLGLogic=new G4LogicalVolume(fLG3,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"LG1");
  //fLGPhysi=new G4PVPlacement(0,G4ThreeVector(),fLGLogic,"LG",fMotherLogic,false,1);

  G4VisAttributes* lg_visatt=new G4VisAttributes();
  lg_visatt->SetColor(G4Color(0,0,1));
  fLGLogic->SetVisAttributes(lg_visatt);
}


void A2DetPID::MakeSupports1(){
  //c Brass tube at upstream end
  // note only for PID1
  G4Tubs* BRTU=new G4Tubs("BRTU",5.455*CLHEP::cm,5.550*CLHEP::cm,77.5/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
  fBRTULogic=new G4LogicalVolume(BRTU,fNistManager->FindOrBuildMaterial("A2_BRASS"),"BRTU");

  //c Aluminium ring at upstream end, 
  //c Note UPS1 should have a more complex, 24 sided outer structure 
  //c to match the shape of the inside of the PMT barrel.  The thickest 
  //c part of this structure would however only be 2.04mm and therefore 
  //c the extra complexity would add little to the accuracy and also would 
  //c complicate the tracking.  
  G4Tubs* UPS1=new G4Tubs("UPS1",4.585*CLHEP::cm,4.785*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
  fUPS1Logic=new G4LogicalVolume(UPS1,fNistManager->FindOrBuildMaterial("G4_Al"),"UPS1");
  //c UPS2 is the central part of the aluminium upstream support ring (with
  //c the largest diameter) which holds it in place in the brass tube.
   G4Tubs* UPS2=new G4Tubs("UPS2",4.585*CLHEP::cm,5.35*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
   fUPS2Logic=new G4LogicalVolume(UPS2,fNistManager->FindOrBuildMaterial("G4_Al"),"UPS2");
  //c UPS3 is the sloping edge of the upstream aluminium support ring 
  //c which allows us to locate it easily into the brass tube.
   G4Cons* UPS3=new G4Cons("UPS3",4.585*CLHEP::cm,5.05*CLHEP::cm,4.585*CLHEP::cm,5.35*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    fUPS3Logic=new G4LogicalVolume(UPS3,fNistManager->FindOrBuildMaterial("G4_Al"),"UPS3");
    //c Aluminium ring for holding PMTs at the downstream end of the detector.
    G4Tubs* solidPMTR=new G4Tubs("solidPMTR",41.35*CLHEP::mm,55.35*CLHEP::mm,5.*CLHEP::mm,0*CLHEP::deg,fpid_theta);
    G4Tubs* subPMTR=new G4Tubs("subPMTR",0*CLHEP::mm,11/2*CLHEP::mm,5*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    G4double R=fpid_rin+fpid_thick/2;
    G4SubtractionSolid *PMTR=new G4SubtractionSolid("PMTR",solidPMTR,subPMTR,new G4RotationMatrix(),G4ThreeVector(R*cos(fpid_theta/2),R*sin(fpid_theta/2),0*CLHEP::cm));
    fPMTRLogic=new G4LogicalVolume(PMTR,fNistManager->FindOrBuildMaterial("G4_Al"),"PMTR");
    //rest Done in MakeDetector so can subtract off the holes!

  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.7,0.4,0.6));
  //CBVisAtt->SetVisibility(false);
  fUPS1Logic->SetVisAttributes(SupVisAtt);
  fUPS2Logic->SetVisAttributes(SupVisAtt);
  fUPS3Logic->SetVisAttributes(SupVisAtt);
  fPMTRLogic->SetVisAttributes(SupVisAtt);


}
void A2DetPID::MakeSupports2(){
 
  //c Aluminium ring at upstream end, 
  //c Note UPS1 should have a more complex, 24 sided outer structure 
  //c to match the shape of the inside of the PMT barrel.  The thickest 
  //c part of this structure would however only be 2.04mm and therefore 
  //c the extra complexity would add little to the accuracy and also would 
  //c complicate the tracking.  
  G4Tubs* UPS1=new G4Tubs("UPS1",5.625*CLHEP::cm,5.825*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
  fUPS1Logic=new G4LogicalVolume(UPS1,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS1");
  //c UPS2 is the central part of the aluminium upstream support ring (with
  //c the largest diameter) which holds it in place in the brass tube.
   G4Tubs* UPS2=new G4Tubs("UPS2",5.625*CLHEP::cm,6.39*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
   fUPS2Logic=new G4LogicalVolume(UPS2,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS2");
  //c UPS3 is the sloping edge of the upstream aluminium support ring 
  //c which allows us to locate it easily into the brass tube.
   G4Cons* UPS3=new G4Cons("UPS3",5.625*CLHEP::cm,6.09*CLHEP::cm,5.625*CLHEP::cm,6.39*CLHEP::cm,3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    fUPS3Logic=new G4LogicalVolume(UPS3,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS3");
    //c Aluminium ring for holding PMTs at the downstream end of the detector.
    G4Tubs* solidPMTR=new G4Tubs("solidPMTR",54.2*CLHEP::mm,66.3*CLHEP::mm,5.*CLHEP::mm,0*CLHEP::deg,fpid_theta);
    G4Tubs* subPMTR=new G4Tubs("subPMTR",0*CLHEP::mm,11/2*CLHEP::mm,5*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    G4double R=fpid_rin+fpid_thick/2;
    G4SubtractionSolid *PMTR=new G4SubtractionSolid("PMTR",solidPMTR,subPMTR,new G4RotationMatrix(),G4ThreeVector(R*cos(fpid_theta/2),R*sin(fpid_theta/2),0*CLHEP::cm));
    fPMTRLogic=new G4LogicalVolume(PMTR,fNistManager->FindOrBuildMaterial("G4_Al"),"PMTR");
    //rest Done in MakeDetector so can subtract off the holes!

  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.7,0.4,0.6));
  //CBVisAtt->SetVisibility(false);
  fUPS1Logic->SetVisAttributes(SupVisAtt);
  fUPS2Logic->SetVisAttributes(SupVisAtt);
  fUPS3Logic->SetVisAttributes(SupVisAtt);
  fPMTRLogic->SetVisAttributes(SupVisAtt);


}
