#include "A2DetPID3.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"

// This is the src code currently used to construct PID3
// Due to an issue with volume overlaps the PMTs, lightguides and support strcuture are currently NOT created
// Relevant lines have been commented out below
// Including these appears to cause the polarimeter to become non interactive

A2DetPID3::A2DetPID3(){
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

A2DetPID3::~A2DetPID3(){
  //delete Rot;
}

G4VPhysicalVolume* A2DetPID3::Construct1(G4LogicalVolume* MotherLogical,G4double Z0){
  //Build the new mini PID-III for 2015/2016 that allows use of MWPC and polarimeter
  //This is the fourth option for the phase 2 polarimeter where the PID is 30cm long with inner radius 3.3cm
  //This allows for the use of an "end cap" on the polarimeter
  //This option is displaced from the target center and will requires the use of the 3.1cm radius target
  //Need to alter support structure AND also add bent lightguides

  fMotherLogic=MotherLogical;
  //some parameters
  //Note it is full length not half length for this G4Trd constructor!
  fpidendL = 1*CLHEP::cm;
  fzpos= 0*CLHEP::cm;
  fpid_z=30*CLHEP::cm; // Requires 1.4 cm displacement of PID! (Will change depending on LG!
  fpid_rin=3.3*CLHEP::cm;
  fpid_thick=0.4*CLHEP::cm;
  fpid_rout=fpid_rin+fpid_thick;
  fpid_theta=360*CLHEP::deg/fNPids;
  fpid_xs=2*fpid_rin*tan(fpid_theta/2);//short length
  fpid_xl=2*fpid_rout*tan(fpid_theta/2);//long length
  fpid_xs2=2*(fpid_rin-(fpidendL*tan(30*CLHEP::deg)))*tan(fpid_theta/2);//short length at new inner radius
  fpid_xl2=2*(fpid_rout-(fpidendL*tan(30*CLHEP::deg)))*tan(fpid_theta/2);//long length at new inner radius

  //Make the light guide shape
  //MakeLightGuide1(); // Makes the bent light guides for the smaller PID case Uncomment if desired, see other lines later too
  //MakePhotomultipliers(); // This is unchanged since last two PIDs Uncomment if desired

  MakeSingleDetector1();

  //MakeSupports1(); // New support structure Uncomment if desired

  //Make PID Logical Volume
  //Take the centre radius from the scintillators, thickness from the lightguides~1CLHEP::cm, and length from scintillators+lightguides+pmts+base
  // G4double moth_rin=fpid_rin+fpid_thick/2-8*mm;
  G4double moth_rin=(fpid_rin - 4.05*CLHEP::mm)-0.1*CLHEP::mm; //aluminium ring
  //  G4double moth_rout=fpid_rin+fpid_thick/2+0.55*CLHEP::cm;
  //G4double moth_rout=(66*CLHEP::mm); //aluminium ring Original values with supports, turn back on if supports commented back in
  //G4double moth_z=fpid_z+flg_z-flg12_z+fpmt_z*2+fbase_z*2+10*CLHEP::mm + fpidendL;//extra 6mm for supports
  G4double moth_rout=(38*CLHEP::mm);
  G4double moth_z=fpid_z+5*CLHEP::mm;//extra 6mm for supports
  //fzpos=((fpid_z-moth_z)/2+6*CLHEP::mm)+fpidendL;//zposition of centre of pid relative to mother, 3mm is for support ring Turn back on if supports commented back in
  fzpos=2.5*CLHEP::mm;//zposition of centre of pid relative to mother
  //fpmtr_z=fzpos+fpid_z/2+flg_z-flg12_z+2*fpmt_z+2*fbase_z-5/2*CLHEP::mm;//zposition of the pmt supportring
  G4RotationMatrix *Mrot=new G4RotationMatrix();
  Mrot->rotateY(180*CLHEP::deg);//PID III is positioned in same orientation as PID II
  G4Tubs *PIDMother=new G4Tubs("PIDD",moth_rin-(fpidendL*tan(30*CLHEP::deg)),moth_rout,moth_z/2,0*CLHEP::deg,360*CLHEP::deg);
  fMyLogic=new G4LogicalVolume(PIDMother,fNistManager->FindOrBuildMaterial("G4_AIR"),"PIDD");
  //Note here position is +fzpos beause of rotation
  fMyPhysi =new G4PVPlacement(Mrot,G4ThreeVector(0,0,Z0+fzpos),fMyLogic,"PIDD",fMotherLogic,false,1);
  MakeDetector1();
  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(0.5,0.5,1,1));
  visatt->SetForceWireframe(true);
  fMyLogic->SetVisAttributes(visatt);

  //  fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);

  return fMyPhysi;
}

