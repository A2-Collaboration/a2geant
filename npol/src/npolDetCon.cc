
#include "npolDetCon.hh"
#include "npolSD.hh"

#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4NistManager.hh"

using namespace CLHEP;

npolDetectorConstruction::npolDetectorConstruction(G4double thick)
{
 
  fSD=NULL;
  fThick=thick*cm;
}



npolDetectorConstruction::~npolDetectorConstruction(){

}




G4VPhysicalVolume* npolDetectorConstruction::Construct()
{
  // materials
  DefineMaterials();
  

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
			  1*m,1*m,1*m);	//its size
                         
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

  //Hydrogen target
  G4Tubs* Scat=new G4Tubs("Scat",0.,3*cm,fThick/2*cm,0*deg,360*deg);
  G4LogicalVolume* ScatLogic=new G4LogicalVolume(Scat,G4NistManager::Instance()->FindOrBuildMaterial("G4_lH2"),"Scat");
 //plastic analyser
  // G4Box* Scat=new G4Box("Scat",10*cm,10*cm,fThick/2*cm);
  //G4LogicalVolume* ScatLogic=new G4LogicalVolume(Scat,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Scat");

  G4cout<<"Making box "<<fThick/cm<<G4endl;
  //G4LogicalVolume* ScatLogic=new G4LogicalVolume(Scat,G4NistManager::Instance()->FindOrBuildMaterial("LaBr3"),"Scat");
  // G4LogicalVolume* ScatLogic=new G4LogicalVolume(Scat,G4NistManager::Instance()->FindOrBuildMaterial("G4_C"),"Scat");
  //
 //G4PVPLacement(rotation,translation,logicvolume,name,mothervolume,false,copyno)
  G4VPhysicalVolume* ScatPhysi=new G4PVPlacement(0,G4ThreeVector(),ScatLogic,"Scat",fWorldLogic,false,1);
 


  // fWorldLogic->SetVisAttributes (G4VisAttributes::Invisible);

 G4SDManager* SDman = G4SDManager::GetSDMpointer();
   if(!fSD)
   {
    fSD = new npolSD("npolSD",this);
    SDman->AddNewDetector( fSD );
    }
     
   ScatLogic->SetSensitiveDetector(fSD);
  
  return fWorldPhysi;
}





#include "G4RunManager.hh"

void npolDetectorConstruction::UpdateGeometry()
{

  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}


void npolDetectorConstruction::DefineMaterials()
{ 
  G4NistManager* NistManager=G4NistManager::Instance();
  // G4double density,z,a,fractionmass;
//   G4int ncomponents;

//   //Add materials not contained in default Nist list
//   G4NistManager* NistManager=G4NistManager::Instance();
 G4double density,z,a,fractionmass;
  G4int ncomponents;
 G4Material *LaBr3=new G4Material("LaBr3",density=5.06*g/cm3, ncomponents=2);  
 LaBr3->AddElement(NistManager->FindOrBuildElement(57),1);
 LaBr3->AddElement(NistManager->FindOrBuildElement(35),3);

}

