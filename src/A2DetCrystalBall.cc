#include "A2DetCrystalBall.hh"

#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4SDManager.hh"
#include "TGeoMatrix.h"

#include "G4Colour.hh"
#include "G4ios.hh"

 
A2DetCrystalBall::A2DetCrystalBall()
{
  fregionCB=NULL;
  fregionCB=new G4Region("CB");//allows seperate cuts to be defined for crystal

  fVerbose=1;
  fRot=NULL;
  fTrans=NULL;
  G4cout<<"Construct the crystal ball!!!!!!"<<G4endl;
  fIsInteractive=1;
  fNcrystals=720;
  //Make array for different crystal types
  fNCrystTypes=11; //Add 1 as 0 is ignmored to keep connection with cbsim
  fCrystal=new G4Trap*[fNCrystTypes+1];   //Crystal shapes
  for(G4int i=0;i<=fNCrystTypes;i++) fCrystal[i]=NULL;
  if(fIsInteractive==0){
    fCrystLogic=new G4LogicalVolume*[fNCrystTypes+1+72];   //Crystal logical volumes,additional space (72) for cut crystals (actually there are less than this!)
    for(G4int i=0;i<=fNCrystTypes;i++) fCrystLogic[i]=NULL;
  }
  if(fIsInteractive==1){
    fCrystLogic=new G4LogicalVolume*[fNcrystals];   //Crystal logical volumes
    for(G4int i=0;i<fNcrystals;i++) fCrystLogic[i]=NULL;
  }
  fCrystPhysi=new G4VPhysicalVolume*[fNcrystals];   //Crystal physical volumes
  for(G4int i=0;i<fNcrystals;i++) fCrystPhysi[i]=NULL;

  fGap=G4ThreeVector(0.4*CLHEP::cm,0.4*CLHEP::cm,0); //default gap between hemispheres

  fCNIN=NULL;
  fCCUT=NULL;
  fTUNL=NULL;
  fBTMM=NULL;
  fRIMS=NULL;
  fCNINLogic=fCCUTLogic=fTUNLLogic=fBTMMLogic=fRIMSLogic=NULL;
  fCNINPhysi[0]=fCNINPhysi[1]=fCCUTPhysi=NULL;
  fTUNLPhysi[0]=fTUNLPhysi[1]=fTUNLPhysi[2]=fTUNLPhysi[3]=NULL;
  fBTMMPhysi[0]=fBTMMPhysi[1]=fBTMMPhysi[2]=fBTMMPhysi[3]=NULL;
  fRIMSPhysi[0]=fRIMSPhysi[1]=NULL;


  for(G4int i=0;i<3;i++){
    fSKI[i]=NULL;
    fSKILogic[i]=NULL;
    fSKIPhysi[i*3]=fSKIPhysi[i*3+1]=fSKIPhysi[i*3+2]=fSKIPhysi[i*3+3]=NULL;
  }
  fCBSD=NULL;
  fVisCBSD=NULL;
  fCrystVisAtt=NULL;

  fCrystalConvert=NULL;
  fCrystalConvert=new G4int[fNcrystals];
  //Read in the AcquRoot id map
  FILE* CCfile=fopen("CrystalConvert.in","r");
  if(!CCfile){
    G4cerr<<"Couldn't open CrystalConvert.in in A2SD::A2SD(G4String name)"<<G4endl;
    exit(1);
  }
  G4int cc=0;
  for(G4int i=0;i<20;i++){//major loop
    char major[100];
    fscanf(CCfile,"%*s%*s%s",major);//take out Major string
    int id=0;
    for(G4int j=0;j<4;j++) //minor loop
      for(G4int k=0;k<9;k++){//crystal loop	
	fscanf(CCfile,"%d",&id);
	fCrystalConvert[cc++]=id;
      }
  }
  if(fCrystalConvert[719]!=621) {G4cerr<<"A2SD::A2SD Might have a problem with crystal convert array!!!"<<G4endl;exit(0);}
  fclose(CCfile);
}
A2DetCrystalBall::~A2DetCrystalBall()
{
  G4cout<<"Deleting CrystalBall"<<G4endl;
 
}
G4VPhysicalVolume* A2DetCrystalBall::Construct(G4LogicalVolume* MotherLogical)
{

  fMotherLogic=MotherLogical;
 //MAke the rotational Matrices and translational vectors stolen from 
  //the G3 sim via ROOT
  MakeRotationMatrices();
  MakeTransformVectors();
 
  //Make the differnet crystal solids and logical volumes
  MakeCrystals();

  //Make the other non-sensitve materials
  if(fGap.z()<1) MakeOther1();
  else if(fGap.z()>1) MakeOther2();
  else {G4cerr<<"Need to specify which other material for the ball."<<G4endl<<"Add line :"<<G4endl<<"/A2/det/setHemiGap 0.4 0.4 1 cm"<<G4endl<<"to DetectorSetup.mac, see manual for details"<<G4endl;exit(0);}
  //Place the ball in the lab 
   MakeBall();

  return fMyPhysi;
}