void A2DetPID3::MakeDetector1(){
    // Make detector fn for when PMT support ring is at different radius to other support ring (case 4)
 G4RotationMatrix** Rot=new G4RotationMatrix*[fNPids];;
 G4RotationMatrix** RotPMTR=new G4RotationMatrix*[fNPids];
  for(G4int i=0;i<fNPids;i++){
   // for(G4int i=0;i<1;i++){
    G4double pid_angle=fpid_theta*i;
    G4double pid_R=fpid_rin+fpid_thick/2; //radius to centre of scintillator
    G4double xpos=pid_R*cos(pid_angle);
    G4double ypos=pid_R*sin(pid_angle);
    G4double xpos2=(pid_R+(24.1*CLHEP::mm))*cos(pid_angle);
    G4double ypos2=(pid_R+(24.1*CLHEP::mm))*sin(pid_angle);
    Rot[i]=new G4RotationMatrix();
    Rot[i]->rotateZ(270*CLHEP::deg-pid_angle-fpid_theta/2);
    G4ThreeVector dpos1(xpos,ypos,fzpos);
    dpos1.rotateZ(fpid_theta/2); //rotate so flat at top
                 //this matches the pid positions with PID_MC.dat
    //Check the hit positions, should collate with AcquRoot setup
    //G4cout<<"PID "<<i<<" "<<xpos<< " "<<ypos<<" "<<" "<<pid_R<<" "<<fzpos<<" "<<dpos1.phi()/CLHEP::deg<<G4endl;
    fPIDPhysi[i]=new G4PVPlacement(Rot[i],dpos1,fPIDLogic,"PID",fMyLogic,false,i+1);
    // Uncomment lines below if supports turned back on
     //fLGPhysi[i]=new  G4PVPlacement(Rot[i],G4ThreeVector(xpos2,ypos2,fzpos+fpid_z/2+flg_z-flg12_z).rotateZ(fpid_theta/2),fLGLogic,"LG",fMyLogic,false,i);
    //fBASEPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos2,ypos2,fzpos+fpid_z/2+flg_z-flg12_z+2*fpmt_z+fbase_z).rotateZ(fpid_theta/2),fBASELogic,"BASE",fMyLogic,false,i);
     //fTPMTPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos2,ypos2,fzpos+fpid_z/2+flg_z-flg12_z+fpmt_z).rotateZ(fpid_theta/2),fTPMTLogic,"TPMT",fMyLogic,false,i);
      //fMUMEPhysi[i]=new  G4PVPlacement(0,G4ThreeVector(xpos2,ypos2,fzpos+fpid_z/2+flg_z-flg12_z+fpmt_z+fbase_z).rotateZ(fpid_theta/2),fMUMELogic,"MUME",fMyLogic,false,i);
    //RotPMTR[i]=new G4RotationMatrix();
    //RotPMTR[i]->rotateZ(pid_angle);
    //new G4PVPlacement(RotPMTR[i],G4ThreeVector(0,0,fpmtr_z),fPMTRLogic,"PMTR",fMyLogic,false,i);

 }

}

