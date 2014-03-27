#include "A2DetTAPS.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"

A2DetTAPS::A2DetTAPS(){
  fregionTAPS=NULL;
  fregionTAPS=new G4Region("TAPS");//allows seperate cuts to be defined for crystal
  fregionTAPSV=NULL;
  fregionTAPSV=new G4Region("TAPSV");//allows seperate cuts to be defined for vetos
  //Default constructor (2003 settings)
  fNTaps=552;//including dummies
  fZ0=175*cm;
  fNPbWORings=0;
  fSetupFile="taps.dat";
  fCOVRPhysi=new G4VPhysicalVolume*[fNTaps+1];   //Crystal physical volumes
  fNPbWO4=72;
//   fCOVRPbPhysi=new G4VPhysicalVolume*[fNPbWO4];   //Crystal physical volumes
//   for(G4int i=0;i<72;i++) fCOVRPbPhysi[i]=NULL;
  for(G4int i=0;i<fNTaps+1;i++) fCOVRPhysi[i]=NULL;
  fTVETPhysi=new G4VPhysicalVolume*[fNTaps+1];   //Veto physical volumes
  for(G4int i=0;i<fNTaps+1;i++) fTVETPhysi[i]=NULL;
  fNRealTaps=510;
  fDUMMPhysi=new G4VPhysicalVolume*[fNTaps-fNRealTaps];   //Veto physical volumes
  for(G4int i=0;i<fNTaps-fNRealTaps;i++) fDUMMPhysi[i]=NULL;
  
  fTAPSSD=NULL;
  fTAPSVSD=NULL;
  fTAPSVisSD=NULL;
  fTAPSVVisSD=NULL;
}

A2DetTAPS::A2DetTAPS(G4String setupfile, G4int Ntaps, G4int NPbWORings,G4double tZ){
  fregionTAPS=NULL;
  fregionTAPS=new G4Region("TAPS");//allows seperate cuts to be defined for crystal
  fregionTAPSV=NULL;
  fregionTAPSV=new G4Region("TAPSV");//allows seperate cuts to be defined for vetos
  fNTaps=552;//including dummies
  fNRealTaps=Ntaps; //number of BaF2
  fZ0=tZ;   // Distance from centre of ball
  fSetupFile=setupfile;
  fNPbWORings= NPbWORings;

  fCOVRPhysi=new G4VPhysicalVolume*[fNTaps+1];   //Crystal physical volumes
  for(G4int i=0;i<fNTaps+1;i++) fCOVRPhysi[i]=NULL;
  fTVETPhysi=new G4VPhysicalVolume*[fNTaps+1];   //Veto physical volumes
  for(G4int i=0;i<fNTaps+1;i++) fTVETPhysi[i]=NULL;
  //fNRealTaps=510;
  fDUMMPhysi=new G4VPhysicalVolume*[fNTaps-fNRealTaps];   //Veto physical volumes
  for(G4int i=0;i<fNTaps-fNRealTaps;i++) fDUMMPhysi[i]=NULL;
  
  fTAPSSD=NULL;
  fTAPSVSD=NULL;
  fTAPSVisSD=NULL;
  fTAPSVVisSD=NULL;
}