void A2DetCrystalBall::MakeOther1(){//please read note above
  //define an inch!
  const G4double inch=1.0/0.393700787*CLHEP::cm;
  G4VisAttributes* CBVisAtt= new G4VisAttributes(G4Colour(1,.0,.0));
  CBVisAtt->SetVisibility(true);



  //  The cylindrical cut in the Crystal Ball. This is 10 cm in radius -
  //  actually, make that 10.3 cm plus 1/16". The 10.3 cm are from A.
  //  Efendiev's measurement of the tunnel diameter on March 6, 96; the
  //  1/16" is the steel thickness specified in the SLAC CB reports. Make
  //  this volume long enough to contain anything that fits in diameter -
  //  i.e., as long as the lab box
  //  Starostin changed thickness to 3/16" prior to G4
  // Changed back on 17/3/11

  //Actually not needed as it is only used to hold the exit tunnel
  G4double tunl_rin=10.3*CLHEP::cm;
  G4double tunl_rout = tunl_rin + 2.5/16*inch;

  G4double ccut_rin=0;
  //  G4double ccut_rout=10.3*CLHEP::cm+1.0/16*inch;
  G4double ccut_rout=tunl_rout+1*CLHEP::mm; //cut at the tunnel outer radius + small distance to avoid overlap
  // G4double ccut_rout=8.95*CLHEP::cm;
  G4double ccut_z=44*CLHEP::cm; //half lenght in z
  G4double ccut_phlow=0;
  G4double ccut_phdelta=360*CLHEP::deg;
  fCCUT=new G4Tubs("CCUT",ccut_rin,ccut_rout,ccut_z,ccut_phlow,ccut_phdelta);
  fCCUTLogic=new G4LogicalVolume(fCCUT,fNistManager->FindOrBuildMaterial("G4_AIR"),"CCUT");
  fCCUTLogic->SetVisAttributes(CBVisAtt);
  //  The actual beam entrance and exit tunnels - a pair of cylindrical
  //  steel tubes, 10.3 cm in inner diameter, 1/16" thick, and 11.5 cm
  //  wide, one around each beam opening.
  // S.P. version of TUNL
  G4double tunl_z = 11*CLHEP::cm/2; //half length in z
  G4double tunl_phlow=0*CLHEP::deg;
  G4double tunl_phdelta=180*CLHEP::deg;
  G4double tunl_z0=22.6*CLHEP::cm;
  fTUNL=new G4Tubs("TUNL",tunl_rin,tunl_rout,tunl_z,tunl_phlow,tunl_phdelta);
  fTUNLLogic=new G4LogicalVolume(fTUNL,fNistManager->FindOrBuildMaterial("G4_Fe"),"TUNL");
  fTUNLLogic->SetVisAttributes(CBVisAtt);
  G4double Tshift=28.1*CLHEP::cm;
  fTUNLPhysi[0]=new G4PVPlacement(0,G4ThreeVector(0,fGap.x(),Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,1);
  fTUNLPhysi[1]=new G4PVPlacement(0,G4ThreeVector(0,fGap.x(),-Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,2);
  fTUNLPhysi[2]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-fGap.y(),Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,3);
  fTUNLPhysi[3]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-fGap.y(),-Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,4);
  //A.S version of CNIN
  //  The inner 'can' around the Crystal Ball. This is a steel sphere ~20
  //  cm in radius, 1/16" thick, with beam openings at both ends covering a
  //  half angle of about 30 degrees each. The actual numbers used are from
  //  measurements made by A. Efendiev on the lower hemisphere on March 6,
  //  1996.
  //  Starostin changed thickness to 3/16" prior to G4,changed back Prakhov08/07
  G4double cnin_rin=9.8*inch;
  G4double cnin_rout=cnin_rin+1.0/16*inch;
  G4double cnin_thlow=asin(tunl_rout/cnin_rin)*CLHEP::rad;
  G4double cnin_thdelta=180*CLHEP::deg-2*cnin_thlow;
  G4double cnin_phlow=0*CLHEP::deg;
  G4double cnin_phdelta=180*CLHEP::deg;
  //Note G4Sphere sets phi limits first then theta, ROOT+G3 are vice versa
  fCNIN=new G4Sphere("CNIN",cnin_rin,cnin_rout,cnin_phlow,cnin_phdelta,cnin_thlow,cnin_thdelta);
  fCNINLogic=new G4LogicalVolume(fCNIN,fNistManager->FindOrBuildMaterial("G4_Fe"),"CNIN");
  fCNINLogic->SetVisAttributes(CBVisAtt);

  //Place upper hemisphere first
  G4Transform3D trans=G4Transform3D(fRot[26]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector(0,fGap.x(),0))*trans;//place in lab
  fCNINPhysi[0]=new G4PVPlacement(trans,fCNINLogic,fCNINLogic->GetName(),fMotherLogic,false,1);
  //Now lower
  trans=G4Transform3D(fRot[95]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector(0,-fGap.y(),0))*trans;//place in lab
  fCNINPhysi[1]=new G4PVPlacement(trans,fCNINLogic,fCNINLogic->GetName(),fMotherLogic,false,2);

  //test outer can
  G4double cnout_rin=67*CLHEP::cm;
  G4double cnout_rout=cnout_rin+0.5*CLHEP::cm;
  G4double cnout_thlow=asin(tunl_rout/cnin_rin)*CLHEP::rad;
  G4double cnout_thdelta=180*CLHEP::deg-2*cnin_thlow;
  G4double cnout_phlow=0*CLHEP::deg;
  G4double cnout_phdelta=180*CLHEP::deg;
  //Note G4Sphere sets phi limits first then theta, ROOT+G3 are vice versa
  G4Sphere* CNOUT=new G4Sphere("CNOUT",cnout_rin,cnout_rout,cnout_phlow,cnout_phdelta,cnout_thlow,cnout_thdelta);
  G4LogicalVolume* CNOUTLogic=new G4LogicalVolume(CNOUT,fNistManager->FindOrBuildMaterial("G4_Fe"),"CNOUT");
  CNOUTLogic->SetVisAttributes(CBVisAtt);

  //Place upper hemisphere first
  trans=G4Transform3D(fRot[26]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector())*trans;//place in lab
  //  new G4PVPlacement(trans,CNOUTLogic,CNOUTLogic->GetName(),fMotherLogic,false,1);///not placed
  //Now lower
  trans=G4Transform3D(fRot[95]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector())*trans;//place in lab
  // new G4PVPlacement(trans,CNOUTLogic,CNOUTLogic->GetName(),fMotherLogic,false,2);///not placed

  //  The "bottom" of the hemispheres. According CB`s draw the thickness of the
  //  plane from stainless steel is 0.030" (0.076 cm). This is a segment of a disk
  //  with the inner radius equal inner radius of the inner `can` around the CB and
  //  with out radius equal outer radius of the outer shell of the CB.

  //June 2012 thcikness changed to 0.063inch, note this also needs changed in the crystal postioning...
  //  G4double btmthnss= 0.076*CLHEP::cm;
  G4double btmthnss= 0.063*inch;
  // G4double gap1  = 0.05*CLHEP::cm + btmthnss;
  // G4double gap2  = 0.05*CLHEP::cm + btmthnss;
  G4double gap1  = fGap.x() + btmthnss;
  G4double gap2  = fGap.y() + btmthnss;
  G4double btm_rin = sqrt(34.7*34.7 + 20.6*20.6)/2*CLHEP::cm;    // inner radius
  G4double cnou_radius = (26. + 11./16.) * inch;  // inner radius of Al sphere
  G4double btm_rout = cnou_radius + 7./8. * inch;  // outer radius
  G4double btm_phlow=21.*CLHEP::deg;
  G4double btm_phdelta=(180-2*21.)*CLHEP::deg;
  fBTMM=new G4Tubs("BTMM",btm_rin,btm_rout,btmthnss/2,btm_phlow,btm_phdelta);
  fBTMMLogic=new G4LogicalVolume(fBTMM,fNistManager->FindOrBuildMaterial("G4_Fe"),"BTMM");
  fBTMMLogic->SetVisAttributes(CBVisAtt);
  fBTMMPhysi[0]=new G4PVPlacement(fRot[25],G4ThreeVector(0,gap1-btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,1);
  fBTMMPhysi[1]=new G4PVPlacement(fRot[25],G4ThreeVector(0,-gap2+btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,2);
  fBTMMPhysi[2]=new G4PVPlacement(fRot[98],G4ThreeVector(0,gap1-btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,3);
  fBTMMPhysi[3]=new G4PVPlacement(fRot[98],G4ThreeVector(0,-gap2+btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,4);
  //The skirting as designed by Viktor Kashevarov
  //Numbers copied straight from crystalball.C file
  //note arguemnts for ROOT trd1 are dx1, dx2, dy and dz
  //for g4 they are dx1,dx2 dy1=dy,dy2=dy,dz, dy=half thickness =inch/32
  //  G4double tunl_zpos=tunl_z0+tunl_z/2;//! outer end of cylindrical tunnels region
  G4double skrl=63./2*CLHEP::cm;//!half-length of skirt's trapezoids
  G4double skrt =3.5*inch/32;//!half-thickness of skirt's steel (1/16") 

  fSKI[0]=new G4Trd("SKI1",8*CLHEP::cm,21.8*CLHEP::cm,skrt,skrt,skrl);
  fSKI[1]=new G4Trd("SKI2",7.*CLHEP::cm,18.65*CLHEP::cm,skrt,skrt,skrl);
  fSKI[2]=new G4Trd("SKI3",7.*CLHEP::cm,18.65*CLHEP::cm,skrt,skrt,skrl);

  fSKILogic[0]=new G4LogicalVolume(fSKI[0],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI1");
  fSKILogic[1]=new G4LogicalVolume(fSKI[1],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI2");
  fSKILogic[2]=new G4LogicalVolume(fSKI[2],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI3");
  //CBVisAtt->SetColor(G4Color(1.0,.0,0));
  fSKILogic[0]->SetVisAttributes(CBVisAtt);
  fSKILogic[1]->SetVisAttributes(CBVisAtt);
  fSKILogic[2]->SetVisAttributes(CBVisAtt);

  fSKIPhysi[0]=new G4PVPlacement(fRot[51],fTrans[51]+G4ThreeVector(0,fGap.x(),0),fSKILogic[0],"SKIRT",fMotherLogic,false,1);
  fSKIPhysi[1]=new G4PVPlacement(fRot[52],fTrans[52]-G4ThreeVector(0,fGap.y(),0),fSKILogic[0],"SKIRT",fMotherLogic,false,2);
  fSKIPhysi[2]=new G4PVPlacement(fRot[53],fTrans[53]+G4ThreeVector(0,fGap.x(),0),fSKILogic[0],"SKIRT",fMotherLogic,false,3);
  fSKIPhysi[3]=new G4PVPlacement(fRot[54],fTrans[54]-G4ThreeVector(0,fGap.y(),0),fSKILogic[0],"SKIRT",fMotherLogic,false,4);

  fSKIPhysi[4]=new G4PVPlacement(fRot[61],fTrans[61]+G4ThreeVector(0,fGap.x(),0),fSKILogic[1],"SKIRT",fMotherLogic,false,1);
  fSKIPhysi[5]=new G4PVPlacement(fRot[62],fTrans[62]-G4ThreeVector(0,fGap.y(),0),fSKILogic[1],"SKIRT",fMotherLogic,false,2);
  fSKIPhysi[6]=new G4PVPlacement(fRot[63],fTrans[63]+G4ThreeVector(0,fGap.x(),0),fSKILogic[1],"SKIRT",fMotherLogic,false,3);
  fSKIPhysi[7]=new G4PVPlacement(fRot[64],fTrans[64]-G4ThreeVector(0,fGap.y(),0),fSKILogic[1],"SKIRT",fMotherLogic,false,4);

  fSKIPhysi[8]=new G4PVPlacement(fRot[71],fTrans[71]+G4ThreeVector(0,fGap.x(),0),fSKILogic[2],"SKIRT",fMotherLogic,false,1);
  fSKIPhysi[9]=new G4PVPlacement(fRot[72],fTrans[72]-G4ThreeVector(0,fGap.y(),0),fSKILogic[2],"SKIRT",fMotherLogic,false,2);
  fSKIPhysi[10]=new G4PVPlacement(fRot[73],fTrans[73]+G4ThreeVector(0,fGap.x(),0),fSKILogic[2],"SKIRT",fMotherLogic,false,3);
  fSKIPhysi[11]=new G4PVPlacement(fRot[74],fTrans[74]-G4ThreeVector(0,fGap.y(),0),fSKILogic[2],"SKIRT",fMotherLogic,false,4);

  //  Rims right after the beam tunnel and inside the CB tunnel. Simulated by placing a 4 mm
  //  steel ring after the Ball. The ring is shadowing 19.93 degree cone. Updated now a 17.7degree cone.15/11/07

  G4double rims_rin = tunl_rout/(tunl_z0+tunl_z*2)*101*CLHEP::cm;
  G4double rims_rout = 65*CLHEP::cm;
  G4double rims_z  = 3.5*inch/32;       // half length
  G4double rims_phlow=0*CLHEP::deg;
  G4double rims_phdelta = 180*CLHEP::deg;
  G4double rims_zpos=1.01*CLHEP::m;
  fRIMS=new G4Tubs("RIMS",rims_rin,rims_rout,rims_z,rims_phlow,rims_phdelta);
  fRIMSLogic=new G4LogicalVolume(fRIMS,fNistManager->FindOrBuildMaterial("G4_Fe"),"RIMS");
  fRIMSLogic->SetVisAttributes(G4VisAttributes::Invisible);
  fRIMSPhysi[0]=new G4PVPlacement(0,G4ThreeVector(0,gap1,rims_zpos),fRIMSLogic,"RIMS",fMotherLogic,false,1);
  fRIMSPhysi[1]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-gap2,rims_zpos),fRIMSLogic,"RIMS",fMotherLogic,false,2);


}

//******************************************************************//
void A2DetCrystalBall::MakeOther2(){//Based on ugeom_mamic_apr09.F
  //define an inch!
  const G4double inch=1.0/0.393700787*CLHEP::cm;
  G4VisAttributes* CBVisAtt= new G4VisAttributes(G4Colour(1,.0,.0));
  CBVisAtt->SetVisibility(true);



  //  The cylindrical cut in the Crystal Ball. This is 10 cm in radius -
  //  actually, make that 10.3 cm plus 1/16". The 10.3 cm are from A.
  //  Efendiev's measurement of the tunnel diameter on March 6, 96; the
  //  1/16" is the steel thickness specified in the SLAC CB reports. Make
  //  this volume long enough to contain anything that fits in diameter -
  //  i.e., as long as the lab box

  //Actually not needed as it is only used to hold the exit tunnel
  G4double radincb = 8.7*inch;
  G4double thcb = 24.*CLHEP::deg;
  G4double radtun = sin(thcb)*radincb;// (~8.95 cm)
  G4double ccut_rin=0;
  G4double ccut_rout=radtun;
  G4double ccut_z=44*CLHEP::cm; //half lenght in z
  G4double ccut_phlow=0;
  G4double ccut_phdelta=360*CLHEP::deg;
  fCCUT=new G4Tubs("CCUT",ccut_rin,ccut_rout,ccut_z,ccut_phlow,ccut_phdelta);
  fCCUTLogic=new G4LogicalVolume(fCCUT,fNistManager->FindOrBuildMaterial("G4_AIR"),"CCUT");
  fCCUTLogic->SetVisAttributes(CBVisAtt);
   //  The actual beam entrance and exit tunnels - a pair of cylindrical
  //  steel tubes, 10.3 cm in inner diameter, 1/16" thick, and 11.5 cm
  //  wide, one around each beam opening.
  //G4double tunl_rin=10.3*CLHEP::cm;
  G4double tunl_rout = radtun;
  G4double tunl_zin =1.25*CLHEP::cm;
  G4double tunl_len=6.8*CLHEP::cm+tunl_zin;
  //G4double tunl_z = 11*CLHEP::cm/2; //half length in z
  G4double tunl_phlow=0*CLHEP::deg;
  G4double tunl_phdelta=180*CLHEP::deg;
  G4double tunl_z0=cos(thcb)*radincb - tunl_zin;
  G4double tunl_rin = tunl_rout - 2.75*inch/16.;
  fTUNL=new G4Tubs("TUNL",tunl_rin,tunl_rout,tunl_len/2,tunl_phlow,tunl_phdelta);
  fTUNLLogic=new G4LogicalVolume(fTUNL,fNistManager->FindOrBuildMaterial("G4_Fe"),"TUNL");
  fTUNLLogic->SetVisAttributes(CBVisAtt);
  G4double Tshift=tunl_z0+tunl_len/2;
  fTUNLPhysi[0]=new G4PVPlacement(0,G4ThreeVector(0,fGap.x(),Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,1);
  fTUNLPhysi[1]=new G4PVPlacement(0,G4ThreeVector(0,fGap.x(),-Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,2);
  fTUNLPhysi[2]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-fGap.y(),Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,3);
  fTUNLPhysi[3]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-fGap.y(),-Tshift),fTUNLLogic,"TUNL",fMotherLogic,false,4);
  //A.S version of CNIN
  //  The inner 'can' around the Crystal Ball. This is a steel sphere ~20
  //  cm in radius, 1/16" thick, with beam openings at both ends covering a
  //  half angle of about 30 degrees each. The actual numbers used are from
  //  measurements made by A. Efendiev on the lower hemisphere on March 6,
  //  1996.
  //  Starostin changed thickness to 3/16" prior to G4,changed back Prakhov08/07
  G4double cnin_rin=9.8*inch;
  G4double cnin_rout=cnin_rin+1.0/16*inch;
  G4double cnin_thlow=asin(tunl_rout/cnin_rin)*CLHEP::rad;
  G4double cnin_thdelta=180*CLHEP::deg-2*cnin_thlow;
  G4double cnin_phlow=0*CLHEP::deg;
  G4double cnin_phdelta=180*CLHEP::deg;
  //Note G4Sphere sets phi limits first then theta, ROOT+G3 are vice versa
  fCNIN=new G4Sphere("CNIN",cnin_rin,cnin_rout,cnin_phlow,cnin_phdelta,cnin_thlow,cnin_thdelta);
  fCNINLogic=new G4LogicalVolume(fCNIN,fNistManager->FindOrBuildMaterial("G4_Fe"),"CNIN");
  fCNINLogic->SetVisAttributes(CBVisAtt);

  //Place upper hemisphere first
  G4Transform3D trans=G4Transform3D(fRot[26]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector(0,fGap.x(),0))*trans;//place in lab
  fCNINPhysi[0]=new G4PVPlacement(trans,fCNINLogic,fCNINLogic->GetName(),fMotherLogic,false,1);
  //Now lower
  trans=G4Transform3D(fRot[95]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector(0,-fGap.y(),0))*trans;//place in lab
  fCNINPhysi[1]=new G4PVPlacement(trans,fCNINLogic,fCNINLogic->GetName(),fMotherLogic,false,2);

  //test outer can
  G4double cnout_rin=67*CLHEP::cm;
  G4double cnout_rout=cnout_rin+0.5*CLHEP::cm;
  G4double cnout_thlow=asin(tunl_rout/cnin_rin)*CLHEP::rad;
  G4double cnout_thdelta=180*CLHEP::deg-2*cnin_thlow;
  G4double cnout_phlow=0*CLHEP::deg;
  G4double cnout_phdelta=180*CLHEP::deg;
  //Note G4Sphere sets phi limits first then theta, ROOT+G3 are vice versa
  G4Sphere* CNOUT=new G4Sphere("CNOUT",cnout_rin,cnout_rout,cnout_phlow,cnout_phdelta,cnout_thlow,cnout_thdelta);
  G4LogicalVolume* CNOUTLogic=new G4LogicalVolume(CNOUT,fNistManager->FindOrBuildMaterial("G4_Fe"),"CNOUT");
  CNOUTLogic->SetVisAttributes(CBVisAtt);

  //Place upper hemisphere first
  trans=G4Transform3D(fRot[26]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector())*trans;//place in lab
  //  new G4PVPlacement(trans,CNOUTLogic,CNOUTLogic->GetName(),fMotherLogic,false,1);///not placed
  //Now lower
  trans=G4Transform3D(fRot[95]->inverse(),G4ThreeVector());//place in hemisphere
  trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector())*trans;//place in lab
  // new G4PVPlacement(trans,CNOUTLogic,CNOUTLogic->GetName(),fMotherLogic,false,2);///not placed

  //  The "bottom" of the hemispheres. According CB`s draw the thickness of the
  //  plane from stainless steel is 0.030" (0.076 cm). This is a segment of a disk
  //  with the inner radius equal inner radius of the inner `can` around the CB and
  //  with out radius equal outer radius of the outer shell of the CB.

 
  G4double btmthnss= 0.076*CLHEP::cm;
  G4double gap1  = fGap.x() + btmthnss;
  G4double gap2  = fGap.y() + btmthnss;
  G4double btm_rin = radincb;    // inner radius, changed from 20-22cm
  G4double cnou_radius = (26. + 11./16.) * inch;  // inner radius of Al sphere
  G4double btm_rout = cnou_radius + 7./8. * inch;  // outer radius
  G4double btm_phlow=thcb;
  G4double btm_phdelta=(180*CLHEP::deg-2*thcb);
  fBTMM=new G4Tubs("BTMM",btm_rin,btm_rout,btmthnss/2,btm_phlow,btm_phdelta);
  fBTMMLogic=new G4LogicalVolume(fBTMM,fNistManager->FindOrBuildMaterial("G4_Fe"),"BTMM");
  fBTMMLogic->SetVisAttributes(CBVisAtt);
  fBTMMPhysi[0]=new G4PVPlacement(fRot[25],G4ThreeVector(0,gap1-btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,1);
  fBTMMPhysi[1]=new G4PVPlacement(fRot[25],G4ThreeVector(0,-gap2+btmthnss,0),fBTMMLogic,"BTMM",fMotherLogic,false,2);
  fBTMMPhysi[2]=new G4PVPlacement(fRot[98],G4ThreeVector(0,gap1-btmthnss/2,0),fBTMMLogic,"BTMM",fMotherLogic,false,3);
  fBTMMPhysi[3]=new G4PVPlacement(fRot[98],G4ThreeVector(0,-gap2+btmthnss,0),fBTMMLogic,"BTMM",fMotherLogic,false,4);
  //The skirting as designed by Viktor Kashevarov
  //Numbers copied straight from crystalball.C file
  //note arguemnts for ROOT trd1 are dx1, dx2, dy and dz
  //for g4 they are dx1,dx2 dy1=dy,dy2=dy,dz, dy=half thickness =inch/32
  //  G4double tunl_zpos=tunl_z0+tunl_z/2;//! outer end of cylindrical tunnels region
  G4double tunl_zpos=cos(thcb)*25.*CLHEP::cm+10*CLHEP::cm;//! outer end of cylindrical tunnels region
  //Ammended now to prakovs version
  G4double skrl=63./2*CLHEP::cm;//!half-length of skirt's trapezoids
  G4double skrt =2.5*inch/32;//!half-thickness of skirt's steel  

  fSKI[0]=new G4Trd("SKI1",8*CLHEP::cm,21.8*CLHEP::cm,skrt,skrt,skrl);
  fSKI[1]=new G4Trd("SKI2",7.*CLHEP::cm,18.65*CLHEP::cm,skrt,skrt,skrl);
  fSKI[2]=new G4Trd("SKI3",7.*CLHEP::cm,18.65*CLHEP::cm,skrt,skrt,skrl);

  fSKILogic[0]=new G4LogicalVolume(fSKI[0],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI1");
  fSKILogic[1]=new G4LogicalVolume(fSKI[1],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI2");
  fSKILogic[2]=new G4LogicalVolume(fSKI[2],fNistManager->FindOrBuildMaterial("G4_Fe"),"SKI3");
  //CBVisAtt->SetColor(G4Color(1.0,.0,0));
  fSKILogic[0]->SetVisAttributes(CBVisAtt);
  fSKILogic[1]->SetVisAttributes(CBVisAtt);
  fSKILogic[2]->SetVisAttributes(CBVisAtt);

  //Calculate rotation matrix via TGeoRotation which has geant3 style implementation!
  TGeoRotation* tempRot = new TGeoRotation("tempRot",90,90,0,0,90,0);
  double th3 = 20.9*CLHEP::deg;
  double ph3 = 148.1*CLHEP::deg;
  double alf = -0.12*CLHEP::deg;
  double ph1=ph3 - 90.*CLHEP::deg - alf;
  double th1=atan(-1./(tan(th3)*cos(ph1-ph3)))+180.*CLHEP::deg;
  double ph2=atan((tan(th3)*cos(ph3)-tan(th1)*cos(ph1))/(tan(th1)*sin(ph1)-tan(th3)*sin(ph3)))+180.*CLHEP::deg;
  double th2=atan(-1./(tan(th3)*cos(ph2-ph3)))+180*CLHEP::deg;

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,th2/CLHEP::deg,ph2/CLHEP::deg,th3/CLHEP::deg,ph3/CLHEP::deg);
  const Double_t *m = tempRot->GetRotationMatrix();
  fRot[51] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg+180,th2/CLHEP::deg,ph2/CLHEP::deg+180,th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[52] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg+180,180-th2/CLHEP::deg,ph2/CLHEP::deg,180-th3/CLHEP::deg,ph3/CLHEP::deg);
  m = tempRot->GetRotationMatrix();
  fRot[53] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,180-th2/CLHEP::deg,ph2/CLHEP::deg+180,180-th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[54] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  //Now translations
  G4double z=tunl_zpos + skrl*cos(th3);
  G4double  zz = (z - skrt/sin(th3))*tan(th3);
  G4double   x = zz*cos(ph3);
  G4double   y = zz*sin(ph3);
    
  fSKIPhysi[0]=new G4PVPlacement(fRot[51],G4ThreeVector(x,y+fGap.x(),z),fSKILogic[0],"SKIRT",fMotherLogic,false,1);

  fSKIPhysi[1]=new G4PVPlacement(fRot[52],G4ThreeVector(-x,-y-fGap.y(),z),fSKILogic[0],"SKIRT",fMotherLogic,false,2);
  fSKIPhysi[2]=new G4PVPlacement(fRot[53],G4ThreeVector(x,y+fGap.x(),-z),fSKILogic[0],"SKIRT",fMotherLogic,false,3);
  fSKIPhysi[3]=new G4PVPlacement(fRot[54],G4ThreeVector(-x,-y-fGap.y(),-z),fSKILogic[0],"SKIRT",fMotherLogic,false,4);

  //Calculate rotation matrix via TGeoRotation which has geant3 style implementation!
  //SK2
  th3 = 19.8*CLHEP::deg;
  ph3 = 89.2*CLHEP::deg;
  alf = 7.9*CLHEP::deg;
  ph1=ph3 - 90.*CLHEP::deg - alf+360*CLHEP::deg;
  th1=atan(-1./(tan(th3)*cos(ph1-ph3)));
  ph2=atan((tan(th3)*cos(ph3)-tan(th1)*cos(ph1))/(tan(th1)*sin(ph1)-tan(th3)*sin(ph3)));
  th2=atan(-1./(tan(th3)*cos(ph2-ph3)))+180*CLHEP::deg;
  
  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,th2/CLHEP::deg,ph2/CLHEP::deg,th3/CLHEP::deg,ph3/CLHEP::deg);
  m = tempRot->GetRotationMatrix();
  fRot[61] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg-180,th2/CLHEP::deg,ph2/CLHEP::deg+180,th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[62] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg-180,180-th2/CLHEP::deg,ph2/CLHEP::deg,180-th3/CLHEP::deg,ph3/CLHEP::deg);
  m = tempRot->GetRotationMatrix();
  fRot[63] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,180-th2/CLHEP::deg,ph2/CLHEP::deg+180,180-th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[64] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  //Now translations
  z = tunl_zpos + skrl*cos(th3);
  zz = (z - skrt/sin(th3))*tan(th3);
  x = zz*cos(ph3);
  y = zz*sin(ph3);
    
  fSKIPhysi[4]=new G4PVPlacement(fRot[61],G4ThreeVector(x,y+fGap.x(),z),fSKILogic[1],"SKIRT",fMotherLogic,false,5);

  fSKIPhysi[5]=new G4PVPlacement(fRot[62],G4ThreeVector(-x,-y-fGap.y(),z),fSKILogic[1],"SKIRT",fMotherLogic,false,6);
  fSKIPhysi[6]=new G4PVPlacement(fRot[63],G4ThreeVector(x,y+fGap.x(),-z),fSKILogic[1],"SKIRT",fMotherLogic,false,7);
  fSKIPhysi[7]=new G4PVPlacement(fRot[64],G4ThreeVector(-x,-y-fGap.y(),-z),fSKILogic[1],"SKIRT",fMotherLogic,false,8);
  //SK3
  th3 = 19.8*CLHEP::deg;
  ph3 = 27.2*CLHEP::deg;
  alf = -8.1*CLHEP::deg;
  ph1=ph3 - 90.*CLHEP::deg - alf+360*CLHEP::deg;
  th1=atan(-1./(tan(th3)*cos(ph1-ph3)))+180*CLHEP::deg;
  ph2=atan((tan(th3)*cos(ph3)-tan(th1)*cos(ph1))/(tan(th1)*sin(ph1)-tan(th3)*sin(ph3)));
  th2=atan(-1./(tan(th3)*cos(ph2-ph3)))+180*CLHEP::deg;
  
  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,th2/CLHEP::deg,ph2/CLHEP::deg,th3/CLHEP::deg,ph3/CLHEP::deg);
  m = tempRot->GetRotationMatrix();
  fRot[71] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg-180,th2/CLHEP::deg,ph2/CLHEP::deg+180,th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[72] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg-180,180-th2/CLHEP::deg,ph2/CLHEP::deg,180-th3/CLHEP::deg,ph3/CLHEP::deg);
  m = tempRot->GetRotationMatrix();
  fRot[73] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  tempRot->SetAngles(th1/CLHEP::deg,ph1/CLHEP::deg,180-th2/CLHEP::deg,ph2/CLHEP::deg+180,180-th3/CLHEP::deg,ph3/CLHEP::deg+180);
  m = tempRot->GetRotationMatrix();
  fRot[74] = new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));

  //Now translations
  z = tunl_zpos + skrl*cos(th3);
  zz = (z - skrt/sin(th3))*tan(th3);
  x = zz*cos(ph3);
  y = zz*sin(ph3);
    
  fSKIPhysi[8]=new G4PVPlacement(fRot[71],G4ThreeVector(x,y+fGap.x(),z),fSKILogic[2],"SKIRT",fMotherLogic,false,9);

  fSKIPhysi[9]=new G4PVPlacement(fRot[72],G4ThreeVector(-x,-y-fGap.y(),z),fSKILogic[2],"SKIRT",fMotherLogic,false,10);
  fSKIPhysi[10]=new G4PVPlacement(fRot[73],G4ThreeVector(x,y+fGap.x(),-z),fSKILogic[2],"SKIRT",fMotherLogic,false,11);
  fSKIPhysi[11]=new G4PVPlacement(fRot[74],G4ThreeVector(-x,-y-fGap.y(),-z),fSKILogic[2],"SKIRT",fMotherLogic,false,12);


  //  Rims right after the beam tunnel and inside the CB tunnel. Simulated by placing a 4 mm
  //  steel ring after the Ball. The ring is shadowing 19.93 CLHEP::degree cone. Updated now a 17.7CLHEP::degree cone.15/11/07
  //DG move inside exit tunnel and reduce outer angle to 21CLHEP::degrees
  G4double zrim=100*CLHEP::cm;
  G4double rims_rin = tunl_rin/(tunl_z0+tunl_len)*zrim;
  G4double rims_rout = tan(30*CLHEP::deg)*zrim;
  G4double rims_z  = 2.5*inch/32;       // half length
  G4double rims_phlow=0*CLHEP::deg;
  G4double rims_phdelta = 180*CLHEP::deg;
  G4double rims_zpos=zrim;
  fRIMS=new G4Tubs("RIMS",rims_rin,rims_rout,rims_z,rims_phlow,rims_phdelta);
  fRIMSLogic=new G4LogicalVolume(fRIMS,fNistManager->FindOrBuildMaterial("G4_Fe"),"RIMS");
  fRIMSLogic->SetVisAttributes(CBVisAtt);
  fRIMSPhysi[0]=new G4PVPlacement(0,G4ThreeVector(0,gap1,rims_zpos),fRIMSLogic,"RIMS",fMotherLogic,false,1);
  fRIMSPhysi[1]=new G4PVPlacement(fRot[96],G4ThreeVector(0,-gap2,rims_zpos),fRIMSLogic,"RIMS",fMotherLogic,false,2);


}
void A2DetCrystalBall::MakeBall(){
  G4VisAttributes* CBVisAtt= new G4VisAttributes(G4Colour(.6,0.8,0.8));
  CBVisAtt->SetVisibility(true);

  //Create ball detector logic volume
  fMyLogic=new G4LogicalVolume(new G4Box("ball",5*CLHEP::m,5*CLHEP::m,5*CLHEP::m),fNistManager->FindOrBuildMaterial("G4_AIR"),"ball");
  // G4VisAttributes* CBVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  fMyLogic->SetVisAttributes(CBVisAtt);
   CBVisAtt->SetVisibility(true);
   if(fCrystVisAtt==NULL) fCrystVisAtt= new G4VisAttributes(G4Colour(1,0.9,0.9));

  const G4double inch=1.0/0.393700787*CLHEP::cm;
 
  //In GEANT3 the ball was constructed in two hemispheres CBLU and CBLD
  //However these hemispheres were actually spheres and therefore gave
  //an overlap when placed in the ball. GEANT4 does not like overlaps!!
  //Solution just place each crystal individually

  //Each hemisphere has 15 Major triangles giving a total of 30
  //Each hemisphere has 40 Minor triangles giving a total of 80
  //Each hemisphre has 336 crystals giving a total of 672
 


//Unfortuanetly the top ad bottom hemisphere contain Majot triangles
//tthat overlap. Hence will now place crystals directly in the lab
//this will also prevent problems when including other detectors
//and shielding.

//Major indexes
  G4int MajType[2][15]={{0,3,4,0,1,2,0,1,6,0,5,2,0,1,2},   //36+19+5+36+27+9+36+27+5+36+19+9+36+27+9=336
			{4,3,0,2,1,0,2,5,0,6,1,0,2,1,0}};  //5+19+36+9+27+36+9+19+36+5+27+36+9+27+36=336
  G4int MajRTindex[2][15]={{1,2,3,5,6,7,9, 10,11,13,14,15,17,18,19},  
			{2,3,4,6,7,8,10,11,12,14,15,16,18,19,20}};
  G4int MajCopy[2][15]={ {1,2,3,5,6,7,9,10,11,13,14,15,17,18,19},  //Equivalent to mtnum in ugeom.F
			 {2,3,4,6,7,8,10,11,12,14,15,16,18,19,20}};
  //Minor indices
  G4int MinType[7][4]={{0,1,2,3},        //9+9+9+9=36        4
		       {1,2,3,20},       //9+9+9  =27        3
		       {0,20,20,20},     //9      =9         1
		       {4,5,6,20},       //5+9+5  =19        3
		       {7,20,20,20},     //5      =5         1
		       {9,10,11,20},     //9+5+5  =19        3
		       {8,20,20,20}};    //5      =5         1
  G4int MinRTindex[7][4]={{21,22,24,27},
		     {22,24,27,0},
		     {21,0,0,0},
		     {22,24,27,0},
		     {21,0,0,0},
		     {22,24,27,0},
		     {21,0,0,0}};
  G4int MinCopy[7][4]={{1,2,4,3},
		  {2,4,3,0},
		  {1,0,0,0},
		  {2,4,3,0},
		  {1,0,0,0},
		  {2,4,3,0},
		  {1,0,0,0}};
  //Crystal indices
  G4int CrystType[12][9]={{1,2,3,4,5,6,7,8,9},
		      {1,2,3,4,5,6,7,8,9},
		      {1,2,3,4,5,6,7,8,9},
		      {10,10,10,11,11,11,11,11,11},
		      {1,2,3,5,6,0,0,0,0},
		      {1,2,3,4,5,6,7,8,9},
		      {10,10,11,11,11,0,0,0,0},
		      {1,3,4,8,9,0,0,0,0},
		      {1,2,3,5,6,0,0,0,0},
		      {1,2,3,4,5,6,7,8,9},
		      {1,3,4,8,9,0,0,0,0},
		      {10,10,11,11,11,0,0,0,0}};
  G4int CrystRTindex[12][9]={{31,32,33,34,35,36,37,38,39},
			{31,32,33,34,35,36,37,38,39},
			{31,32,33,34,35,36,37,38,39},
			{41,42,43,44,45,46,47,48,49},
			{31,32,33,35,36,0,0,0,0},
			{31,32,33,34,35,36,37,38,39},
			{42,43,45,46,48,0,0,0,0},
			{31,33,34,38,39,0,0,0,0},
			{31,32,33,35,36,0,0,0,0},
			{31,32,33,34,35,36,37,38,39},
			{31,33,34,38,39,0,0,0,0},
			{41,42,44,45,47,0,0,0,0}}; //the order of the rotation and transformation index
  G4int CrystCopy[12][9]={{1,2,3,4,5,6,7,8,9},
		     {5,7,6,2,9,8,4,3,1},
		     {9,4,8,7,1,3,2,6,5},
		     {9,1,5,8,3,6,4,2,7},
		     {5,7,6,9,8,0,0,0,0},
		     {9,4,8,7,1,3,2,6,5},
		     {1,5,3,6,2,0,0,0,0},
		     {1,3,4,8,9,0,0,0,0},
		     {1,2,3,5,6,0,0,0,0},
		     {5,7,6,2,9,8,4,3,1},
		     {9,8,7,6,5,0,0,0,0},
		     {9,1,8,3,4,0,0,0,0}};  //the copy number of the crystal in the minor (has to be right to get correct hit id!

  if(fIsInteractive==1){
    //set up sensitive detectors
    if(!fVisCBSD){
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      fVisCBSD = new A2VisSD("VisCBSD",fNcrystals);
      SDman->AddNewDetector( fVisCBSD );		
    }
  }

  char crystname[20];
  G4int ihemi,imaj,imin,majt,mint,icryst,cryt,copy;
  G4int icut=fNCrystTypes+1;
  for(ihemi=0;ihemi<2;ihemi++){//loop over hemispheres
    for(imaj=0;imaj<15;imaj++){//loop over major triangles
      //  for(ihemi=0;ihemi<1;ihemi++){//loop over hemispheres
      //    for(imaj=9;imaj==9;imaj++){//loop over major triangles
      imin=0;
      majt=MajType[ihemi][imaj];
      while(MinCopy[majt][imin]!=0&&imin<4){//loop over minor traingle
	mint=MinType[majt][imin];
	icryst=0;
	while(CrystCopy[mint][icryst]!=0&&icryst<9){//loop over crystals
	  cryt=CrystType[mint][icryst];
	  G4Transform3D trans=G4Transform3D(fRot[CrystRTindex[mint][icryst]]->inverse(),fTrans[CrystRTindex[mint][icryst]]); //position in minor
	  trans=G4Transform3D(fRot[MinRTindex[majt][imin]]->inverse(),fTrans[MinRTindex[majt][imin]])*trans;//position in major
	  trans=G4Transform3D(fRot[MajRTindex[ihemi][imaj]]->inverse(),fTrans[MajRTindex[ihemi][imaj]])*trans;//position in hemisphere
	  if(ihemi==0)trans=G4Transform3D(fRot[25]->inverse(),G4ThreeVector(0,fGap.x()+0.063*inch,0))*trans;//position in lab
	  if(ihemi==1)trans=G4Transform3D(fRot[25]->inverse(),-G4ThreeVector(0,fGap.y()+0.063*inch,0))*trans;//position in lab
	  copy=36*(MajCopy[ihemi][imaj]-1)+9*(MinCopy[majt][imin]-1)+CrystCopy[mint][icryst]-1;//calculate copy number so can convert to AcquRoot number in sensitive detector
	  sprintf(crystname,"CRYSTAL_%d",copy);
	  if(fIsInteractive==1){
	    //Cut crystals on minor traingles at the beam ntrance and exit
	    if((MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==2)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==2)||
	       (MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==4)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==4)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==1)){
	      //Need to subtract off the CCUT volume
	      //First rotate it into the frame of the crystal  
	      G4SubtractionSolid* subtract=new G4SubtractionSolid("Subtract",fCrystal[cryt],fCCUT,trans.inverse());
	      //union used to check positioning of CCUT to be subtracted, need to change z half length to 20*CLHEP::cm or visualisation won't work
	      //G4UnionSolid* subtract=new G4UnionSolid("Subtract",fCrystal[cryt],fCCUT,trans.inverse());
	      fCrystLogic[copy]=new G4LogicalVolume(subtract,fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"),fCrystal[cryt]->GetName()+G4String(crystname));
	  
	      fCrystLogic[copy]->SetSensitiveDetector(fVisCBSD);
	      fregionCB->AddRootLogicalVolume(fCrystLogic[copy]);
	      fCrystPhysi[copy]=new G4PVPlacement(trans,fCrystLogic[copy],crystname,fMotherLogic,false,fCrystalConvert[copy],false);
	      fCrystLogic[copy]->SetVisAttributes(fCrystVisAtt);
	      icut++;
	      
	    }
	    else {
	      fCrystLogic[copy]=new G4LogicalVolume(fCrystal[cryt],fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"),fCrystal[cryt]->GetName());
	      
	      fCrystLogic[copy]->SetSensitiveDetector(fVisCBSD);
	      fregionCB->AddRootLogicalVolume(fCrystLogic[copy]);

	      fCrystPhysi[copy]=new G4PVPlacement(trans,fCrystLogic[copy],crystname,fMotherLogic,false,fCrystalConvert[copy],false);
	      if(fCrystLogic[copy])fCrystLogic[copy]->SetVisAttributes(fCrystVisAtt);

	    }
	    
	  }
	  else  {
	    //only make logic volumes for the cut crystals the others can share!
	    //Cut crystals on minor traingles at the beam ntrance and exit
	    if((MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==2)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==3&&MinCopy[majt][imin]==2)||
	       (MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==2&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==4)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==14&&MinCopy[majt][imin]==1)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==4)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==3)||
	       (MajCopy[ihemi][imaj]==11&&MinCopy[majt][imin]==1)){
	      //Need to subtract off the CCUT volume
	      //First rotate it into the frame of the crystal  
	      G4SubtractionSolid* subtract=new G4SubtractionSolid("Subtract",fCrystal[cryt],fCCUT,trans.inverse());
	      //union used to check positioning of CCUT to be subtracted, need to change z half length to 20*CLHEP::cm or visualisation won't work
	      //G4UnionSolid* subtract=new G4UnionSolid("Subtract",fCrystal[cryt],fCCUT,trans.inverse());
	      fCrystLogic[icut]=new G4LogicalVolume(subtract,fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"),fCrystal[cryt]->GetName()+G4String(crystname));
	      fCrystLogic[icut]->SetVisAttributes(fCrystVisAtt);

	      fCrystLogic[icut]->SetSensitiveDetector(fCBSD);
	      fregionCB->AddRootLogicalVolume(fCrystLogic[icut]);

	      fCrystPhysi[copy]=new G4PVPlacement(trans,fCrystLogic[icut],crystname,fMotherLogic,false,fCrystalConvert[copy],false);
	      icut++;//Make new logical volume for each cut crystal
	    }
	    else fCrystPhysi[copy]=new G4PVPlacement(trans,fCrystLogic[cryt],crystname,fMotherLogic,false,fCrystalConvert[copy],false);
	  }
	  icryst++;
	}
	imin++;

      }
    }
  }





}

