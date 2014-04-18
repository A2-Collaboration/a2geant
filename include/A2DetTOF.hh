
#ifndef A2DetTOF_h
#define A2DetTOF_h 1

#include "A2Detector.hh"
#include "A2SD.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

class A2DetTOF : public A2Detector
{
public:
  
  A2DetTOF();
  ~A2DetTOF();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);
  void ReadParameters(G4String parfile);

private:
  G4int fNWalls;  //Number of distinct TOF walls
  //Note the following paramters are kept in arrays with the number
  //of elements equal to the number of walls created
  G4int *fNBars;  //Number of scintillators in row
  G4int *fNLayers;  //Number of Layers
  G4double **fRotAng; //orientation of each layer (angle of rotation along z axis)
                  //note number of elements here =fNWalls*fNLayers 
  G4double *fBarX;  //X width of bar
  G4double *fBarY;  //Y width of bar
  G4double *fBarZ;  //Z width of bar

  G4double *fBarXoff;  //offset perpindicular to transverse direction
  G4double *fWallX; //X position of centre of wall in lab
  G4double *fWallY; //Y position of centre of wall in lab
  G4double *fWallZ; //Z position of centre of wall in lab

  //End of parmaters
  G4int fCounter; //To count the bars and assign index for AcquRoot::ReadDecoded() 
  G4int fTotBars; // total number of tof bars used

  A2SD* fTOFSD;

  G4Box *fBar;  //TOF scintillator shape
  G4LogicalVolume *fBarLogic;
  G4VPhysicalVolume* fBarPhysi;

public:
  G4int GetNToF(){return fTotBars;}
 
};

#endif
