
#include "A2DetectorConstruction.hh"
#include "A2DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


#include "G4VVisManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Point3D.hh"
#include "G4Polyline.hh"

#include "A2CryoTarget.hh"
#include "A2SolidTarget.hh"
#include "A2PolarizedTarget.hh"

A2DetectorConstruction::A2DetectorConstruction(G4String detSet)
{
  fDetectorSetup=detSet;
  // create commands for interactive definition of the calorimeter
  fDetMessenger = new A2DetectorMessenger(this);

  fIsInteractive=1;

  fUseCB=0;
  fUseTAPS=0;
  fUsePID=0;
  fUseMWPC=0;
  fUseTOF=0;
  fUseCherenkov=0;

  fCrystalBall=NULL;
  fTAPS=NULL;
  fPID=NULL;
  fMWPC=NULL;
  fTOF=NULL;
  fWorldSolid=NULL;
  fWorldLogic=NULL;
  fWorldPhysi=NULL;

  fTOFparFile="TOF.par";
  //fHemiGap=0.4*cm;
  fTarget=NULL;
  fTargetLength=4.8*cm;
  fUseTarget=G4String("NO");
  //Default taps settings as for 2003
  fTAPSSetupFile="taps.dat";
  fTAPSN=510;
  fTAPSZ=175*cm;
  fNPbWO4=0;

  //default settings for PID
  fPIDZ=0.;
  //has to be done here in case use new material for target
  DefineMaterials();

}


A2DetectorConstruction::~A2DetectorConstruction(){
 delete fDetMessenger;
}




G4VPhysicalVolume* A2DetectorConstruction::Construct()
{


  fUseTAPS=0;
  fUsePID=0;
  fUseMWPC=0;
  fUseTOF=0;
  fUseCherenkov=0;

  // read the set up file DetectorSetup.mac
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute "+fDetectorSetup;//macros/DetectorSetup.mac";
  UI->ApplyCommand(command);
  if(fUseCB==0&&fUseTAPS==0&&fUsePID==0&&fUseMWPC==0&&fUseTOF==0&&fUseCherenkov==0&&fUseTarget==G4String("NO")){
    G4cout<<"G4VPhysicalVolume* A2DetectorConstruction::Construct() Don't seem to be simulating any detectors, please check you are using an appopriate detector setup. I tried the file "<<fDetectorSetup<< " I will exit here before the computer explodes"<<G4endl;
    exit(0);
  }


  // Clean old geometry, if any
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  //     
  // World
  //
  fWorldSolid = new G4Box("World",				//its name
			  20*m,20*m,20*m);	//its size
                         
  fWorldLogic = new G4LogicalVolume(fWorldSolid,		//its solid
				    G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),	//its material
                                   "World");		//its name
                                   
  fWorldPhysi = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 fWorldLogic,		//its logical volume
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number


  //Make the crystal ball
  if(fUseCB){
    G4cout<<"A2DetectorConstruction::Construct() Make the crystal ball with hemispheres "<<fHemiGap/cm<<" cm from the equator"<<G4endl;
    fCrystalBall=new A2DetCrystalBall();
    fCrystalBall->SetIsInteractive(fIsInteractive);
    fCrystalBall->SetGap(fHemiGap);
    fCrystalBall->Construct(fWorldLogic);
  }
  if(fUseTAPS){
    G4cout<<"A2DetectorConstruction::Construct() Turn on the taps"<<G4endl;
    G4cout<<"TAPS setup "<<fTAPSSetupFile<<" with "<<fTAPSN<<" crystals, "<<fTAPSZ/cm<<" cm from the target"<<G4endl; 
    fTAPS=new A2DetTAPS(fTAPSSetupFile,fTAPSN,fNPbWO4,fTAPSZ);
    //fTAPS=new A2DetTAPS();
    fTAPS->SetIsInteractive(fIsInteractive);
    fTAPS->Construct(fWorldLogic);
  }
  if(fUsePID){
    G4cout<<"A2DetectorConstruction::Construct() Take the pid "<< fUsePID<<G4endl;
    fPID=new A2DetPID();
    if(fUsePID==1) fPID->Construct1(fWorldLogic,fPIDZ);
    else if(fUsePID==2) fPID->Construct2(fWorldLogic,fPIDZ);
    else {G4cerr<<"There are 2 possible PIDS, please set UsePID to be 1 (2003) or 2 (2007)"<<G4endl; exit(1);}
  }

  if(fUseMWPC){
    G4cout<<"A2DetectorConstruction::Construct() Make the Wire Chambers"<<G4endl;
    if(fUseMWPC==2)G4cout<<"With the anode wires created"<<G4endl;
    fMWPC = new A2DetMWPC();
    if(fUseMWPC==2)fMWPC->UseAnodes(true);
    fMWPC ->Construct(fWorldLogic); 
  }
  if(fUseTOF){
    G4cout<<"A2DetectorConstruction::Construct() ToF time!"<<G4endl;
    fTOF=new A2DetTOF();
    fTOF->ReadParameters(fTOFparFile);
    fTOF->Construct(fWorldLogic);
  }
 if(fUseCherenkov){
    G4cout<<"A2DetectorConstruction::Construct() Make the Cherenkov"<<G4endl;
    fCherenkov=new A2DetCherenkov();
    fCherenkov->SetIsInteractive(fIsInteractive);
    fCherenkov->Construct(fWorldLogic);
 }
  if(fUseTarget!=G4String("NO")){
    G4cout<<"A2DetectorConstruction::Construct() Fill the "<<fUseTarget<<" with "<<fTargetMaterial->GetName()<<G4endl;
    if(fUseTarget=="Cryo") fTarget=static_cast<A2Target*>(new A2CryoTarget());
    else if(fUseTarget=="Solid") fTarget=static_cast<A2Target*>(new A2SolidTarget());
    else if(fUseTarget=="Polarized") fTarget=static_cast<A2Target*>(new A2PolarizedTarget());
    else{G4cerr<<"A2DetectorConstruction::Construct() Target type does not exist. Should be Cryo or Solid. See DetectorSetup.mac"<<G4endl;exit(1);}
    //A2CryoTarget* Target=new  A2CryoTarget();
    fTarget->SetMaterial(fTargetMaterial);
    if(fUseTarget=="Cryo") ((A2CryoTarget*)fTarget)->SetTargetLength(fTargetLength);
    if(fUseTarget=="Polarized")
    {
      G4cout<<"A2DetectorConstruction::Construct() make the polarised target with "<<fTypeMagneticCoils<<" coils"<<G4endl;
      (static_cast<A2PolarizedTarget*>(fTarget))->SetMagneticField(fNameFileFieldMap);
      (static_cast<A2PolarizedTarget*>(fTarget))->SetMagneticCoils(fTypeMagneticCoils);
    }
    fTarget->Construct(fWorldLogic);
  }
  //                                        
  // Visualization attributes
  //
  fWorldLogic->SetVisAttributes (G4VisAttributes::Invisible);