void A2DetCrystalBall::MakeCrystals(){
  //Crystal parameters are taken from CrystalBall.C
  //which is a root conversion of cbsim ugeom.F
  //original parameters were calculated in the Prism.F file
  //There are 11 different shapes of crystal as defined below, each needs it's own logical volume
  G4double Vanish=1*CLHEP::nm;//small length to make G4Trap work with approx 3 sides
  G4double Shrink=1;///for testing
  fCrystal[1]=new G4Trap("CR01", 20.19144*Shrink*CLHEP::cm,2.217791*CLHEP::deg,89.99987*CLHEP::deg,1.875317*Shrink*CLHEP::cm,2.707754*Shrink*CLHEP::cm,Vanish,    0.*CLHEP::deg,         4.875824*Shrink*CLHEP::cm, 7.04016*Shrink*CLHEP::cm,  Vanish,   0.*CLHEP::deg);
  fCrystal[2]=new G4Trap("CR02", 20.18581*Shrink*CLHEP::cm,2.130379*CLHEP::deg,90.53368*CLHEP::deg,1.975611*Shrink*CLHEP::cm,2.723455*Shrink*CLHEP::cm,Vanish,    -0.2537469*CLHEP::deg, 5.136588*Shrink*CLHEP::cm, 7.080982*Shrink*CLHEP::cm, Vanish,   -0.2537412*CLHEP::deg);
  fCrystal[3]=new G4Trap("CR03", 20.18625*Shrink*CLHEP::cm,2.093683*CLHEP::deg,269.9998*CLHEP::deg,1.987023*Shrink*CLHEP::cm,Vanish,   2.7077546*Shrink*CLHEP::cm,0.*CLHEP::deg,         5.16626*Shrink*CLHEP::cm,  Vanish,    7.04016*Shrink*CLHEP::cm, 0.*CLHEP::deg);
  fCrystal[4]=new G4Trap("CR04", 20.18581*Shrink*CLHEP::cm,2.130379*CLHEP::deg,89.46632*CLHEP::deg,1.975611*Shrink*CLHEP::cm,2.723455*Shrink*CLHEP::cm,Vanish,    0.2537469*CLHEP::deg,  5.136588*Shrink*CLHEP::cm, 7.080982*Shrink*CLHEP::cm, Vanish,   0.2537412*CLHEP::deg);
  fCrystal[5]=new G4Trap("CR05", 20.1931*Shrink*CLHEP::cm, 1.978827*CLHEP::deg,95.34957*CLHEP::deg,1.961185*Shrink*CLHEP::cm,2.615028*Shrink*CLHEP::cm,Vanish,    -2.374402*CLHEP::deg,  5.09908*Shrink*CLHEP::cm,  6.799075*Shrink*CLHEP::cm, Vanish,   -2.374406*CLHEP::deg);
  fCrystal[6]=new G4Trap("CR06", 20.18483*Shrink*CLHEP::cm,2.109143*CLHEP::deg,268.5951*CLHEP::deg,1.995582*Shrink*CLHEP::cm,Vanish,   2.723455*Shrink*CLHEP::cm, 0.6542568*CLHEP::deg,  5.188515*Shrink*CLHEP::cm, Vanish,    7.080982*Shrink*CLHEP::cm,0.6542572*CLHEP::deg);
  fCrystal[7]=new G4Trap("CR07", 20.18352*Shrink*CLHEP::cm,2.114796*CLHEP::deg,89.99928*CLHEP::deg,2.007283*Shrink*CLHEP::cm,2.734993*Shrink*CLHEP::cm,Vanish,    0.*CLHEP::deg,         5.218937*Shrink*CLHEP::cm, 7.110981*Shrink*CLHEP::cm, Vanish,   0.*CLHEP::deg);
  fCrystal[8]=new G4Trap("CR08", 20.18483*Shrink*CLHEP::cm,2.109143*CLHEP::deg,271.4048*CLHEP::deg,1.995582*Shrink*CLHEP::cm,Vanish,   2.7234556*Shrink*CLHEP::cm,-0.6542568*CLHEP::deg, 5.188515*Shrink*CLHEP::cm, Vanish,    7.080982*Shrink*CLHEP::cm,-0.6542572*CLHEP::deg);
  fCrystal[9]=new G4Trap("CR09", 20.1931*Shrink*CLHEP::cm, 1.978827*CLHEP::deg,84.65044*CLHEP::deg,1.961185*Shrink*CLHEP::cm,2.615028*Shrink*CLHEP::cm,Vanish,    2.374402*CLHEP::deg,   5.09908*Shrink*CLHEP::cm,  6.799075*Shrink*CLHEP::cm, Vanish,   2.374406*CLHEP::deg);
  fCrystal[10]=new G4Trap("CR10",20.17123*Shrink*CLHEP::cm,1.674679*CLHEP::deg,260.4183*CLHEP::deg,2.331228*Shrink*CLHEP::cm,Vanish,   2.6150286*Shrink*CLHEP::cm,3.013278*CLHEP::deg,   6.061194*Shrink*CLHEP::cm, Vanish,    6.799075*Shrink*CLHEP::cm,3.013267*CLHEP::deg);
  fCrystal[11]=new G4Trap("CR11",20.16107*Shrink*CLHEP::cm,1.812293*CLHEP::deg,91.16*CLHEP::deg,   2.3732*Shrink*CLHEP::cm,  2.751179*Shrink*CLHEP::cm,Vanish,    -0.3896213*CLHEP::deg, 6.17032*Shrink*CLHEP::cm,  7.153065*Shrink*CLHEP::cm, Vanish,   -0.3896199*CLHEP::deg);
  if(fIsInteractive==1)return;
  //set up sensitive detectors
  if(!fCBSD){
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fCBSD = new A2SD("CBSD",fNcrystals);
    SDman->AddNewDetector( fCBSD );		
  }
  //Use the shapes to make the different Logical volumes
  for(G4int i=1;i<=fNCrystTypes;i++){
    fCrystLogic[i]=new G4LogicalVolume(fCrystal[i],fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"),fCrystal[i]->GetName());
    // fCrystVisAtt= new G4VisAttributes(G4Colour(.1,.5,.5));
   fCrystVisAtt= new G4VisAttributes(G4Colour(.1,.8,.5));
    fCrystLogic[i]->SetVisAttributes(fCrystVisAtt);
  //Make the crystals sensitive detectors
    fCrystLogic[i]->SetSensitiveDetector(fCBSD);
    fregionCB->AddRootLogicalVolume(fCrystLogic[i]);
  }
}
void A2DetCrystalBall::MakeTransformVectors(){
  //Try assuming each rotation matrix has a corresponding transf. matrix
  //therefore use the same numbering
  //These transforms come from the same AddNode(Vol,N,Trans,Rot) line
  //as the correspondingly numbered rotations
  //
  fTrans=new G4ThreeVector[100];

  // fTrans[]=G4ThreeVector();
  fTrans[1]=G4ThreeVector(28.86752*CLHEP::cm,-9.379624*CLHEP::cm,39.73272*CLHEP::cm);
  fTrans[2]=G4ThreeVector(46.70862*CLHEP::cm,-15.17655*CLHEP::cm,9.379623*CLHEP::cm);
  fTrans[3]=G4ThreeVector(46.70862*CLHEP::cm,15.17655*CLHEP::cm,-9.379623*CLHEP::cm);
  fTrans[4]=G4ThreeVector(28.86752*CLHEP::cm,9.379626*CLHEP::cm,-39.73272*CLHEP::cm);  
  fTrans[5]=G4ThreeVector(17.8411*CLHEP::cm,24.55618*CLHEP::cm,39.73272*CLHEP::cm);
  fTrans[6]=G4ThreeVector(28.86751*CLHEP::cm,39.73272*CLHEP::cm,9.379623*CLHEP::cm);
  fTrans[7]=G4ThreeVector(0.5975973E-06*CLHEP::cm,49.11235*CLHEP::cm,-9.379623*CLHEP::cm);
  fTrans[8]=G4ThreeVector(0.3693355E-06*CLHEP::cm,30.35311*CLHEP::cm,-39.73272*CLHEP::cm);
  fTrans[9]=G4ThreeVector(-17.8411*CLHEP::cm,24.55618*CLHEP::cm,39.73272*CLHEP::cm);
  fTrans[10]=G4ThreeVector(-28.86751*CLHEP::cm,39.73272*CLHEP::cm,9.379623*CLHEP::cm);
  fTrans[11]=G4ThreeVector(-46.70861*CLHEP::cm,15.17655*CLHEP::cm,-9.379623*CLHEP::cm);
  fTrans[12]=G4ThreeVector(-28.86752*CLHEP::cm,9.379626*CLHEP::cm,-39.73272*CLHEP::cm);
  fTrans[13]=G4ThreeVector(-28.86752*CLHEP::cm,-9.379623*CLHEP::cm,39.73272*CLHEP::cm);
  fTrans[14]=G4ThreeVector(-46.70862*CLHEP::cm,-15.17655*CLHEP::cm,9.379623*CLHEP::cm);
  fTrans[15]=G4ThreeVector(-28.86752*CLHEP::cm,-39.73272*CLHEP::cm,-9.379623*CLHEP::cm);
  fTrans[16]=G4ThreeVector(-17.84111*CLHEP::cm,-24.55618*CLHEP::cm,-39.73272*CLHEP::cm);
  fTrans[17]=G4ThreeVector(-0.1108006E-05*CLHEP::cm,-30.3531*CLHEP::cm,39.73272*CLHEP::cm);
  fTrans[18]=G4ThreeVector(-0.1792792E-05*CLHEP::cm,-49.11235*CLHEP::cm,9.379623*CLHEP::cm);
  fTrans[19]=G4ThreeVector(28.86751*CLHEP::cm,-39.73272*CLHEP::cm,-9.379623*CLHEP::cm);
  fTrans[20]=G4ThreeVector(17.84111*CLHEP::cm,-24.55618*CLHEP::cm,-39.73272*CLHEP::cm); 
  fTrans[21]=G4ThreeVector(0.*CLHEP::cm,19.63683*CLHEP::cm,-6.779331*CLHEP::cm);
  fTrans[22]=G4ThreeVector(-17.00599*CLHEP::cm,-9.818414*CLHEP::cm,-6.779331*CLHEP::cm);
  fTrans[23]=G4ThreeVector();
  fTrans[24]=G4ThreeVector(17.00599*CLHEP::cm,-9.818414*CLHEP::cm,-6.779331*CLHEP::cm);
  //  fTrans[25]=G4ThreeVector(0.,0.126*CLHEP::cm,0.);//split 2 hemi in the LABB frame
  //  fTrans[26]=G4ThreeVector(0.,0,0.126*CLHEP::cm);//or split 2 hemi in one sphere frame 
  // fTrans[25]=G4ThreeVector(0.,fGap+0.076*CLHEP::cm,0.);//split 2 hemi in the LABB frame
  // fTrans[26]=G4ThreeVector(0.,0,fGap+0.076*CLHEP::cm);//or split 2 hemi in one sphere frame 
  fTrans[27]=G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,-3.291383*CLHEP::cm);  //added in for central minor placement with no rotation
  fTrans[28]=G4ThreeVector(0.*CLHEP::cm,19.63683*CLHEP::cm,-6.779331*CLHEP::cm);//fix for 21
  fTrans[29]=G4ThreeVector();
  fTrans[30]=G4ThreeVector();
  fTrans[31]=G4ThreeVector(0.*CLHEP::cm,6.561235*CLHEP::cm,-3.284699*CLHEP::cm);
  fTrans[32]=G4ThreeVector(-4.873957*CLHEP::cm,-0.2755656*CLHEP::cm,-2.268451*CLHEP::cm);
  fTrans[33]=G4ThreeVector(0.*CLHEP::cm,-0.2669953*CLHEP::cm,-2.005777*CLHEP::cm);
  fTrans[34]=G4ThreeVector(4.873957*CLHEP::cm,-0.2755656*CLHEP::cm,-2.268451*CLHEP::cm);
  fTrans[35]=G4ThreeVector(-9.636768*CLHEP::cm,-7.360624*CLHEP::cm,-2.795638*CLHEP::cm);
  fTrans[36]=G4ThreeVector(-4.898472*CLHEP::cm,-7.413983*CLHEP::cm,-2.043895*CLHEP::cm);
  fTrans[37]=G4ThreeVector(0.*CLHEP::cm,-7.405412*CLHEP::cm,-1.781221*CLHEP::cm);
  fTrans[38]=G4ThreeVector(4.898472*CLHEP::cm,-7.413983*CLHEP::cm,-2.043895*CLHEP::cm);
  fTrans[39]=G4ThreeVector(9.636768*CLHEP::cm,-7.360624*CLHEP::cm,-2.795638*CLHEP::cm);
  fTrans[40]=G4ThreeVector();
  fTrans[41]=G4ThreeVector(-9.685799*CLHEP::cm,4.170954*CLHEP::cm,-2.545603*CLHEP::cm);
  fTrans[42]=G4ThreeVector(1.230747*CLHEP::cm,-10.47362*CLHEP::cm,-2.545603*CLHEP::cm);
  fTrans[43]=G4ThreeVector(8.455051*CLHEP::cm,6.30267*CLHEP::cm,-2.545603*CLHEP::cm);
  fTrans[44]=G4ThreeVector(-4.922987*CLHEP::cm,4.263432*CLHEP::cm,-1.793192*CLHEP::cm);
  fTrans[45]=G4ThreeVector(-1.230747*CLHEP::cm,-6.395147*CLHEP::cm,-1.793192*CLHEP::cm);
  fTrans[46]=G4ThreeVector(6.153733*CLHEP::cm,2.131716*CLHEP::cm,-1.793192*CLHEP::cm);
  fTrans[47]=G4ThreeVector(-6.153733*CLHEP::cm,-2.131716*CLHEP::cm,-1.793192*CLHEP::cm);
  fTrans[48]=G4ThreeVector(4.922987*CLHEP::cm,-4.263432*CLHEP::cm,-1.793192*CLHEP::cm);
  fTrans[49]=G4ThreeVector(1.230747*CLHEP::cm,6.395147*CLHEP::cm,-1.793192*CLHEP::cm);
//   fTrans[51]=G4ThreeVector(-19.60074*CLHEP::cm,12.32638*CLHEP::cm,60.68302*CLHEP::cm);
//   fTrans[52]=G4ThreeVector(19.60074*CLHEP::cm,-12.32638*CLHEP::cm,60.68302*CLHEP::cm);
//   fTrans[53]=G4ThreeVector(-19.60074*CLHEP::cm,12.32638*CLHEP::cm,-60.68302*CLHEP::cm);
//   fTrans[54]=G4ThreeVector(19.60074*CLHEP::cm,-12.32638*CLHEP::cm,-60.68302*CLHEP::cm);
//   fTrans[61]=G4ThreeVector(0.3097284*CLHEP::cm,22.30712*CLHEP::cm,60.84964*CLHEP::cm);
//   fTrans[62]=G4ThreeVector(-0.3097284*CLHEP::cm,-22.30712*CLHEP::cm,60.84964*CLHEP::cm);
//   fTrans[63]=G4ThreeVector(0.3097284*CLHEP::cm,22.30712*CLHEP::cm,-60.84964*CLHEP::cm);
//   fTrans[64]=G4ThreeVector(-0.3097284*CLHEP::cm,-22.30712*CLHEP::cm,-60.84964*CLHEP::cm);
//   fTrans[71]=G4ThreeVector(19.73018*CLHEP::cm,10.26593*CLHEP::cm,60.84964*CLHEP::cm);
//   fTrans[72]=G4ThreeVector(-19.73018*CLHEP::cm,-10.26593*CLHEP::cm,60.84964*CLHEP::cm);
//   fTrans[73]=G4ThreeVector(19.73018*CLHEP::cm,10.26593*CLHEP::cm,-60.84964*CLHEP::cm);
//   fTrans[74]=G4ThreeVector(-19.73018*CLHEP::cm,-10.26593*CLHEP::cm,-60.84964*CLHEP::cm);
  //note 61-74 are slighly changed in sergeis new version cb_mamic.C
  //old version above
  fTrans[51]=G4ThreeVector(-20.18045*CLHEP::cm,12.63722*CLHEP::cm,63.02744*CLHEP::cm);
  fTrans[52]=G4ThreeVector(20.18045*CLHEP::cm,-12.63722*CLHEP::cm,63.02744*CLHEP::cm);
  fTrans[53]=G4ThreeVector(-20.18045*CLHEP::cm,12.63722*CLHEP::cm,-63.02744*CLHEP::cm);
  fTrans[54]=G4ThreeVector(20.18045*CLHEP::cm,-12.63722*CLHEP::cm,-63.02744*CLHEP::cm);
  fTrans[61]=G4ThreeVector(0.3137556*CLHEP::cm,22.54553*CLHEP::cm,63.23774*CLHEP::cm);
  fTrans[62]=G4ThreeVector(-0.3137556*CLHEP::cm,-22.54553*CLHEP::cm,63.23774*CLHEP::cm);
  fTrans[63]=G4ThreeVector(0.3137556*CLHEP::cm,22.54553*CLHEP::cm,-63.23774*CLHEP::cm);
  fTrans[64]=G4ThreeVector(-0.3137556*CLHEP::cm,-22.54553*CLHEP::cm,-63.23774*CLHEP::cm);
  fTrans[71]=G4ThreeVector(19.98671*CLHEP::cm,10.34778*CLHEP::cm,63.23774*CLHEP::cm);
  fTrans[72]=G4ThreeVector(-19.98671*CLHEP::cm,-10.34778*CLHEP::cm,63.23774*CLHEP::cm);
  fTrans[73]=G4ThreeVector(19.98671*CLHEP::cm,10.34778*CLHEP::cm,-63.23774*CLHEP::cm);
  fTrans[74]=G4ThreeVector(-19.98671*CLHEP::cm,-10.34778*CLHEP::cm,-63.23774*CLHEP::cm);
  // fTrans[96]=G4ThreeVector(0*CLHEP::cm,0*CLHEP::cm,28.4*CLHEP::cm);
  //TUNL offset slighly changed in sergeis new version cb_mamic.C
  //old version above
  //fTrans[96]=G4ThreeVector(0*CLHEP::cm,fGap,28.1*CLHEP::cm!);

}
void A2DetCrystalBall::MakeRotationMatrices(){
  //Create G4Rotation Matrices from TGeoRotation defined in ROOT geometry file crystalball.C
  //Numbering is as fot the fortran geometry ugeom.F
  TGeoRotation* rot[100];
  for (G4int i=0;i<100;i++)
    rot[i]=NULL;


  rot[1] = new TGeoRotation("rot1",90,72,52.62263,162,37.37737,-18);
  rot[2] = new TGeoRotation("rot2",90,-108,169.1877,-18,79.18768,-18);
  rot[3] = new TGeoRotation("rot3",90,108,10.81232,18,100.8123,18);
  rot[4] = new TGeoRotation("rot4",90,-72,127.3774,198,142.6226,18);
  rot[5] = new TGeoRotation("rot5",90,144,52.62263,234,37.37737,54);
  rot[6] = new TGeoRotation("rot6",90,-36,169.1877,54,79.18768,54);
  rot[7] = new TGeoRotation("rot7",90,180,10.81232,90,100.8123,90);
  rot[8] = new TGeoRotation("rot8",90,0,127.3774,270,142.6226,90);
  rot[9] = new TGeoRotation("rot9",90,216,52.62263,306,37.37737,126);
  rot[10] = new TGeoRotation("rot10",90,36,169.1877,126,79.18768,126);
  rot[11] = new TGeoRotation("rot11",90,252,10.81232,162,100.8123,162);
  rot[12] = new TGeoRotation("rot12",90,72,127.3774,342,142.6226,162);
  rot[13] = new TGeoRotation("rot13",90,288,52.62263,378,37.37737,198);
  rot[14] = new TGeoRotation("rot14",90,108,169.1877,198,79.18768,198);
  rot[15] = new TGeoRotation("rot15",90,324,10.81232,234,100.8123,234);
  rot[16] = new TGeoRotation("rot16",90,144,127.3774,414,142.6226,234);
  rot[17] = new TGeoRotation("rot17",90,360,52.62263,450,37.37737,270);
  rot[18] = new TGeoRotation("rot18",90,180,169.1877,270,79.18768,270);
  rot[19] = new TGeoRotation("rot19",90,396,10.81232,306,100.8123,306);
  rot[20] = new TGeoRotation("rot20",90,216,127.3774,486,142.6226,306);
  rot[21] = new TGeoRotation("rot21",90,0,  108.0291,90 ,18.0291,90);
  rot[22] = new TGeoRotation("rot22",90,120,108.0291,210,18.0291,210);
  rot[24] = new TGeoRotation("rot24",90,240,108.0291,330,18.0291,330);
  rot[23] = new TGeoRotation("rot23",90,180,90,90,180,0);
  rot[25] = new TGeoRotation("rot25",0,0,90,0,90,90);
  rot[26] = new TGeoRotation("rot26",90,90,0,0,90,0);
  rot[27] = new TGeoRotation("rot27"); //added in for central minor placement with no rotation
  rot[31] = new TGeoRotation("rot31",90,360,102.8996,90,12.89964,90);
  rot[32] = new TGeoRotation("rot32",83.84803,0.2015059,94.2088,89.74704,7.463124,145.5692);
  rot[33] = new TGeoRotation("rot33",90,360,98.44637,90,8.446371,90.00005);
  rot[34] = new TGeoRotation("rot34",96.15197,359.7985,94.2088,90.25296,7.463124,34.43079);
  rot[35] = new TGeoRotation("rot35",78.00652,358.4585,85.20763,89.47906,12.9421,200.8759);
  rot[36] = new TGeoRotation("rot36",83.84803,0.2015059,89.46205,90.25949,6.175653,185.2375);
  rot[37] = new TGeoRotation("rot37",90,360,85.22453,90,4.775468,269.9999);
  rot[38] = new TGeoRotation("rot38",96.15197,359.7985,89.46205,89.74051,6.175653,354.7625);
  rot[39] = new TGeoRotation("rot39",101.9935,1.541459,85.20763,90.52094,12.9421,339.1241);
  rot[41] = new TGeoRotation("rot41",78.12711,2.301167,97.25146,90.76825,13.96771,150);
  rot[42] = new TGeoRotation("rot42",78.12711,122.3012,97.25146,210.7682,13.96771,270);
  rot[43] = new TGeoRotation("rot43",78.12711,242.3012,97.25146,330.7682,13.96771,390);
  rot[44] = new TGeoRotation("rot44",83.78185,0,93.55728,89.61192,7.170748,149.9998);
  rot[45] = new TGeoRotation("rot45",83.78185,120,93.55728,209.6119,7.170748,269.9998);
  rot[46] = new TGeoRotation("rot46",83.78185,240,93.55728,329.6119,7.170748,389.9998);
  rot[47] = new TGeoRotation("rot47",83.78185,60,93.55728,149.6119,7.170748,209.9998);
  rot[48] = new TGeoRotation("rot48",83.78185,180,93.55728,269.6119,7.170748,329.9998);
  rot[49] = new TGeoRotation("rot49",83.78185,300,93.55728,389.6119,7.170748,449.9998);
  rot[51] = new TGeoRotation("rot51",90.04582,58.22001,110.8999,148.2375,20.9,148.1);
  rot[52] = new TGeoRotation("rot52",90.04582,238.22,110.8999,328.2375,20.9,328.1);
  rot[53] = new TGeoRotation("rot53",90.04582,238.22,69.10005,148.2375,159.1,148.1);
  rot[54] = new TGeoRotation("rot54",90.04582,58.22001,69.10005,328.2375,159.1,328.1);
  //note 61-74 are slighly changed in sergeis new version cb_mamic.C
  rot[61] = new TGeoRotation("rot61",87.16713,351.3,109.5794,80.29152,19.8,89.2);
  rot[62]= new TGeoRotation("rot62",87.16713,171.3,109.5794,260.2915,19.8,269.2);
  rot[63] = new TGeoRotation("rot63",87.16713,171.3,70.42059,80.29152,160.2,89.2);
  rot[64] = new TGeoRotation("rot64",87.16713,351.3,70.42059,260.2915,160.2,269.2);
  rot[71] = new TGeoRotation("rot71",92.90398,305.3,109.5682,36.33317,19.8,27.2);
  rot[72] = new TGeoRotation("rot72",92.90398,125.3,109.5682,216.3332,19.8,207.2);
  rot[73] = new TGeoRotation("rot73",92.90398,125.3,70.43185,36.33317,160.2,27.2);
  rot[74] = new TGeoRotation("rot74",92.90398,305.3,70.43185,216.3332,160.2,207.2);
//   rot[61] = new TGeoRotation("rot61",87.12058,351.3,109.8749,80.25814,20.1,89.2);
//   rot[62] = new TGeoRotation("rot62",87.12058,171.3,109.8749,260.2581,20.1,269.2);
//   rot[63] = new TGeoRotation("rot63",87.12058,171.3,70.12505,80.25814,159.9,89.2);
//   rot[64] = new TGeoRotation("rot64",87.12058,351.3,70.12505,260.2581,159.9,269.2);
//   rot[71] = new TGeoRotation("rot71",92.95169,305.3,109.8635,36.36736,20.1,27.2);
//   rot[72] = new TGeoRotation("rot72",92.95169,125.3,109.8635,216.3674,20.1,207.2);
//   rot[73] = new TGeoRotation("rot73",92.95169,125.3,70.13654,36.36736,159.9,27.2);
//   rot[74] = new TGeoRotation("rot74",92.95169,305.3,70.13654,216.3674,159.9,207.2);
  rot[80] = new TGeoRotation("rot80",90,1.2,90,91.2,0,0);
  //  rot[95] = new TGeoRotation("rot95",90,90,180,0,90,0);//Gives imroper rotation
  rot[95] = new TGeoRotation("rot95",90,90,180,0,90,0);//Gives imroper rotation

  rot[96] = new TGeoRotation("rot96",90,0,90,270,0,0);//Gives imroper rotation
  rot[98] = new TGeoRotation("rot98",0,0,90,180,90,90);//Gives imroper rotation

  fRot=new G4RotationMatrix*[100];

  for(G4int i=0;i<100;i++)
    if(rot[i]){
      //G4cout<<"Rotation Matrix "<<i<<G4endl;
      if(i==95){
// 	const Double_t *CLHEP::m = rot[26]->Inverse().GetRotationMatrix();
// 	fRot[i]=new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));
// 	G4cout<<"MAde 95"<<G4endl;
	fRot[i]=new G4RotationMatrix();
	fRot[i]->rotateX(90*CLHEP::deg);
	fRot[i]->rotateY(90*CLHEP::deg);
      }
      else{
	const Double_t *m = rot[i]->GetRotationMatrix();
	fRot[i]=new G4RotationMatrix(G4ThreeVector(m[0],m[1],m[2]),G4ThreeVector(m[3],m[4],m[5]),G4ThreeVector(m[6],m[7],m[8]));
      }
//       if(i==27) fRot[i]=new G4RotationMatrix();
 
    }
    else fRot[i]=NULL;
  
  //if(rot[i]) delete rot[i];
}