void A2DetPID3::MakeSingleDetector1(){
 //Used for PID III (both options) to add on bent bit at end of PID scintillators
  fPID=new G4Trap("PID",fpid_z,fpid_thick,fpid_xl,fpid_xs);
  //Commented out PIDEnd is for slanted/angled PID end pieces
  //fPIDEnd=new G4Trap("PIDEnd", ((fpidendL)/2), 30*CLHEP::deg, 270*CLHEP::deg, (fpid_thick/2), (fpid_xl2/2), (fpid_xs2/2), 0*CLHEP::deg, (fpid_thick/2), (fpid_xl/2), (fpid_xs/2), 0*CLHEP::deg); // Define shape of bit at end
  fPIDEnd = new G4Trap("PIDEnd", ((fpid_thick)), ((fpidendL)) , (fpid_xs), 5.99*CLHEP::mm);
  fPIDF=new G4UnionSolid("PIDFinal", fPID, fPIDEnd, 0,  G4ThreeVector (0, (fpid_thick+fpidendL)/2, -((fpid_z)/2)+((fpid_thick)/2)));
  fPIDLogic=new G4LogicalVolume(fPIDF,fNistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"PID");
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

void A2DetPID3::MakePhotomultipliers(){

  //note sasha uses fglass with an inner radius of 0.24CLHEP::cm
  fpmt_z=2.315*CLHEP::cm;//half length
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

void A2DetPID3::MakeLightGuide1(){

  //Bent light guides for use with PID III Option 2 (3.1cm rin)
  G4double lg12_y=2*0.05*CLHEP::cm;
  flg12_z=2*0.4*CLHEP::cm;
  //longer one (LGFO)
  G4double lg1_xl=fpid_xl+lg12_y*tan(fpid_theta/2);
  G4double lg1_xs=fpid_xl;
  //shorter one (LGFI)
  G4double lg2_xl=fpid_xs;
  G4double lg2_xs=fpid_xs-lg12_y*tan(fpid_theta/2);
  G4double lg3_z=2.1*CLHEP::cm;//half length for G4Trd constructor!
  G4double lg3_y=(fpid_thick+2*lg12_y)/2;
  G4double lg3_xbot=fpid_xl/2;
  //tub (LGTU)
  G4double lg4_z=0.1*CLHEP::cm;
  G4double lg4_r=9.7/2;//change dglazier 26/01/09, too big before!

  flg_z=2*lg3_z+lg4_z+flg12_z; //distance from z=0 (cenre of LGTU) to tip

  fLGFO=new G4Trap("LGFO",flg12_z,50*lg12_y,lg1_xl,lg1_xs);
  fLGFI=new G4Trap("LGFO",flg12_z,50*lg12_y,lg2_xl,lg2_xs);
  fLGTU=new G4Tubs("LGTU",0.,lg4_r,lg4_z,0.,360*CLHEP::deg);
  // Main bit of lightguide is parallelepiped, 4.2cm long angled at 30 degrees (also needs to be rotated a bit)
  fLGB1 = new G4Para("LGB3", lg3_xbot, lg3_y, lg3_z, 0.*CLHEP::deg, 150.*CLHEP::deg, 90.*CLHEP::deg); // This shape LOOKS fine but does not fit the PID elements correctly currently

  fLG1=new G4UnionSolid("LG1",fLGTU,fLGB1,0,G4ThreeVector(0,1.2*CLHEP::cm,-(lg4_z+lg3_z))); // Not sure WHY y parameter needs to be 1 but this seems to work so...
  //fLG2=new G4UnionSolid("LG2",fLG1,fLGFO,0,G4ThreeVector(0,((2*lg12_y)+(tan(30)*2*lg3_z))*CLHEP::cm,-(lg4_z+2*lg3_z+flg12_z/2)));
  //fLG3=new G4UnionSolid("LG3",fLG2,fLGFI,0,G4ThreeVector(0,(tan(30)*2*lg3_z)*CLHEP::cm,-(lg4_z+2*lg3_z+flg12_z/2)));
  fLGLogic=new G4LogicalVolume(fLG1,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"LG1");
  //fLGPhysi=new G4PVPlacement(0,G4ThreeVector(),fLGLogic,"LG",fMotherLogic,false,1);

  G4VisAttributes* lg_visatt=new G4VisAttributes();
  lg_visatt->SetColor(G4Color(0,0,1));
  fLGLogic->SetVisAttributes(lg_visatt);
}

void A2DetPID3::MakeSupports1(){

  //PID-III Supports, crown not included

  G4Tubs* UPS1=new G4Tubs("UPS1",(fpid_rin - 2*CLHEP::mm),(fpid_rin*CLHEP::mm),3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
  fUPS1Logic=new G4LogicalVolume(UPS1,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS1");
  //c UPS2 is the central part of the aluminium upstream support ring (with
  //c the largest diameter) which holds it in place in the brass tube.
   G4Tubs* UPS2=new G4Tubs("UPS2",(fpid_rin - 2*CLHEP::mm),(fpid_rin + 5.65*CLHEP::mm),3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
   fUPS2Logic=new G4LogicalVolume(UPS2,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS2");
  //c UPS3 is the sloping edge of the upstream aluminium support ring
  //c which allows us to locate it easily into the brass tube.
   G4Cons* UPS3=new G4Cons("UPS3",(fpid_rin - 2*CLHEP::mm),(fpid_rin + 2.65*CLHEP::mm),(fpid_rin - 2*CLHEP::mm),(fpid_rin + 5.65*CLHEP::mm),3/2*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    fUPS3Logic=new G4LogicalVolume(UPS3,fNistManager->FindOrBuildMaterial("A2_PLASTIC"),"UPS3");
    //c Aluminium ring for holding PMTs at the downstream end of the detector.
    G4Tubs* solidPMTR=new G4Tubs("solidPMTR",(54*CLHEP::mm), 65.6*CLHEP::mm,5.*CLHEP::mm,0*CLHEP::deg,fpid_theta);
    G4Tubs* subPMTR=new G4Tubs("subPMTR",0*CLHEP::mm,11/2*CLHEP::mm,5*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
    G4SubtractionSolid *PMTR=new G4SubtractionSolid("PMTR",solidPMTR,subPMTR,new G4RotationMatrix(),G4ThreeVector(59.1*CLHEP::mm,8*CLHEP::mm,0*CLHEP::cm));
    fPMTRLogic=new G4LogicalVolume(PMTR,fNistManager->FindOrBuildMaterial("G4_Al"),"PMTR");
    //rest Done in MakeDetector so can subtract off the holes!

  G4VisAttributes* SupVisAtt= new G4VisAttributes(G4Colour(0.7,0.4,0.6));
  //CBVisAtt->SetVisibility(false);
  fUPS1Logic->SetVisAttributes(SupVisAtt);
  fUPS2Logic->SetVisAttributes(SupVisAtt);
  fUPS3Logic->SetVisAttributes(SupVisAtt);
  fPMTRLogic->SetVisAttributes(SupVisAtt);

}
