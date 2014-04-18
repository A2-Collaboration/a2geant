#include "A2PolarizedTarget.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "A2MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

A2PolarizedTarget::A2PolarizedTarget()
{
  fMagneticField = NULL;
}
A2PolarizedTarget::~A2PolarizedTarget()
{
  if(fMagneticField) delete fMagneticField;
}

void A2PolarizedTarget::SetMagneticField(G4String &nameFileFieldMap)
{
  // If nameFileFieldMap is a NULL string then do not set the target magnetic field
  if(nameFileFieldMap.isNull()) {G4cout<<"Warning A2PolarizedTarget::SetMagneticField No field map given, therefore there will be no field!"<<G4endl;return;}
  
  // Create magnetic field
  fMagneticField = new A2MagneticField();
  
  // Read magnetic field map
  // Set this field as default and create trajectory calculator
  // Or, in case of a problem reading the field map, delete fMagneticField
  if(fMagneticField->ReadFieldMap(nameFileFieldMap))
  {
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(fMagneticField);
    fieldMgr->CreateChordFinder(fMagneticField);
  }
  else
  {
    delete fMagneticField;
  }
}

G4VPhysicalVolume* A2PolarizedTarget::Construct(G4LogicalVolume *MotherLogic)
{

//   //Magnetic field moved from A2DetectorConstruction dglazier
//   static G4bool fieldIsInitialized = false;
//   if(!fieldIsInitialized)
//   {
//     fMagneticField = new A2MagneticField();
//     G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
//     fieldMgr->SetDetectorField(fMagneticField);
//     fieldMgr->CreateChordFinder(fMagneticField);
//     fieldIsInitialized = true;
//   }

 fMotherLogic=MotherLogic;
 fLength=20.0*CLHEP::mm;
 fRadius=0.5*CLHEP::mm;

 //Note: 
 //l indicates length,
 //r indicates radius, which is always measured to outside of cylinder,
 //t indicates thickness. 

 //Also note: there are 2 approximations in the target geometries. When the tubes change thickness, there is no slope joining the different thicknesses. Also, when the ends of the tubes are rounded, this is approximated as a straight 90*degree cylinder.
 
 G4double l_TRGT = 276.*CLHEP::mm + 67.*CLHEP::mm + 2*CLHEP::mm;
 G4double r_TRGT = 47.97*CLHEP::mm;
  
 //Mother volume, a cylinder with at least a 1-mm space between it and the target in all directions:
 G4Tubs* MyShape=new G4Tubs("TRGT",0.,r_TRGT + 1.0*CLHEP::mm, l_TRGT/2 +1.0*CLHEP::mm,0*CLHEP::deg,360*CLHEP::deg);
 fMyLogic=new G4LogicalVolume(MyShape,fNistManager->FindOrBuildMaterial("G4_AIR"),"TRGT");
 fMyPhysi=new G4PVPlacement(0,G4ThreeVector(0,0, - 20.0*CLHEP::mm/2 - 11.5*CLHEP::mm - 231.5*CLHEP::mm + l_TRGT/2.),fMyLogic,"TRGT",fMotherLogic,false,1);
 fMyLogic->SetVisAttributes (G4VisAttributes::Invisible);

 //Colours with thier corresponding materials used in the visualization:
 G4VisAttributes* SSVisAtt= new G4VisAttributes(G4Colour(0.8,0.8,0.8)); //stainless steel (grey)
 G4VisAttributes* CUVisAtt= new G4VisAttributes(G4Colour(0.8,0.6,0.2)); //copper (brown)
 G4VisAttributes* CyanVisAtt= new G4VisAttributes(G4Colour(.5,.2,0.0)); //NbTi
 G4VisAttributes* MagentaVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,1.0)); //epoxy
 G4VisAttributes* BlueVisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0)); //titanium
 G4VisAttributes* GreenVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0)); //aluminum
 G4VisAttributes* RedVisAtt= new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); //kapton
 G4VisAttributes* WhiteVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0)); //helium

 //Cylinders which make up the target, starting from the outside and working in.
 //Outer stainless steel cylinder:
 G4double l_SSO = 233.5*CLHEP::mm;
 G4double r_SSO = 32.5*CLHEP::mm;
 G4double t_SSO = 0.5*CLHEP::mm;
 G4Tubs* SSO=new G4Tubs("SSO",r_SSO-t_SSO,r_SSO,l_SSO/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* SSOLogic=new G4LogicalVolume(SSO,fNistManager->FindOrBuildMaterial("A2_SS"),"SSO");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_SSO/2 + 42.5*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),SSOLogic,"SSO",fMyLogic,false,1);
 SSOLogic->SetVisAttributes(SSVisAtt);