G4VPhysicalVolume* A2DetTAPS::Construct(G4LogicalVolume* MotherLogical){
  fMotherLogic=MotherLogical;

  //Make the mother volume
  MakeForwardWallMother();
  //Make crystals
  MakeCrystals();
  //Make the veto detectors
  MakeVeto();
  //Place the crystals
  PlaceCrystals();
  return fFWMVPhysi;
}
void  A2DetTAPS::PlaceCrystals(){
  G4double z_vbox = 10.*cm;   // z size of Veto box
  G4double z_Al = 50.*cm;     // z size of Al box
  G4double az=25.*cm;            // z
  G4double t_veto = 0.5*cm;     //thickness of veto detector
  G4double  pz = -(z_Al - z_vbox - az)/2.;              //COVR(+BaF2)
  G4double  vz = -(z_Al - z_vbox + t_veto)/2. - 0.35*cm;   //Veto

  FILE* TAPSfile=fopen(fSetupFile.data(),"r");
  //FILE* TAPSfile=fopen("taps.dat","r");//workaround until get new taps files
  if(!TAPSfile){
    G4cerr<<"Couldn't open taps.dat in A2DetTAPS::PlaceCrystals()"<<G4endl;
    exit(1);
  }
  G4int index,ix,iy;
  G4double px,py;
  // G4int no=1;
  G4int NCrystals=1;
  for(G4int i=1;i<=fNTaps;i++){
    G4int scan=fscanf(TAPSfile,"%d%d%d",&index,&ix,&iy);
    if(scan<0) continue;//no more lines left in file
    px = ix * 1.5 * 6.0 / sqrt(3.)*cm;
    py = iy * 6. / 2.*cm;
    //    if(index<fNRealTaps&&fSetupFile=="taps.dat"){

    if(index<fNRealTaps){
      //    if(abs(ix)<1+fNPbWORings&&abs(iy)<2+fNPbWORings){
      if(abs(ix)+abs(iy)<=fNPbWORings*2&&abs(ix)<=fNPbWORings){
	fCOVRPhysi[i-1]=new G4PVPlacement(0,G4ThreeVector(px,py,pz),fCOVRPbLogic,"COVRPb",fFWMVLogic,false,NCrystals);
	//Note assume the veto layout is as for all BaF2 thus use i not NCrystals
	fTVETPhysi[i-1]=new G4PVPlacement(0,G4ThreeVector(px,py,vz),fTVETLogic,"TVET",fFWMVLogic,false,i);
	NCrystals+=4; //4 PbW04 for each BaF2
      }	
      else{
	fCOVRPhysi[i-1]=new G4PVPlacement(0,G4ThreeVector(px,py,pz),fCOVRLogic,"COVR",fFWMVLogic,false,NCrystals);
	fTVETPhysi[i-1]=new G4PVPlacement(0,G4ThreeVector(px,py,vz),fTVETLogic,"TVET",fFWMVLogic,false,i);
	NCrystals+=1; //4 PbW04 for each BaF2
      }
    }

  

    else if(fSetupFile=="taps.dat"&&index<=fNTaps)//dummies are not in tpas07.dat
      fDUMMPhysi[i-fNRealTaps]=new G4PVPlacement(0,G4ThreeVector(px,py,pz),fDUMMLogic,"COVR",fFWMVLogic,false,i-fNRealTaps+1);

  }
  fclose(TAPSfile);
}
void  A2DetTAPS::MakeForwardWallMother(){
  G4double z_vbox = 10.*cm;   // z size of Veto box
  G4double z_Al = 50.*cm;     // z size of Al box
  //G4double  set0 = 175.*cm;   //distance from target to front of TAPS
  G4double fwmv_phlow=0.*deg;
  G4double fwmv_phdelta=360.*deg;
  G4int fwmv_npdv=6;
  G4int fwmv_nz=2;
  const G4double fwmv_z[]={-(z_vbox + z_Al)/2.,(z_vbox + z_Al)/2.};
  const G4double fwmv_rin[]={0,0};
  const G4double fwmv_rout[]={74.5*cm,74.5*cm};
  fFWMV=new G4Polyhedra("FWMV",fwmv_phlow,fwmv_phdelta,fwmv_npdv,fwmv_nz,fwmv_z,fwmv_rin,fwmv_rout);
  fFWMVLogic=new G4LogicalVolume(fFWMV,fNistManager->FindOrBuildMaterial("G4_AIR"),"FWMV");
  G4double zpos = fZ0 + (z_Al - z_vbox)/2.;
  fFWMVPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,zpos),fFWMVLogic,"FWMV",fMotherLogic,false,1);
 
  fFWMVLogic->SetVisAttributes (G4VisAttributes::Invisible);
}
void  A2DetTAPS::MakeCrystals(){
  G4double rin=3.*cm;               //radius of inscribed circle [cm]
  G4double az=25.*cm;                              //z
  G4double phlow=0*deg;
  G4double phdelta=360*deg;
  G4int npdv=6; //Number of sides
  G4int nz=2; //Number of perp. planes along z axis
  G4double  t_covr = 0.05*cm;   // thickness of covering for BaF2
  G4double z_vbox = 10.*cm;   // z size of Veto box

  //TAPS crystals are placed in a PVC cover volume
  const G4double covr_rin[]={0,0};
  const G4double covr_rout[]={rin,rin};
  const G4double covr_z[]={-az/2-t_covr,az/2+t_covr};
  fCOVR=new G4Polyhedra("COVR",phlow,phdelta,npdv,nz,covr_z,covr_rin,covr_rout);
  fCOVRLogic=new G4LogicalVolume(fCOVR,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"COVR");
  //and for PbWO4
  fCOVRPbLogic=new G4LogicalVolume(fCOVR,fNistManager->FindOrBuildMaterial("G4_AIR"),"COVRPb");

  //TAPS BaF2 crystal
  const G4double taps_rin[]={0,0};
  const G4double taps_rout[]={rin-t_covr,rin-t_covr};
  const G4double taps_z[]={-(az-2.5*cm)/2,(az-2.5*cm)/2};
  fTAPS=new G4Polyhedra("TAPS",phlow,phdelta,npdv,nz,taps_z,taps_rin,taps_rout);
  fTAPSLogic=new G4LogicalVolume(fTAPS,fNistManager->FindOrBuildMaterial("G4_BARIUM_FLUORIDE"),"TAPS");
  //note copy number must be zero for PbWO4 crystal IDs to work
  fTAPSPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,-1.25*cm),fTAPSLogic,"TAPS",fCOVRLogic,false,0);
  G4VisAttributes* visatt=new G4VisAttributes();
  visatt->SetColor(G4Color(0.,1,0.,1));
  fTAPSLogic->SetVisAttributes(visatt);

  //TAPS PbW04 crystal
  //Polyhedra with two sides does not work as it has to be symmetric whereas our crystals have one side longer than the other!
