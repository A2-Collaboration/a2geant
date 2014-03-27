
#ifndef A2CBOutput_h
#define A2CBOutput_h 1

#include "A2PrimaryGeneratorAction.hh"
#include "A2DetectorConstruction.hh"
#include "A2Hit.hh"
#include "G4HCofThisEvent.hh"


#include "TLorentzVector.h"

const G4int MAXSIZE_NAI= 720;
const G4int MAXSIZE_TAPS= 512;
const G4int MAXSIZE_PID= 24;
const G4int MAXSIZE_MWPC = 400;

class A2CBOutput 
{
public:
  
  A2CBOutput();
  ~A2CBOutput();

protected:
  A2PrimaryGeneratorAction* fPGA;
  A2DetectorConstruction* fDET;
 
  TFile* fFile;   //Root output file
  TTree* fTree;    //ROOT output tree

  Float_t fbeam[5]; //beam branch Px,Py,Pz(all unit),Pt,E
  //  Float_t **fdircos; //direction cosines of generated particles
  Float_t fdircos[100][3]; //direction cosines of generated particles
  Float_t fecryst[MAXSIZE_NAI];  //Energy deposited in each NaI crystal
  Float_t ftcryst[MAXSIZE_NAI];  //Time of hit in each NaI crystal
  Float_t fectapfs[MAXSIZE_TAPS];  //Fast compenent of energy deposited in TAPS crystals 
  Float_t fectapsl[MAXSIZE_TAPS];  //Slow compenent of energy deposited in TAPS crystals(currently Edep in taps)
  Float_t *felab;    //Energy of initial generatd particles
  Float_t feleak;    //Energy leaking out of system (NOT CURRENTLY IMPLEMENTED)
  Float_t fenai;     //Total energy deposited in NaI
  Float_t fetot;     //Total energy deposited in all detectors
  Float_t feveto[MAXSIZE_PID];  //Energy deposited in PID elements
  Float_t ftveto[MAXSIZE_PID];  //Time of hit in PID elements
  Float_t fevtaps[MAXSIZE_TAPS];  //Energy deposited in TAPS veto counter
  Int_t ficryst[MAXSIZE_NAI];   //id numbers of Nai hits
  Int_t fictaps[MAXSIZE_TAPS];   //id numbers of TAPS hits
  Int_t fivtaps[MAXSIZE_TAPS];   //id numbers of TAPS veto hits
  Int_t *fidpart;   //g3 id number of initial generated particle
  Int_t fiveto[MAXSIZE_PID];    //id number of the PID hits
  Int_t fnhits;    //Number of NaI hits
  Int_t fnpart;    //number of generated particles (not necessarily same as # tracked)
  Int_t fntaps;    //Number of hits in TAPS
  Int_t fnvtaps;    //Number of veto hits in TAPS
  Float_t *fplab;   //momentum of original generated particles
  Float_t ftctaps[MAXSIZE_TAPS];  //Time of hits in TAPS
  Float_t *fvertex;  //Vertex position
  Int_t fvhits;     //Number of hits in PID
 
  Int_t fnmwpc; //total no. wc hits
  Int_t fimwpc[MAXSIZE_MWPC]; //layer id of hit
  Float_t fmposx[MAXSIZE_MWPC]; //position of hit
  Float_t fmposy[MAXSIZE_MWPC]; //position of hit
  Float_t fmposz[MAXSIZE_MWPC]; //position of hit
  Float_t femwpc[MAXSIZE_MWPC]; //position of hit

  //New TOF hits
  Int_t fToFTot; //total number of constructed tof bars
  Int_t fntof; //Number of hits in TOF
  Int_t *ftofi; //hit bar indexes
  Float_t *ftofe; //hit bar energy deposits
  Float_t *ftoft; //hit bar time
  Float_t *ftofx; //x hit position
  Float_t *ftofy; //y hit position
  Float_t *ftofz; //z hit position

  TLorentzVector** fGenLorentzVec;
  TLorentzVector* fBeamLorentzVec;
  Int_t *fGenPartType;

  G4int fCBCollID;
  G4int fTAPSCollID;

public:
  void SetFile(TFile* f){fFile=f;fTree->SetDirectory(fFile);}
  TFile* GetFile(){return fFile;}

  void SetTree(TTree* t){fTree=t;}
  TTree* GetTree(){return fTree;}
 
  void SetBranches();
  
  void WriteTree(){fTree->Write();}
  void WriteHit(G4HCofThisEvent* );
  void WriteGenInput();
};



#endif