//  SSOLogic->SetVisAttributes(G4VisAttributes::Invisible);
 
 //Outer copper cylinder:
 G4double l_CUO = 222.5*CLHEP::mm;
 G4double r_CUO = 29.0*CLHEP::mm;
 G4double t_CUO = 0.5*CLHEP::mm;
 G4Tubs* CUO=new G4Tubs("CUO",r_CUO-t_CUO,r_CUO,l_CUO/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUOLogic=new G4LogicalVolume(CUO,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUO");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUO/2 + 42.5*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUOLogic,"CUO",fMyLogic,false,1);
 CUOLogic->SetVisAttributes(CUVisAtt);
//  CUOLogic->SetVisAttributes(G4VisAttributes::Invisible);

 if ( fTypeMagneticCoils == G4String("Solenoidal") || fTypeMagneticCoils == G4String("solenoidal") ) // The solenoidal coils
 {
   //The coils are approximated to be 3 individual layers of material, 460 microm NiTi, 340 microm Cu, 200 microm epoxy.
   //NbTi coil layer:
   G4double l_NbTiC = 136.0*CLHEP::mm;
   G4double r_NbTiC = 24.6*CLHEP::mm;
   G4double t_NbTiC = 0.460*CLHEP::mm;
   G4Tubs* NbTiC=new G4Tubs("NbTiC",r_NbTiC-t_NbTiC,r_NbTiC,l_NbTiC/2,0*CLHEP::deg,360*CLHEP::deg);
   G4LogicalVolume* NbTiCLogic=new G4LogicalVolume(NbTiC,fNistManager->FindOrBuildMaterial("A2_NbTi"),"NbTiC");
   new G4PVPlacement(0,G4ThreeVector(0,0,(l_NbTiC/2 + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),NbTiCLogic,"NbTiC",fMyLogic,false,1);
   NbTiCLogic->SetVisAttributes(CyanVisAtt);
//    NbTiCLogic->SetVisAttributes(G4VisAttributes::Invisible);
   
   //Copper coil layer:
   G4double l_CUC = 136.0*CLHEP::mm;
   G4double r_CUC = 24.14*CLHEP::mm;
   G4double t_CUC = 0.340*CLHEP::mm;
   G4Tubs* CUC=new G4Tubs("CUC",r_CUC-t_CUC,r_CUC,l_CUC/2,0*CLHEP::deg,360*CLHEP::deg);
   G4LogicalVolume* CUCLogic=new G4LogicalVolume(CUC,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUC");
   new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUC/2 + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUCLogic,"CUC",fMyLogic,false,1);
   CUCLogic->SetVisAttributes(CUVisAtt);
//    CUCLogic->SetVisAttributes(G4VisAttributes::Invisible);
   
   //Epoxy coil layer:
   G4double l_EPC = 136.0*CLHEP::mm;
   G4double r_EPC = 23.8*CLHEP::mm;
   G4double t_EPC = 0.200*CLHEP::mm;
   G4Tubs* EPC=new G4Tubs("EPC",r_EPC-t_EPC,r_EPC,l_EPC/2,0*CLHEP::deg,360*CLHEP::deg);
   G4LogicalVolume* EPCLogic=new G4LogicalVolume(EPC,fNistManager->FindOrBuildMaterial("A2_Epoxy"),"EPC");
   new G4PVPlacement(0,G4ThreeVector(0,0,(l_EPC/2 + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),EPCLogic,"EPC",fMyLogic,false,1);
   EPCLogic->SetVisAttributes(MagentaVisAtt);
//    EPCLogic->SetVisAttributes(G4VisAttributes::Invisible);
 }
 else if ( fTypeMagneticCoils == G4String("Saddle") || fTypeMagneticCoils == G4String("saddle") ) // The saddle coils
 {
   // Box-splitter
   G4double xBoxSplitter = 25*CLHEP::mm;
   G4double yBoxSplitter = 0.1*CLHEP::mm;
   G4double zBoxSplitter = 136*CLHEP::mm;
   G4Box *boxSplitter = new G4Box("boxSplitter", xBoxSplitter, yBoxSplitter, zBoxSplitter);
   
   // Layer1
   G4double lTube1 = 135.48*CLHEP::mm;
   G4double rTube1 =  24.46*CLHEP::mm;
   G4double tTube1 =   0.46*CLHEP::mm;
   G4Tubs *tube1 = new G4Tubs("tube1", rTube1 - tTube1, rTube1, lTube1/2, 0*CLHEP::deg, 360*CLHEP::deg);
   G4double xBox1 = 6*CLHEP::mm;
   G4double yBox1 = 50*CLHEP::mm;
   G4double zBox1 = 36*CLHEP::mm;
   G4Box *box1 = new G4Box("box1", xBox1, yBox1, zBox1);
   G4SubtractionSolid *tube_box1 = new G4SubtractionSolid("tube1-box1", tube1, box1);
   G4SubtractionSolid *layer1 = new G4SubtractionSolid("layer1", tube_box1, boxSplitter);
   G4LogicalVolume* logicSaddleCoilsLayer1 = new G4LogicalVolume(layer1, fNistManager->FindOrBuildMaterial("A2_NbTi"), "logicSaddleCoilsLayer1");
   new G4PVPlacement(0, G4ThreeVector(0.,0.,(lTube1/2. + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)), logicSaddleCoilsLayer1, "physSaddleCoilsLayer1", fMyLogic, false, 1);
   logicSaddleCoilsLayer1->SetVisAttributes(CyanVisAtt);
   //  logicSaddleCoilsLayer1->SetVisAttributes(G4VisAttributes::Invisible);
   
   // Layer2
   G4double lTube2 = 135.88*CLHEP::mm;
   G4double rTube2 =  24.92*CLHEP::mm;
   G4double tTube2 =   0.46*CLHEP::mm;
   G4Tubs *tube2 = new G4Tubs("tube2", rTube2 - tTube2, rTube2, lTube2/2, 0*CLHEP::deg, 360*CLHEP::deg);
   G4double xBox2 = 20*CLHEP::mm;
   G4double yBox2 = 50*CLHEP::mm;
   G4double zBox2 = 50*CLHEP::mm;
   G4Box *box2 = new G4Box("box2", xBox2, yBox2, zBox2);
   G4SubtractionSolid *tube_box2 = new G4SubtractionSolid("tube2-box2", tube2, box2);
   G4SubtractionSolid *layer2 = new G4SubtractionSolid("layer2", tube_box2, boxSplitter);
   G4LogicalVolume* logicSaddleCoilsLayer2 = new G4LogicalVolume(layer2, fNistManager->FindOrBuildMaterial("A2_NbTi"), "logicSaddleCoilsLayer2");
   new G4PVPlacement(0, G4ThreeVector(0.,0.,(lTube2/2 + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)), logicSaddleCoilsLayer2, "physSaddleCoilsLayer2", fMyLogic, false, 1);
   logicSaddleCoilsLayer2->SetVisAttributes(CyanVisAtt);
   //  logicSaddleCoilsLayer2->SetVisAttributes(G4VisAttributes::Invisible);
 }
 
 //The coils are wrapped around part of a middle copper cylinder, which has 3 different thicknesses, 0.3, 0.7, and 1.2, labelled A, B, and C respectively, going up-beam.
 //Middle copper cylinder, part A, which the coils are wrapped around:
 G4double l_CUMA = 136.0*CLHEP::mm;
 G4double r_CUMA = 23.6*CLHEP::mm;
 G4double t_CUMA = 0.3*CLHEP::mm;
 G4Tubs* CUMA=new G4Tubs("CUMA",r_CUMA-t_CUMA,r_CUMA,l_CUMA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUMALogic=new G4LogicalVolume(CUMA,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUMA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUMA/2 + 119*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUMALogic,"CUMA",fMyLogic,false,1);
 CUMALogic->SetVisAttributes(CUVisAtt);
//  CUMALogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Middle copper cylinder, part B:
 G4double l_CUMB = 33.5*CLHEP::mm;
 G4double r_CUMB = 24.0*CLHEP::mm;
 G4double t_CUMB = 0.7*CLHEP::mm;
 G4Tubs* CUMB=new G4Tubs("CUMB",r_CUMB-t_CUMB,r_CUMB,l_CUMB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUMBLogic=new G4LogicalVolume(CUMB,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUMB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUMB/2 + 85.5*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUMBLogic,"CUMB",fMyLogic,false,1);
 CUMBLogic->SetVisAttributes(CUVisAtt);
//  CUMBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Middle copper cylinder, part C:
 G4double l_CUMC = 57.7*CLHEP::mm;
 G4double r_CUMC = 24.5*CLHEP::mm;
 G4double t_CUMC = 1.2*CLHEP::mm;
 G4Tubs* CUMC=new G4Tubs("CUMC",r_CUMC-t_CUMC,r_CUMC,l_CUMC/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUMCLogic=new G4LogicalVolume(CUMC,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUMC");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUMC/2 + 27.8*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUMCLogic,"CUIC",fMyLogic,false,1);
 CUMCLogic->SetVisAttributes(CUVisAtt);
//  CUMCLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner stainless steel cylinder, with 2 thicknesses, going up-beam:
 //Inner stainless steel cylinder, part A:
 G4double l_SSIA = 122.0*CLHEP::mm;
 G4double r_SSIA = 19.8*CLHEP::mm;
 G4double t_SSIA = 0.3*CLHEP::mm;
 G4Tubs* SSIA=new G4Tubs("SSIA",r_SSIA-t_SSIA,r_SSIA,l_SSIA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* SSIALogic=new G4LogicalVolume(SSIA,fNistManager->FindOrBuildMaterial("A2_SS"),"SSIA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_SSIA/2 + 78*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),SSIALogic,"SSIA",fMyLogic,false,1);
 SSIALogic->SetVisAttributes(SSVisAtt);
//  SSIALogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner stainless steel cylinder, part B:
 G4double l_SSIB = 56.75*CLHEP::mm;
 G4double r_SSIB = 20.0*CLHEP::mm;
 G4double t_SSIB = 0.5*CLHEP::mm;
 G4Tubs* SSIB=new G4Tubs("SSIB",r_SSIB-t_SSIB,r_SSIB,l_SSIB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* SSIBLogic=new G4LogicalVolume(SSIB,fNistManager->FindOrBuildMaterial("A2_SS"),"SSIB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_SSIB/2 + 21.25*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),SSIBLogic,"SSIB",fMyLogic,false,1);
 SSIBLogic->SetVisAttributes(SSVisAtt);
//  SSIBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner copper cylinder, part A:
 G4double l_CUIA = 10.4*CLHEP::mm;
 G4double r_CUIA = 11.5*CLHEP::mm;
 G4double t_CUIA = 1.5*CLHEP::mm;
 G4Tubs* CUIA=new G4Tubs("CUIA",r_CUIA-t_CUIA,r_CUIA,l_CUIA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUIALogic=new G4LogicalVolume(CUIA,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUIA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm + 1.0*CLHEP::mm + l_CUIA/2 - l_TRGT/2.)),CUIALogic,"CUIA",fMyLogic,false,1);
 CUIALogic->SetVisAttributes(CUVisAtt);
//  CUIALogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner copper cylinder, part B:
 G4double l_CUIB = 1.0*CLHEP::mm;
 G4double r_CUIB = 12.2*CLHEP::mm;
 G4double t_CUIB = 2.2*CLHEP::mm;
 G4Tubs* CUIB=new G4Tubs("CUIB",r_CUIB-t_CUIB,r_CUIB,l_CUIB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUIBLogic=new G4LogicalVolume(CUIB,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUIB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm + l_CUIB/2 - l_TRGT/2.)),CUIBLogic,"CUIB",fMyLogic,false,1);
 CUIBLogic->SetVisAttributes(CUVisAtt);
//  CUIBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner copper cylinder, part C:
 G4double l_CUIC = 9.0*CLHEP::mm;
 G4double r_CUIC = 12.2*CLHEP::mm;
 G4double t_CUIC = 0.5*CLHEP::mm;
 G4Tubs* CUIC=new G4Tubs("CUIC",r_CUIC-t_CUIC,r_CUIC,l_CUIC/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUICLogic=new G4LogicalVolume(CUIC,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUIC");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm - l_CUIC/2 - l_TRGT/2.)),CUICLogic,"CUIC",fMyLogic,false,1);
 CUICLogic->SetVisAttributes(CUVisAtt);
//  CUICLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner copper cylinder, part D:
 G4double l_CUID = 215.*CLHEP::mm - 67*CLHEP::mm;
 G4double r_CUID = 12.5*CLHEP::mm;
 G4double t_CUID = 0.3*CLHEP::mm;
 G4Tubs* CUID=new G4Tubs("CUID",r_CUID-t_CUID,r_CUID,l_CUID/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUIDLogic=new G4LogicalVolume(CUID,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUID");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUID/2 + 67*CLHEP::mm + 16*CLHEP::mm - l_TRGT/2.)),CUIDLogic,"CUID",fMyLogic,false,1);
 CUIDLogic->SetVisAttributes(CUVisAtt);
//  CUIDLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //The butenol target is encased in kapton. This cup has holes in it, which are ingnored. 
 //Kapton outside cell, part A:
 G4double l_KAPA = 0.6*CLHEP::mm;
 G4double r_KAPA = 10.505*CLHEP::mm;
 G4Tubs* KAPA=new G4Tubs("KAPA",0,r_KAPA,l_KAPA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* KAPALogic=new G4LogicalVolume(KAPA,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_KAPA/2 + 20.0*CLHEP::mm + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),KAPALogic,"KAPA",fMyLogic,false,1);
 KAPALogic->SetVisAttributes(RedVisAtt);
//  KAPALogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Kapton outside cell, part B:
 G4double l_KAPB = 19.0*CLHEP::mm;
 G4double r_KAPB = 10.505*CLHEP::mm;
 G4double t_KAPB = 0.6*CLHEP::mm;
 G4Tubs* KAPB=new G4Tubs("KAPB",r_KAPB-t_KAPB,r_KAPB,l_KAPB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* KAPBLogic=new G4LogicalVolume(KAPB,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_KAPB/2 + 1.0*CLHEP::mm + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),KAPBLogic,"KAPB",fMyLogic,false,1);
 KAPBLogic->SetVisAttributes(RedVisAtt);
//  KAPBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Kapton outside cell, part C:
 G4double l_KAPC = 1.0*CLHEP::mm;
 G4double r_KAPC = 12.43*CLHEP::mm;
 G4double t_KAPC = 2.5*CLHEP::mm;
 G4Tubs* KAPC=new G4Tubs("KAPC",r_KAPC-t_KAPC,r_KAPC,l_KAPC/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* KAPCLogic=new G4LogicalVolume(KAPC,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPC");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_KAPC/2 + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),KAPCLogic,"KAPC",fMyLogic,false,1);
 KAPCLogic->SetVisAttributes(RedVisAtt);
//  KAPCLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Kapton outside cell, part D:
 G4double l_KAPD = 10.0*CLHEP::mm;
 G4double r_KAPD = 12.43*CLHEP::mm;
 G4double t_KAPD = 0.93*CLHEP::mm;
 G4Tubs* KAPD=new G4Tubs("KAPD",r_KAPD-t_KAPD,r_KAPD,l_KAPD/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* KAPDLogic=new G4LogicalVolume(KAPD,fNistManager->FindOrBuildMaterial("G4_KAPTON"),"KAPD");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_KAPD/2 + 1.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),KAPDLogic,"KAPD",fMyLogic,false,1);
 KAPDLogic->SetVisAttributes(RedVisAtt);
//  KAPDLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Target cell, butanol, 60% filling:
 G4double l_BTRGT = 20.0*CLHEP::mm;
 G4double r_BTRGT = 9.905*CLHEP::mm;
 G4Tubs* BTRGT=new G4Tubs("BTRGT",0,r_BTRGT,l_BTRGT/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* BTRGTLogic=new G4LogicalVolume(BTRGT,fNistManager->FindOrBuildMaterial("A2_HeButanol"),"BTRGT");
 //G4LogicalVolume* BTRGTLogic=new G4LogicalVolume(BTRGT,fNistManager->FindOrBuildMaterial("A2_HeMix"),"BTRGT");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_BTRGT/2 + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),BTRGTLogic,"BTRGT",fMyLogic,false,1);
 BTRGTLogic->SetVisAttributes(MagentaVisAtt);
//  BTRGTLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //He between inner SS and Cu cylinders, part A:
 G4double l_HEA = 3.1*CLHEP::mm;
 G4double r_HEA = 19.5*CLHEP::mm;
 G4Tubs* HEA=new G4Tubs("HEA",0,r_HEA,l_HEA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HEALogic=new G4LogicalVolume(HEA,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HEA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(67*CLHEP::mm + 200*CLHEP::mm - 0.3*CLHEP::mm - l_HEA/2 - l_TRGT/2.)),HEALogic,"HEA",fMyLogic,false,1);
 HEALogic->SetVisAttributes(WhiteVisAtt);
//  HEALogic->SetVisAttributes(G4VisAttributes::Invisible);

 //He, part B:
 G4double l_HEB = 19.575*CLHEP::mm;
 G4double r_HEB = 19.5*CLHEP::mm;
 G4double t_HEB = 8.995*CLHEP::mm;
 G4Tubs* HEB=new G4Tubs("HEB",r_HEB-t_HEB,r_HEB,l_HEB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HEBLogic=new G4LogicalVolume(HEB,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HEB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_HEB/2 + 1*CLHEP::mm + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),HEBLogic,"HEB",fMyLogic,false,1);
 HEBLogic->SetVisAttributes(WhiteVisAtt);
//  HEBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //He, part C:
 G4double l_HEC = 11.0*CLHEP::mm;
 G4double r_HEC = 19.5*CLHEP::mm;
 G4double t_HEC = 7.07*CLHEP::mm;
 G4Tubs* HEC=new G4Tubs("HEC",r_HEC-t_HEC,r_HEC,l_HEC/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HECLogic=new G4LogicalVolume(HEC,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HEC");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm + 1.5*CLHEP::mm + l_HEC/2 - l_TRGT/2.)),HECLogic,"HEC",fMyLogic,false,1);
 HECLogic->SetVisAttributes(WhiteVisAtt);
//  HECLogic->SetVisAttributes(G4VisAttributes::Invisible);
 
 //He, part D:
 G4double l_HED = 0.5*CLHEP::mm;
 G4double r_HED = 19.5*CLHEP::mm;
 G4double t_HED = 8.0*CLHEP::mm;
 G4Tubs* HED=new G4Tubs("HED",r_HED-t_HED,r_HED,l_HED/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HEDLogic=new G4LogicalVolume(HED,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HED");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm + 1*CLHEP::mm + l_HED/2 - l_TRGT/2.)),HEDLogic,"HED",fMyLogic,false,1);
 HEDLogic->SetVisAttributes(WhiteVisAtt);
//  HEDLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //He, part E:
 G4double l_HEE = 1.0*CLHEP::mm;
 G4double r_HEE = 19.5*CLHEP::mm;
 G4double t_HEE = 7.3*CLHEP::mm;
 G4Tubs* HEE=new G4Tubs("HEE",r_HEE-t_HEE,r_HEE,l_HEE/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HEELogic=new G4LogicalVolume(HEE,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HEE");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm + l_HEE/2 - l_TRGT/2.)),HEELogic,"HEE",fMyLogic,false,1);
 HEELogic->SetVisAttributes(WhiteVisAtt);
//  HEELogic->SetVisAttributes(G4VisAttributes::Invisible);

 //He, part F:
 G4double l_HEF = 122.0*CLHEP::mm;
 G4double r_HEF = 19.5*CLHEP::mm;
 G4double t_HEF = 7.0*CLHEP::mm; 
 G4Tubs* HEF=new G4Tubs("HEF",r_HEF-t_HEF,r_HEF,l_HEF/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* HEFLogic=new G4LogicalVolume(HEF,fNistManager->FindOrBuildMaterial("A2_HeMix"),"HEF");
 new G4PVPlacement(0,G4ThreeVector(0,0,(231.5*CLHEP::mm - l_HEF/2 - l_TRGT/2.)),HEFLogic,"HEF",fMyLogic,false,1);
 HEFLogic->SetVisAttributes(WhiteVisAtt);
//  HEFLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Windows (from outside, going up-beam):
 //The cylinders with the Ti windows attached were rounded on the down-beam end. This makes the approximation of 90*CLHEP::deg corners. 
 //Outer Ti window:
 G4double r_TIOW = 12.5*CLHEP::mm;
 G4double t_TIOW = 0.02*CLHEP::mm;
 G4Tubs* TIOW=new G4Tubs("TIOW",0,r_TIOW,t_TIOW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* TIOWLogic=new G4LogicalVolume(TIOW,fNistManager->FindOrBuildMaterial("G4_Ti"),"TIOW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(t_TIOW/2 + 276*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),TIOWLogic,"TIOW",fMyLogic,false,1);
 TIOWLogic->SetVisAttributes(BlueVisAtt);
//  TIOWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Copper ring around outer Ti window:
 G4double r_CUOW = 32.5*CLHEP::mm;
 G4double t_CUOW = 0.40*CLHEP::mm;
 G4Tubs* CUOW=new G4Tubs("CUOW",r_TIOW,r_CUOW,t_CUOW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUOWLogic=new G4LogicalVolume(CUOW,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUOW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(t_CUOW/2 + 276*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUOWLogic,"CUOW",fMyLogic,false,1);
 CUOWLogic->SetVisAttributes(CUVisAtt);
//  CUOWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Outer Al window:
 G4double r_ALOW = 29.0*CLHEP::mm;
 G4double t_ALOW = 0.01*CLHEP::mm;
 G4Tubs* ALOW=new G4Tubs("ALOW",0,r_ALOW,t_ALOW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* ALOWLogic=new G4LogicalVolume(ALOW,fNistManager->FindOrBuildMaterial("G4_Al"),"ALOW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(t_ALOW/2 + 265*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),ALOWLogic,"ALOW",fMyLogic,false,1);
 ALOWLogic->SetVisAttributes(GreenVisAtt);
//  ALOWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner Al window:
 G4double r_ALIW = 23.7*CLHEP::mm;
 G4double t_ALIW = 0.01*CLHEP::mm;
 G4Tubs* ALIW=new G4Tubs("ALIW",0,r_ALIW,t_ALIW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* ALIWLogic=new G4LogicalVolume(ALIW,fNistManager->FindOrBuildMaterial("G4_Al"),"ALIW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(t_ALIW/2 + 256.5*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),ALIWLogic,"ALIW",fMyLogic,false,1);
 ALIWLogic->SetVisAttributes(GreenVisAtt);
//  ALIWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Small copper bit between coils and inner Al window:
 //Part A:
 G4double l_CUBA = 0.5*CLHEP::mm;
 G4double r_CUBA = 23.7*CLHEP::mm;
 G4double t_CUBA = 1.2*CLHEP::mm;
 G4Tubs* CUBA=new G4Tubs("CUBA",r_CUBA-t_CUBA,r_CUBA,l_CUBA/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUBALogic=new G4LogicalVolume(CUBA,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUBA");
 new G4PVPlacement(0,G4ThreeVector(0,0,(l_CUBA/2 + 256.5*CLHEP::mm + 67*CLHEP::mm - l_TRGT/2.)),CUBALogic,"CUBA",fMyLogic,false,1);
 CUBALogic->SetVisAttributes(CUVisAtt);
//  CUBALogic->SetVisAttributes(G4VisAttributes::Invisible);
 
 //Part B:
  G4double l_CUBB = 3.18*CLHEP::mm;
 G4double r_CUBB = 23.0*CLHEP::mm;
 G4double t_CUBB = 0.5*CLHEP::mm;
 G4Tubs* CUBB=new G4Tubs("CUBB",r_CUBB-t_CUBB,r_CUBB,l_CUBB/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* CUBBLogic=new G4LogicalVolume(CUBB,fNistManager->FindOrBuildMaterial("G4_Cu"),"CUBB");
 new G4PVPlacement(0,G4ThreeVector(0,0,(256.5*CLHEP::mm + 67*CLHEP::mm - l_CUBB/2 - l_TRGT/2.)),CUBBLogic,"CUBB",fMyLogic,false,1);
 CUBBLogic->SetVisAttributes(CUVisAtt);
//  CUBBLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Middle Ti window:
 G4double r_TIMW = 11.0*CLHEP::mm;
 G4double t_TIMW = 0.02*CLHEP::mm;
 G4Tubs* TIMW=new G4Tubs("TIMW",0,r_TIMW,t_TIMW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* TIMWLogic=new G4LogicalVolume(TIMW,fNistManager->FindOrBuildMaterial("G4_Ti"),"TIMW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(-t_TIMW/2 + 267*CLHEP::mm - l_TRGT/2.)),TIMWLogic,"TIMW",fMyLogic,false,1);
 TIMWLogic->SetVisAttributes(BlueVisAtt);
//  TIMWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Stainless steel ring around middle Ti window:
 G4double r_SSIW = 19.8*CLHEP::mm;
 G4double t_SSIW = 0.30*CLHEP::mm;
 G4Tubs* SSIW=new G4Tubs("SSIW",r_TIMW,r_SSIW,t_SSIW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* SSIWLogic=new G4LogicalVolume(SSIW,fNistManager->FindOrBuildMaterial("A2_SS"),"SSIW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(-t_SSIW/2 + 267*CLHEP::mm - l_TRGT/2.)),SSIWLogic,"SSIW",fMyLogic,false,1);
 SSIWLogic->SetVisAttributes(SSVisAtt);
//  SSIWLogic->SetVisAttributes(G4VisAttributes::Invisible);

 //Inner Ti window:
 G4double r_TIIW = 10.0*CLHEP::mm;
 G4double t_TIIW = 0.02*CLHEP::mm;
 G4Tubs* TIIW=new G4Tubs("TIIW",0,r_TIIW,t_TIIW/2,0*CLHEP::deg,360*CLHEP::deg);
 G4LogicalVolume* TIIWLogic=new G4LogicalVolume(TIIW,fNistManager->FindOrBuildMaterial("G4_Ti"),"TIIW");
 new G4PVPlacement(0,G4ThreeVector(0,0,(t_TIIW/2 + 11.5*CLHEP::mm + 231.5*CLHEP::mm - l_TRGT/2.)),TIIWLogic,"TIIW",fMyLogic,false,1);
 TIIWLogic->SetVisAttributes(BlueVisAtt);
//  TIIWLogic->SetVisAttributes(G4VisAttributes::Invisible);



 return fMyPhysi;
}
