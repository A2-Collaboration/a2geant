
#ifndef A2Target_h
#define A2Target_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class A2Target
{
public:
  
  A2Target();
  ~A2Target();
  
  //virtual function to build the target needs to be implemented in derived class
  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic)=0;//Build the target

  G4VPhysicalVolume* GetPhysi(){return fMyPhysi;};
  G4LogicalVolume* GetLogic(){return fMyLogic;}
  G4LogicalVolume* GetMotherLogic(){return fMotherLogic;}
 
  void SetIsInteractive(G4int is){fIsInteractive=is;}
  void SetMaterial(G4Material* mat){fMaterial=mat;}

  G4ThreeVector GetCenter(){return fCenter;}
  G4double GetLength(){return fLength;}
  G4double GetRadius(){return fRadius;}

protected:
  G4int fVerbose;                       //verbose level
  G4int fIsInteractive;    // batch(0) or interactive(1) mode

  G4LogicalVolume* fMotherLogic;        //Logical volume of the mother

  G4LogicalVolume* fMyLogic;            //Logical Volume for this detector
  G4VPhysicalVolume* fMyPhysi;              //Physical volume for this detector
  G4Material* fMaterial;         //target material

  G4NistManager* fNistManager;

  G4ThreeVector fCenter;   //centre of active target region
  G4double fLength;        //Length of active target region
  G4double fRadius;        //Radius of active target region
};

#endif
