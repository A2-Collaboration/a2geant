
#ifndef npolDetCon_h
#define npolDetCon_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4NistManager.hh"
//#include "G4String.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class npolDetectorMessenger;
class npolSD;


class npolDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
  npolDetectorConstruction(){};
   npolDetectorConstruction(G4double thick);
   ~npolDetectorConstruction();

  public:
   
     G4VPhysicalVolume* Construct();

     void UpdateGeometry();
     void DefineMaterials();

public:
  
     
     const G4VPhysicalVolume* GetWorld() {return fWorldPhysi;};           
                 
   
     G4Box*             fWorldSolid;    //pointer to the solid World 
     G4LogicalVolume*   fWorldLogic;    //pointer to the logical World
     G4VPhysicalVolume* fWorldPhysi;    //pointer to the physical World

  npolSD* fSD;
private:
  G4double fThick;
   
};


#endif