//   phlow=-15*deg;
//   phdelta=90*deg;
//   npdv=2;
//////  fPbWO=new G4Polyhedra("TAPS",phlow,phdelta,npdv,nz,taps_z,taps_rin,taps_rout);
  G4RotationMatrix* PbRot0=new G4RotationMatrix();
  G4RotationMatrix* PbRot1=new G4RotationMatrix();
  G4RotationMatrix* PbRot2=new G4RotationMatrix();
  G4RotationMatrix* PbRot3=new G4RotationMatrix();
  //Note the subtraction solid did not work perfectly with angles 90 and 270, i.e. when gamma
  //fired at subtracted part  there was always 1 conversion. 270 270 seems to work although the 
  //crystals do not appear when use RayTracer from 0 0 degrees, fine form 180 0.
  //placing the 4 crystals inside the COVR would hopefully mask this anyway.
  PbRot0->rotateZ(180*deg);
  G4Tubs* PbCut=new G4Tubs("PbCut",0,rin*2,(az*cm),270*deg,270*deg);
  const G4double pb_z[]={-(20*cm)/2,(20*cm)/2};//PBWO crystal is 20cm long
  G4Polyhedra* PbTAPS=new G4Polyhedra("PbTAPS",phlow,phdelta,npdv,nz,pb_z,taps_rin,taps_rout);

  fPbWO=new G4SubtractionSolid("PbWO4",PbTAPS,PbCut);
  fPbWOLogic=new G4LogicalVolume(fPbWO,fNistManager->FindOrBuildMaterial("G4_PbWO4"),"PbWO");
 
  new G4PVPlacement(PbRot0,G4ThreeVector(0,0,-1.25*cm-1.25*cm),fPbWOLogic,"PbWO0",fCOVRPbLogic,false,0);
  PbRot1->rotateX(180*deg);
  new G4PVPlacement(PbRot1,G4ThreeVector(0,0,-1.25*cm-1.25*cm),fPbWOLogic,"PbWO1",fCOVRPbLogic,false,1);
  PbRot2->rotateY(180*deg);
  new G4PVPlacement(PbRot2,G4ThreeVector(0,0,-1.25*cm-1.25*cm),fPbWOLogic,"PbWO2",fCOVRPbLogic,false,2);
  PbRot3->rotateZ(0*deg);
  new G4PVPlacement(PbRot3,G4ThreeVector(0,0,-1.25*cm-1.25*cm),fPbWOLogic,"PbWO3",fCOVRPbLogic,false,3);

  G4VisAttributes* visattpb=new G4VisAttributes();
  visattpb->SetColor(G4Color(1.,0,0.,1));
  fPbWOLogic->SetVisAttributes(visattpb);


  //c======= G4_BARIUM_FLUORIDE crystal(cylider part) ==============>
  fTEND=new G4Tubs("TEND",0,2.7*cm,1.25*cm,0*deg,360*deg);
  fTENDLogic=new G4LogicalVolume(fTEND,fNistManager->FindOrBuildMaterial("G4_BARIUM_FLUORIDE"),"TEND");
  //note copy number must be zero for PbWO4 crystal IDs to work
  fTENDPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,11.25*cm),fTENDLogic,"TEND",fCOVRLogic,false,0);
  //c======= Air(for the cylider part) ==============>
  //Should probably be a polyhedra as leaves some G4_POLYVINYL_CHLORIDE volume surrounding!!
  fAIRC=new G4Tubs("AIRC",2.7*cm,5.9/2*cm,1.25*cm,0*deg,360*deg);
  fAIRCLogic=new G4LogicalVolume(fAIRC,fNistManager->FindOrBuildMaterial("G4_AIR"),"AIRC");
  fAIRCPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,11.25*cm),fAIRCLogic,"AIRC",fCOVRLogic,false,1);
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if(fIsInteractive){
    if(!fTAPSVisSD)fTAPSVisSD = new A2VisSD("TAPSVisSD",fNTaps);
    SDman->AddNewDetector( fTAPSVisSD );
    fTAPSLogic->SetSensitiveDetector(fTAPSVisSD);
    fTENDLogic->SetSensitiveDetector(fTAPSVisSD);
    fPbWOLogic->SetSensitiveDetector(fTAPSVisSD);
    fregionTAPS->AddRootLogicalVolume(fTAPSLogic);
    fregionTAPS->AddRootLogicalVolume(fTENDLogic);
    fregionTAPS->AddRootLogicalVolume(fPbWOLogic);
  }		
  else{
    if(!fTAPSSD)fTAPSSD = new A2SD("TAPSSD",fNTaps);
    SDman->AddNewDetector( fTAPSSD );		
    fTAPSLogic->SetSensitiveDetector(fTAPSSD);
    fTENDLogic->SetSensitiveDetector(fTAPSSD);
    fPbWOLogic->SetSensitiveDetector(fTAPSSD);
    fregionTAPS->AddRootLogicalVolume(fTAPSLogic);
    fregionTAPS->AddRootLogicalVolume(fTENDLogic);
    fregionTAPS->AddRootLogicalVolume(fPbWOLogic);

  }

  //c======= dummy =========
  //Same shape as COVR
  fDUMM=new G4Polyhedra("DUMM",phlow,phdelta,npdv,nz,covr_z,covr_rin,covr_rout);
  fDUMMLogic=new G4LogicalVolume(fDUMM,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"DUMM");


  //c========== Al box for TAPS =============>
  const G4double abox_z[]={-25.*cm,25.*cm};
  const G4double abox_rin[]={72.5*cm,72.5*cm};
  const G4double abox_rout[]={74.5*cm,74.5*cm};
  fABOX=new G4Polyhedra("ABOX",phlow,phdelta,npdv,nz,abox_z,abox_rin,abox_rout);
  fABOXLogic=new G4LogicalVolume(fABOX,fNistManager->FindOrBuildMaterial("G4_Al"),"ABOX");
  G4double zposbox = z_vbox/2.;
  fABOXPhysi=new G4PVPlacement(0,G4ThreeVector(0,0,zposbox),fABOXLogic,"AIRC",fFWMVLogic,false,1);

  G4VisAttributes* abox_visatt=new G4VisAttributes();
  abox_visatt->SetForceWireframe(true);
  abox_visatt->SetColor(G4Color(1,1,1));
  fABOXLogic->SetVisAttributes(G4VisAttributes::Invisible);
  fCOVRLogic->SetVisAttributes(G4VisAttributes::Invisible);
  fCOVRPbLogic->SetVisAttributes(G4VisAttributes::Invisible);
}
void  A2DetTAPS::MakeVeto(){
  G4double z_vbox = 10.*cm;   // z size of Veto box
  G4double z_Al = 50.*cm;     // z size of Al box
  //c======= Veto detector ==============>
  G4double rin=3.*cm;    //radius of inscribed circle [cm]  
  G4double tveto=0.5*cm;
  G4double phlow=0*deg;
  G4double phdelta=360*deg;
  G4int npdv=6; //Number of sides
  G4int nz=2; //Number of perp. planes along z axis
  G4double veto_z[]={-tveto/2.,tveto/2};
  G4double veto_rin[]={0,0};
  G4double veto_rout[]={rin,rin};
  fTVET=new G4Polyhedra("TVET",phlow,phdelta,npdv,nz,veto_z,veto_rin,veto_rout);
  fTVETLogic=new G4LogicalVolume(fTVET,fNistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"TVET");

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if(fIsInteractive){
    if(!fTAPSVVisSD)fTAPSVVisSD = new A2VisSD("TAPSVVisSD",fNTaps);
    SDman->AddNewDetector( fTAPSVVisSD );
    fTVETLogic->SetSensitiveDetector(fTAPSVVisSD);
    fregionTAPSV->AddRootLogicalVolume(fTVETLogic);

  }
  else{
    if(!fTAPSVSD)fTAPSVSD = new A2SD("TAPSVSD",fNTaps);
    SDman->AddNewDetector( fTAPSVSD );
    fTVETLogic->SetSensitiveDetector(fTAPSVSD);	
    fregionTAPSV->AddRootLogicalVolume(fTVETLogic);
  }
  

  G4VisAttributes* vbox_visatt=new G4VisAttributes();
  vbox_visatt->SetVisibility(false);;
  vbox_visatt->SetColor(G4Color(0,0,1));

  //c=== Veto detector box (front and back walls) =======>
  G4double vbox_z1[]={-0.1*cm,0.1*cm};//1mm wall thickness + 1mm as effective thickness of the light guide fibers
  G4double vbox_rin[]={0,0};
  G4double vbox_rout[]={72.5*cm,72.5*cm};
  fVDB1=new G4Polyhedra("VDB1",phlow,phdelta,npdv,nz,vbox_z1,vbox_rin,vbox_rout);
  fVDB1Logic=new G4LogicalVolume(fVDB1,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"VDB1");
  G4double  zpos1 = -(z_vbox + z_Al)/2. - vbox_z1[0];
  fVDB1Physi=new G4PVPlacement(0,G4ThreeVector(0,0,zpos1),fVDB1Logic,"VDB1",fFWMVLogic,false,1);

  G4double vbox_z2[]={-0.175*cm,0.175*cm};//3mm wall thickness + 0.5mm thickness of covering for G4_BARIUM_FLUORIDE
  fVDB2=new G4Polyhedra("VDB2",phlow,phdelta,npdv,nz,vbox_z2,vbox_rin,vbox_rout);
  fVDB2Logic=new G4LogicalVolume(fVDB2,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"VDB2");
  G4double  zpos2 = -(z_vbox + z_Al)/2. +z_vbox+ vbox_z1[0];
  fVDB2Physi=new G4PVPlacement(0,G4ThreeVector(0,0,zpos2),fVDB2Logic,"VDB2",fFWMVLogic,false,1);

  fVDB1Logic->SetVisAttributes(G4VisAttributes::Invisible);
  fVDB2Logic->SetVisAttributes(G4VisAttributes::Invisible);
  fTVETLogic->SetVisAttributes(vbox_visatt);
}
