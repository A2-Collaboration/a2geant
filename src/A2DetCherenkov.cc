#include "A2DetCherenkov.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

A2DetCherenkov::A2DetCherenkov()
{

}
A2DetCherenkov::~A2DetCherenkov()
{

}

G4VPhysicalVolume* A2DetCherenkov::Construct(G4LogicalVolume* MotherLogical){
fMotherLogic=MotherLogical;

 G4Box* MyBox=new G4Box("Box", 60.0*cm, 60.0*cm, 36.0*cm);
 fMyLogic=new G4LogicalVolume(MyBox, fNistManager->FindOrBuildMaterial("G4_AIR"), "Box");
 fMyPhysi=new G4PVPlacement(0, G4ThreeVector(-12.0*cm,0.0*cm,133*cm), fMyLogic, "MyBox", fMotherLogic, false, 1);
 fMyLogic->SetVisAttributes(G4Colour(1,0,8));

return fMyPhysi;
}
