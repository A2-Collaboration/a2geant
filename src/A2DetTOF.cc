// general ToF detector class
//requires a parameter file giving number of walls, dimensions etc.

#include "A2DetTOF.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include <fstream>
//using namespace std;

A2DetTOF::A2DetTOF(){
  
  fNBars=0;
  fNLayers=0;
  fCounter=0;
  fTOFSD=NULL;
}

A2DetTOF::~A2DetTOF(){
  delete fNBars;
  delete fNLayers;
  delete fBarX;
  delete fBarY;
  delete fBarZ;
  delete fBarXoff;
  delete fWallX;
  delete fWallY;
  delete fWallZ;
  delete fRotAng;
}
void A2DetTOF::ReadParameters(G4String parfile){
  std::fstream pars;
  pars.open(parfile.data(),std::ios::in);

  pars>>fNWalls;//Get number of walls
  //Now create arrrays to contain params for each wall
  fNBars=new G4int[fNWalls];
  fNLayers=new G4int[fNWalls];
  fBarX=new G4double[fNWalls];
  fBarY=new G4double[fNWalls];
  fBarZ=new G4double[fNWalls];
  fBarXoff=new G4double[fNWalls];
  fWallX=new G4double[fNWalls];
  fWallY=new G4double[fNWalls];
  fWallZ=new G4double[fNWalls];
  fRotAng=new G4double*[fNWalls];
  fTotBars=0;
  //Now loop over parameter for each wall
  for(G4int w=0;w<fNWalls;w++){
    // G4cout<<"WALL "<<w<<G4endl;
    pars>>fNBars[w];
    //G4cout<<"NUMBER OF BARS "<<fNBars[w]<<G4endl;
    pars>>fNLayers[w];
    fRotAng[w]=new G4double[fNLayers[w]];
    for(G4int i=0;i<fNLayers[w];i++) pars>>fRotAng[w][i];
    //G4cout<<"NUMBER OF Layer "<<fNLayers[w]<<G4endl;
    pars>>fBarX[w]>>fBarY[w]>>fBarZ[w];
    pars>>fBarXoff[w];
    pars>>fWallX[w]>>fWallY[w]>>fWallZ[w];
    //G4cout<<"Wall position (cm)"<<fWallX[w]<<" "<<fWallY[w]<<" "<<fWallZ[w]<<" "<<G4endl;
    fTotBars+=fNLayers[w]*fNBars[w];
    if(fNLayers[w]==0) fTotBars+=fNLayers[w-1]*fNBars[w-1];//copy a previous wall
  }
  G4String check;
  pars>>check;
  if(check!=G4String("END")){G4cerr<<"Incorrect amount of pars A2DetTOF::ReadParameters"<<G4endl; exit(0);}

}