//   G4Tubs *BeamLine=new G4Tubs("BeamLine",0.,1*cm,2*m,0,2*3.1415);
//   G4LogicalVolume* BeamLogic=new G4LogicalVolume(BeamLine,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),"BeamLine");
//   new G4PVPlacement(0,G4ThreeVector(),BeamLogic,"BeamLogic",fWorldLogic,false,1000);
//   G4VisAttributes* beamAtt=new G4VisAttributes(G4Color(0.5,0.5,0.5));
//   beamAtt->SetForceSolid(true);
//   BeamLogic->SetVisAttributes(beamAtt);
  return fWorldPhysi;
}





#include "G4RunManager.hh"

void A2DetectorConstruction::UpdateGeometry()
{

  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}


void A2DetectorConstruction::DefineMaterials()
{ 
  G4double density,z,a,fractionmass;
  G4int ncomponents;

  //Add materials not contained in default Nist list
  G4NistManager* NistManager=G4NistManager::Instance();

  //Rohacell. From cbsim.  The formula is (C4 H7 N 0)n. Density is 0.057 g/cm3.  (V.K)
 G4Material *Roha=new G4Material("A2_ROHACELL",density=0.057*g/cm3, ncomponents=4);  
 Roha->AddElement(NistManager->FindOrBuildElement(6),4);
 Roha->AddElement(NistManager->FindOrBuildElement(1),7);
 Roha->AddElement(NistManager->FindOrBuildElement(7),1);
 Roha->AddElement(NistManager->FindOrBuildElement(8),1);
 //NistManager->RegisterMaterial(Roha);

 //Wire Chamber Material. 74.5% Ar, 25% ethane, 0.5% freon. Density is 0.046g/cm3.
 G4Material *WCmat=new G4Material("A2_WCMAT", density=0.046*g/cm3, ncomponents=3,kStateGas);
 WCmat->AddElement(NistManager->FindOrBuildElement(18),fractionmass=0.745);
 WCmat->AddMaterial(NistManager->FindOrBuildMaterial("G4_ETHANE"), fractionmass=0.25);
 WCmat->AddMaterial(NistManager->FindOrBuildMaterial("G4_FREON-12"), fractionmass=0.005);
 //NistManager->RegisterMaterial(WCmat);

 //Cardboard.From cbsim. MWPC electronics
 G4Material *cboard=new G4Material("A2_CBOARD",density=0.69*g/cm3,ncomponents=3);
 cboard->AddElement(NistManager->FindOrBuildElement(6),fractionmass=0.285714286);
 cboard->AddElement(NistManager->FindOrBuildElement(1),fractionmass=0.476190476);
 cboard->AddElement(NistManager->FindOrBuildElement(8),fractionmass=0.238095238);
 //NistManager->RegisterMaterial(cboard);


 //Fibre GLass. From cbsim.
 G4Material *fglass=new G4Material("A2_FGLASS",density=2.0*g/cm3, ncomponents=8);
 fglass->AddElement(NistManager->FindOrBuildElement(8),fractionmass=0.557);
 fglass->AddElement(NistManager->FindOrBuildElement(14),fractionmass=0.347);
 fglass->AddElement(NistManager->FindOrBuildElement(20),fractionmass=0.3580000E-02);
 fglass->AddElement(NistManager->FindOrBuildElement(13),fractionmass=0.1590000E-02);
 fglass->AddElement(NistManager->FindOrBuildElement(5),fractionmass=0.6840000E-01);
 fglass->AddElement(NistManager->FindOrBuildElement(11),fractionmass=0.7420000E-02);
 fglass->AddElement(NistManager->FindOrBuildElement(19),fractionmass=0.1250000E-01);
 fglass->AddElement(NistManager->FindOrBuildElement(3),fractionmass=0.2320000E-02);
 //NistManager->RegisterMaterial(fglass);

 //Plastic. From cbsim 
 G4Material *plastic=new G4Material("A2_PLASTIC",density=1.19*g/cm3, ncomponents=2);
 plastic->AddElement(NistManager->FindOrBuildElement(6),fractionmass=0.8562844);
 plastic->AddElement(NistManager->FindOrBuildElement(1),fractionmass=0.1437155);
 //NistManager->RegisterMaterial(plastic);

 //Base material. From cbsim
 G4Material *basemat=new G4Material("A2_BASEMAT",density=2.26*g/cm3, ncomponents=2);
 basemat->AddElement(NistManager->FindOrBuildElement(6),fractionmass=0.8562844);
 basemat->AddElement(NistManager->FindOrBuildElement(1),fractionmass=0.1437155);
 //NistManager->RegisterMaterial(basemat);

 //mumetal for pmt shielding. From cbsim.
 G4Material* mumetal=new G4Material("A2_MUMETAL",density=8.7*g/cm3,ncomponents=5);
 mumetal->AddElement(NistManager->FindOrBuildElement(28),fractionmass=0.8);
 mumetal->AddElement(NistManager->FindOrBuildElement(42),fractionmass=0.5000000E-01);
 mumetal->AddElement(NistManager->FindOrBuildElement(14),fractionmass=0.5000000E-02);
 mumetal->AddElement(NistManager->FindOrBuildElement(29),fractionmass=0.2000000E-03);
 mumetal->AddElement(NistManager->FindOrBuildElement(26),fractionmass=0.1448);
 //NistManager->RegisterMaterial(mumetal);

 //brass for pid support
 G4Material* brass=new G4Material("A2_BRASS",density=8.5*g/cm3,ncomponents=2);
 brass->AddElement(NistManager->FindOrBuildElement(29),fractionmass=0.62);
 brass->AddElement(NistManager->FindOrBuildElement(30),fractionmass=0.38);
 //NistManager->RegisterMaterial(brass);

 //Liquid deuterium. From cbsim.
 // NistManager->RegisterMaterial(new G4Material("A2_LD2", z=1., a= 2.014*g/mole, density= 0.162*g/cm3));
 new G4Material("A2_lD2", z=1., a= 2.014*g/mole, density= 0.162*g/cm3);
 new G4Material("A2_lHe3", z=2., a= 3.0160*g/mole, density= 0.059*g/cm3);


//////////
//Materials for Polarized Target:
//////////
 
  G4String symbol;             
  G4int iz, n;                 //iz=number of protons  in an isotope; 
                               // n=number of nucleons in an isotope;
  G4int natoms;
  G4double abundance;

//6% helium 3, 94% helium 4 mixture:
 G4Isotope* A2_He3=new G4Isotope("A2_He3", iz=2, n=3, a=3.0160293*g/mole);
 G4Isotope* A2_He4=new G4Isotope("A2_He4", iz=2, n=4, a=4.002602*g/mole);
 G4Element* A2_Hemix=new G4Element("Helium mixture", symbol="A2_Hemix", ncomponents=2);
 A2_Hemix->AddIsotope(A2_He3, abundance=6*perCent);
 A2_Hemix->AddIsotope(A2_He4, abundance=94*perCent);

//Now make Hemix a material so that density can be defined:
  G4Material* A2_HeMix=new G4Material("A2_HeMix", density=0.14*g/cm3, ncomponents=1);
  A2_HeMix->AddElement(A2_Hemix, natoms=1);

//Butanol (C4H9OH):
  G4Material* A2_Butanol=new G4Material("A2_Butanol", density=0.8*g/cm3, ncomponents=3);
  A2_Butanol->AddElement(NistManager->FindOrBuildElement(6),4);
  A2_Butanol->AddElement(NistManager->FindOrBuildElement(1),10);
  A2_Butanol->AddElement(NistManager->FindOrBuildElement(8),1);

//60% butanol, 40% helium polarized target material:
  G4Material* A2_HeButanol=new G4Material("A2_HeButanol", density=0.6*g/cm3, ncomponents=2);
  A2_HeButanol->AddMaterial(A2_HeMix, fractionmass=0.094);
  A2_HeButanol->AddMaterial(A2_Butanol, fractionmass=0.906);
  
//Stainless steel (18% Cr, 10% Ni, 72% Fe):
  G4Material* A2_SS=new G4Material("A2_SS", density=8000.*kg/m3, ncomponents=3);
  A2_SS->AddElement(NistManager->FindOrBuildElement(24), fractionmass=0.18);
  A2_SS->AddElement(NistManager->FindOrBuildElement(28), fractionmass=0.10);
  A2_SS->AddElement(NistManager->FindOrBuildElement(25), fractionmass=0.72);

//NbTi ***don't know density at cold temperature***:
  G4Material* A2_NbTi=new G4Material("A2_NbTi", density=6.45*g/cm3, ncomponents=2);
  A2_NbTi->AddElement(NistManager->FindOrBuildElement(22), 0.47);
  A2_NbTi->AddElement(NistManager->FindOrBuildElement(41), 0.53);

//Epoxy resin (C21H25Cl05) ***not certain if correct chemical formula or density for colder temperature***:
  G4Material* A2_Resin=new G4Material("A2_Resin", density=1.15*g/cm3, ncomponents=4);
  A2_Resin->AddElement(NistManager->FindOrBuildElement(6), 21);
  A2_Resin->AddElement(NistManager->FindOrBuildElement(1), 25);
  A2_Resin->AddElement(NistManager->FindOrBuildElement(17), 1);
  A2_Resin->AddElement(NistManager->FindOrBuildElement(8), 5);

//Amine Hardener (C8H18N2) ***not certain if correct chemical formula or density for colder temperature***:
  G4Material* A2_13BAC=new G4Material("A2_13BAC", density=0.94*g/cm3, ncomponents=3);
  A2_13BAC->AddElement(NistManager->FindOrBuildElement(6), 8);
  A2_13BAC->AddElement(NistManager->FindOrBuildElement(1), 18);
  A2_13BAC->AddElement(NistManager->FindOrBuildElement(7), 2);

//Epoxy adhesive with mix ratio 100:25 parts by weight resin/hardener ***don't know density at cold temperature***:
  G4Material* A2_Epoxy=new G4Material("A2_Epoxy", density=1.2*g/cm3, ncomponents=2);
  A2_Epoxy->AddMaterial(A2_Resin, fractionmass=0.8);
  A2_Epoxy->AddMaterial(A2_13BAC, fractionmass=0.2);


  /*Now useG4NistManager
 //This function illustrates the possible ways to define materials
 
  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;  
  G4int iz, n;                 //iz=number of protons  in an isotope; 
                             // n=number of nucleons in an isotope;
  
  G4int ncomponents, natoms;
  G4double abundance, fractionmass;

//
// define Elements
//

G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
 G4Element* Si = new G4Element("Silicon",symbol="Si" , z= 14., a= 28.09*g/mole);
 G4Element* Na = new G4Element("Sodium", symbol="Na" , z= 11., a= 22.98977*g/mole);
 G4Element* I = new G4Element("Iodine",  symbol="I" , z= 53., a= 126.9045*g/mole);
 G4Element* Ca = new G4Element("Calcium",symbol="Ca",z=20, a= 40.08*g/mole);
 G4Element* Al = new G4Element("Aluminium",symbol="Al",z=13, a= 26.98154*g/mole);
 G4Element* B = new G4Element("Boron",   symbol="B",z=5, a=10.81*g/mole);
 G4Element* K = new G4Element("Potassium",symbol="K",z=19, a=39.098*g/mole);
 G4Element* Li = new G4Element("Lithium",symbol="Li",z=3, a= 6.941*g/mole);
 G4Element* Cl = new G4Element("Chlorine",symbol="Cl",z=17, a= 35.453*g/mole);
 G4Element* Ba = new G4Element("Barium",symbol="Ba",z=56, a= 137.327*g/mole);
 G4Element* F = new G4Element("Fluorine",symbol="F",z=9, a= 18.9984032*g/mole);
 G4Element* Ni = new G4Element("Nickel",symbol="Ni",z=28, a= 58.6934*g/mole);
 G4Element* Mo = new G4Element("Molybdenum",symbol="Mo",z=42, a= 95.94*g/mole);
 G4Element* Cu = new G4Element("Copper",symbol="Cu",z=29, a= 63.546*g/mole);
 G4Element* Fe = new G4Element("Iron",symbol="Fe",z=26, a= 55.845*g/mole);


 new G4Material("Aluminium", z=13., a=26.98154*g/mole, density=2.700*g/cm3);
 new G4Material("Iron", z=26., a=55.85*g/mole, density=7.87*g/cm3);
 new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
 new G4Material("Lead"     , z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
 new G4Material("Graphite"     , z=6., a= 12.01*g/mole, density= 2.267*g/cm3);//Note cbsim had rho=1.70?? 
 new G4Material("Copper"     , z=29, a= 63.546*g/mole, density= 8.96*g/cm3);
 //Targets
 new G4Material("LH2"     , z=1., a= 1.00794*g/mole, density= 0.0708*g/cm3);
 new G4Material("LD2"     , z=1., a= 2.014*g/mole, density= 0.162*g/cm3);
 new G4Material("Li7"     , z=3., a= 6.941*g/mole, density= 0.5340*g/cm3);
 new G4Material("Ca40"     , z=20., a= 40.08*g/mole, density= 1.55*g/cm3);//Note cbsim had 4??
 new G4Material("LD2"     , z=1., a= 2.014*g/mole, density= 0.162*g/cm3);

//
// define a material from elements.   case 1: chemical molecule
//

G4Material* H2O = 
new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
H2O->AddElement(H, natoms=2);
H2O->AddElement(O, natoms=1);
// overwrite computed meanExcitationEnergy with ICRU recommended value 
H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

G4Material* Sci = 
new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
Sci->AddElement(C, natoms=9);
Sci->AddElement(H, natoms=10);

G4Material* NaI=new G4Material("NaI", density= 3.67*g/cm3, ncomponents=2);
NaI->AddElement(Na, fractionmass=0.1533733);
NaI->AddElement(I, fractionmass=0.8466268);

G4Material* Myl = 
new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
Myl->AddElement(C, natoms=10);
Myl->AddElement(H, natoms= 8);
Myl->AddElement(O, natoms= 4);

G4Material* SiO2 = 
new G4Material("quartz",density= 2.200*g/cm3, ncomponents=2);
SiO2->AddElement(Si, natoms=1);
SiO2->AddElement(O , natoms=2);

//
// define a material from elements.   case 2: mixture by fractional mass
//

G4Material* Air = 
new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
Air->AddElement(N, fractionmass=0.7);
Air->AddElement(O, fractionmass=0.3);

 G4Material *plastic=new G4Material("plastic",density=1.19*g/cm3, ncomponents=2);
 plastic->AddElement(C,fractionmass=0.8562844);
 plastic->AddElement(H,fractionmass=0.1437155);

 G4Material *basemat=new G4Material("basemat",density=2.26*g/cm3, ncomponents=2);
 basemat->AddElement(C,fractionmass=0.8562844);
 basemat->AddElement(H,fractionmass=0.1437155);

 G4Material *fglass=new G4Material("fglass",density=2.0*g/cm3, ncomponents=8);
 fglass->AddElement(O,fractionmass=0.557);
 fglass->AddElement(Si,fractionmass=0.347);
 fglass->AddElement(Ca,fractionmass=0.3580000E-02);
 fglass->AddElement(Al,fractionmass=0.1590000E-02);
 fglass->AddElement(B,fractionmass=0.6840000E-01);
 fglass->AddElement(Na,fractionmass=0.7420000E-02);
 fglass->AddElement(K,fractionmass=0.1250000E-01);
 fglass->AddElement(Li,fractionmass=0.2320000E-02);

 G4Material* PVC=new G4Material("PVC",density=1.38*g/cm3,ncomponents=3);
 PVC->AddElement(C,fractionmass=0.3843611);
 PVC->AddElement(H,fractionmass=0.4838227E-01);
 PVC->AddElement(Cl,fractionmass=0.5672566);

 G4Material* BaF2=new G4Material("BaF2",density=4.89*g/cm3,ncomponents=2);
 BaF2->AddElement(Ba,fractionmass=0.7832764);
 BaF2->AddElement(F,fractionmass=0.2167236);
 G4Material* mumetal=new G4Material("mumetal",density=8.7*g/cm3,ncomponents=5);
 mumetal->AddElement(Ni,fractionmass=0.8);
 mumetal->AddElement(Mo,fractionmass=0.5000000E-01);
 mumetal->AddElement(Si,fractionmass=0.5000000E-02);
 mumetal->AddElement(Cu,fractionmass=0.2000000E-03);
 mumetal->AddElement(Fe,fractionmass=0.1448);
 G4Material* Kapton=new G4Material("Kapton",density=1.42000*g/cm3,ncomponents=4);
 Kapton->AddElement(C,0.6911325);
 Kapton->AddElement(H,0.2636296E-01);
 Kapton->AddElement(N,0.7327006E-01);
 Kapton->AddElement(O,0.2092344);
//
// define a material from elements and/or others materials (mixture of mixtures)
//

G4Material* Aerog = 
new G4Material("Aerogel", density= 0.200*g/cm3, ncomponents=3);
Aerog->AddMaterial(SiO2, fractionmass=62.5*perCent);
Aerog->AddMaterial(H2O , fractionmass=37.4*perCent);
Aerog->AddElement (C   , fractionmass= 0.1*perCent);

//
// examples of gas in non STP conditions
//

G4Material* CO2 = 
new G4Material("CarbonicGas", density= 27.*mg/cm3, ncomponents=2,
                              kStateGas, 325.*kelvin, 50.*atmosphere);
CO2->AddElement(C, natoms=1);
CO2->AddElement(O, natoms=2);
 
G4Material* steam = 
new G4Material("WaterSteam", density= 0.3*mg/cm3, ncomponents=1,
                             kStateGas, 500.*kelvin, 2.*atmosphere);
steam->AddMaterial(H2O, fractionmass=1.);

//
// examples of vacuum
//

G4Material* Vacuum =
new G4Material("Vacuum", z=1., a=1.01*g/mole,density= universe_mean_density,
                           kStateGas, 2.73*kelvin, 3.e-18*pascal);

G4Material* beam = 
new G4Material("Beam", density= 1.e-5*g/cm3, ncomponents=1,
                       kStateGas, STP_Temperature, 2.e-2*bar);
beam->AddMaterial(Air, fractionmass=1.);

G4cout << *(G4Material::GetMaterialTable()) << G4endl;
 G4cout<< G4Material::GetMaterial("Air")->GetName()<<G4endl;
//default materials of the World
fMaterialDefault  = Vacuum;
  */
}

