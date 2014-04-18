
#ifndef A2PrimaryGeneratorAction_h
#define A2PrimaryGeneratorAction_h 1

#include "A2DetectorConstruction.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "globals.hh"


//ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"

class G4ParticleGun;
class G4Event;
class A2PrimaryGeneratorMessenger;

//Event generator mode
enum { EPGA_g4, EPGA_phase_space, EPGA_ROOT, EPGA_Overlap};

class A2PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  A2PrimaryGeneratorAction();    
  ~A2PrimaryGeneratorAction();
  
  public:
  void GeneratePrimaries(G4Event*);

  void SetUpROOTInput();
  void SetInputFile(TString filename){fInFileName=filename;};
  void SetNParticlesToBeTracked(Int_t n){fNToBeTracked=n;fTrackThis=new Int_t[n];}
  void SetParticlesToBeTracked(Int_t ipart){if(fTrackThis) fTrackThis[fNToBeTcount++]=ipart;}
  Int_t *GetParticlesToBeTracked(){return fTrackThis;}
  G4int GetNEvents(){if(fGenTree)return static_cast<G4int>(fGenTree->GetEntries());
    else return -1;
  }
  
  Int_t GetNGenParticles(){return fNGenParticles;}
  TLorentzVector* GetGenLorentzVec(Int_t i){return fGenLorentzVec[i];}
  TLorentzVector** GetGenLorentzVecs(){return fGenLorentzVec;}
  TLorentzVector* GetBeamLorentzVec(){return fBeamLorentzVec;}
  Float_t* GetVertex(){return fGenPosition;}
  Int_t* GetGenPartType(){return fGenPartType;}
  
  void SetDetCon(A2DetectorConstruction* det){fDetCon=det;}
  G4bool IsInTarget(G4ThreeVector vec){
    G4Navigator* nav=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4VPhysicalVolume* vol=nav->LocateGlobalPointAndSetup(vec);
    // G4ThreeVector vpos=vol->GetTranslation()+vol->GetMother;
    // G4cout<<vol->GetName()<<" "<<vpos.x()/cm<<" "<<vpos.y()/cm<<" "<<vpos.z()/cm<<G4endl;
    return (vol->GetLogicalVolume()->GetMaterial()==fDetCon->GetTargetMaterial());
  }

private:
  G4ParticleGun*  fParticleGun;    //pointer to particle gun
  G4ParticleTable* fParticleTable;  //pointer to particle table
  G4ParticleDefinition** fParticleDefinition;  //pointer to particle definitions
  
  A2PrimaryGeneratorMessenger* fGunMessenger; //messenger of this class
  
  A2DetectorConstruction* fDetCon;   //pointer to the detector volumes
  
  TString fInFileName;  //Name of input file
  TFile* fGeneratedFile;       //Root file with input events ntuple produced bt mkin or MCGenerator
  TTree* fGenTree;  //Tree of input events stored in fGeneratedFile
  Int_t fNGenBranches;      //Total number of branches in ntuple
  Int_t fNGenParticles;     //Number of particles in ntuple
  Float_t fGenPosition[3]; //vertex position from ntuple, can't be double!
  Float_t ** fGen4Vectors;    //4 vector components from the ntuple branches
  TLorentzVector ** fGenLorentzVec;    //4 vector components from the ntuple branches converted into a ROOT lorentz vector
  TLorentzVector* fBeamLorentzVec; //For the beam or nonntuple input
  
  Float_t *fGenMass;        //Masses of the generated particles 
  Int_t *fGenPartType;        //Array of G3 particle types 
  Int_t *fTrackThis;         //Array carrying the index of particles to be tracked
  Int_t fNToBeTracked;    //Number of particles in input file to be tracked
  Int_t fNToBeTcount;     //counter for setting fTrackThis array
  Int_t fNevent;          //event number for the ROOT tree
  
  G4int fMode;    //select events via standard, phase space or ROOT input
  G4int fTargetWarning;
public:
  void SetMode(G4int mode){
    fMode=mode;
      G4cout<<"Set Mode "<<G4endl;
    if(fMode==EPGA_phase_space||fMode==EPGA_g4){
      fGenLorentzVec=new TLorentzVector*[2];
      fGenPartType=new G4int[2];
      fNGenParticles=2;//to write the A2CBoutput into dircos branch
      fGenLorentzVec[0]=new TLorentzVector(0,0,0,0);
      fGenLorentzVec[1]=new TLorentzVector(0,0,0,0);
      // fBeamLorentzVec=new TLorentzVector(0,0,0,0);
      fGenPartType[0]=-1;
      fGenPartType[1]=-1;
  
    }
     G4cout<<"Set Mode done "<<G4endl;
 
  }
  G4int GetMode(){return fMode;}
  //for phase space generator
private:
  void PhaseSpaceGenerator(G4Event* anEvent);
  void OverlapGenerator(G4Event* anEvent);
  G4ThreeVector fThreeVector;
  G4float fTmin;       //Min phase spce kinetic energy
  G4float fTmax;       //Max phase space kinetic energy
  G4float fThetamin;       //Min phase spce angle
  G4float fThetamax;       //Max phase space angle
  G4float fBeamXSigma;    //beam X width
  G4float fBeamYSigma;    //beam X width
  G4float fTargetZ0;      //Target Z position
  G4float fTargetThick;   //Target thickness
  G4float fTargetRadius;   //Target radius (in x-y plane)
  G4bool fdoPhaseSpace;
  G4float fSplitTheta;  //angle between particles for overlap generator

public:
  void SetTmin(G4float min){fTmin=min;}
  void SetTmax(G4float max){fTmax=max;}
  void SetThetamin(G4float min){fThetamin=min;}
  void SetThetamax(G4float max){fThetamax=max;}
  void SetBeamXSigma(G4float sigma){fBeamXSigma=sigma;}
  void SetBeamYSigma(G4float sigma){fBeamYSigma=sigma;}
  void SetTargetZ0(G4float z){fTargetZ0=z;}
  void SetTargetThick(G4float z){fTargetThick=z;}
  void SetTargetRadius(G4float z){fTargetRadius=z;}
 
  void SetSplitTheta(G4float min){fSplitTheta=min;}
 
};


#endif


