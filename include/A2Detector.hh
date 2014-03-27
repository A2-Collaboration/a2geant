/*Base class for A2 detectors
Contains general functions for visulisation...
*/

#ifndef A2Detector_h
#define A2Detector_h 1
#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class A2Detector
{
public:
  
  A2Detector();
  ~A2Detector();
  
  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic)=0; //Build the detector

  G4VPhysicalVolume* GetPhysi(){return fMyPhysi;};
  G4LogicalVolume* GetLogic(){return fMyLogic;}
  G4LogicalVolume* GetMotherLogic(){return fMotherLogic;}
 
  void SetIsInteractive(G4int is){fIsInteractive=is;}

protected:
  G4int fVerbose;                       //verbose level
  G4int fIsInteractive;    // batch(0) or interactive(1) mode

  G4LogicalVolume* fMotherLogic;        //Logical volume of the mother

  G4LogicalVolume* fMyLogic;            //Logical Volume for this detector
  G4VPhysicalVolume* fMyPhysi;              //Physical volume for this detector

  G4NistManager* fNistManager;
};

#endif