G4VPhysicalVolume* A2DetTOF::Construct(G4LogicalVolume* MotherLogical){

  fMotherLogic=MotherLogical;
  char stname[20];
  //Store the position information for each bar for writing
  G4ThreeVector Vwall;
  G4ThreeVector Vlay; //=wall+layer
  G4ThreeVector *Vbar;// =wall+layer +bar
  //output file for bar positiona
  std::fstream pos;
  pos.open("TOFpos.dat",std::ios::out);

  Vbar=new G4ThreeVector[fTotBars];
  for(G4int w=0;w<fNWalls;w++){
    //Make sure dimension in cm
    fBarX[w]*=cm;
    fBarY[w]*=cm;
    fBarZ[w]*=cm;
    fWallX[w]*=cm;
    fWallY[w]*=cm;
    fWallZ[w]*=cm;
    if(fNLayers[w]==0){
      //Copy the previous wall if NLayers set to 0 in config file
      fNLayers[w]=fNLayers[w-1];
      fNBars[w]=fNBars[w-1];
      fBarX[w]=fBarX[w-1];
      fBarY[w]=fBarY[w-1];
      fBarZ[w]=fBarZ[w-1];
      delete fRotAng[w];
      fRotAng[w]=new G4double[fNLayers[w]];
      for(G4int i=0;i<fNLayers[w-1];i++)
	fRotAng[w][i]=fRotAng[w-1][i];
     }
    //Wall Loop
    sprintf(stname,"%d",w);
    //The wall volumes
    G4Box *wallBox=new G4Box(G4String("TOFW")+G4String(stname),fBarX[w]/2*fNBars[w],fBarY[w]/2,fBarZ[w]/2*fNLayers[w]);
    G4LogicalVolume* wallLogic=new G4LogicalVolume(wallBox,fNistManager->FindOrBuildMaterial("G4_AIR"),G4String("TOFW")+G4String(stname));
    wallLogic->SetVisAttributes (G4VisAttributes::Invisible);
    //Place the wall in the lab.
    Vwall.set(fWallX[w],fWallY[w],fWallZ[w]);
    new G4PVPlacement(0,Vwall,wallLogic,wallBox->GetName(),fMotherLogic,false,1000+w);
 
    //The bar volumes
    G4Box *tofBox=new G4Box(G4String("TOFB")+G4String(stname),fBarX[w]/2,fBarY[w]/2,fBarZ[w]/2);
    G4LogicalVolume* barLogic=new G4LogicalVolume(tofBox,fNistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),tofBox->GetName());
    if(!fTOFSD){
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      fTOFSD = new A2SD("TOFSD",fTotBars);
      SDman->AddNewDetector( fTOFSD );		
    }
    barLogic->SetSensitiveDetector(fTOFSD);

    for(G4int l=0;l<fNLayers[w];l++){
      //Layer Loop
      //place the layer in the wall
      //first calc z position
      G4double zlay=-fNLayers[w]*fBarZ[w]/2+l*fBarZ[w]+fBarZ[w]/2;
      G4RotationMatrix *layRot=new G4RotationMatrix();
   //The layer volumes
    sprintf(stname,"%d",l);
    G4Box *layBox=new G4Box(G4String("TOFL")+G4String(stname),fBarX[w]/2*fNBars[w],fBarY[w]/2,fBarZ[w]/2);
    G4LogicalVolume* layLogic=new G4LogicalVolume(layBox,fNistManager->FindOrBuildMaterial("G4_AIR"),G4String("TOFL")+G4String(stname)); 
   layLogic->SetVisAttributes (G4VisAttributes::Invisible);
      layRot->rotateZ(fRotAng[w][l]*deg);
      new G4PVPlacement(layRot,G4ThreeVector(0,0,zlay),layLogic,layBox->GetName(),wallLogic,false,2000*w+l);
      Vlay=Vwall+G4ThreeVector(0,0,zlay);
      for(G4int b=0;b<fNBars[w];b++){
	//	G4cout<<b<<" "<<l<<" "<<fNLayers[w]<<" "<<fNBars[w]<<" "<<zlay<<" "<<fRotAng[w][l]<<G4endl;
	//Bar Loop
	//Place the bar in the layer
	//first calc x position
	G4double xbar=fNBars[w]*fBarX[w]/2-b*fBarX[w]-fBarX[w]/2+fBarXoff[w]*(fNBars[w]/2-b);
	Vbar[fCounter]=Vlay+G4ThreeVector(xbar,0,0);//store global position
	new G4PVPlacement(0,G4ThreeVector(xbar,0,0),barLogic,tofBox->GetName(),layLogic,false,fCounter++);
	G4ThreeVector End=Vbar[fCounter-1]+G4ThreeVector(0,fBarY[w]/2,0);
	End*=(*layRot);
	pos<<End.x()/cm<<" "<<End.y()/cm<<" "<<End.z()/cm<<G4endl;
	End=Vbar[fCounter-1]+G4ThreeVector(0,-fBarY[w]/2,0);
	End*=(*layRot);
	pos<<End.x()/cm<<" "<<End.y()/cm<<" "<<End.z()/cm<<G4endl;
      }

    }
  }
//   fstream pos;
//   pos.open("TOFpos.dat",std::ios::out);
//   for(G4int i=0;i<fCounter;i++)
//     pos<<Vbar[i].x()/cm<<" "<<Vbar[i].y()/cm<<" "<<Vbar[i].z()/cm<<G4endl;

  G4cout<<"Number of TOF bars made "<<fCounter<<" out of "<<fTotBars<<G4endl;
  return fMyPhysi;
}
