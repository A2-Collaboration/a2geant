
#include "A2PrimaryGeneratorAction.hh"

#include "A2PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4String.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "Randomize.hh"

#include "TObjArray.h"
#include "TBranch.h"
#include "TString.h"

#include "MCNtuple.h"

//String that converts g3 particle number into a G4 particle name
//const G4String G3ParticleType[]={"nothing","gamma","e+","e-","nu_e","mu+","mu-","pi0","pi+","pi-","kaon0L","kaon+","kaon-","neutron","proton","anti_proton","kaon0S","eta","lambda","sigma+","sigma0","sigma-","xi0","xi-","omega","anti_neutron","anti_lambda","anti_sigma-","anti_sigma0","anti_sigma+","anti_xi0","anti_xi+","GenericIon","","","","","","","","","","","","","deuteron","triton","","","He3","","","","","","","","","","","","","deuteron","triton","","","He3","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","deuteron"};

A2PrimaryGeneratorAction::A2PrimaryGeneratorAction()

{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  //create a messenger for this class
  fGunMessenger = new A2PrimaryGeneratorMessenger(this);
  //Get Particle table pointer
  fParticleTable=G4ParticleTable::GetParticleTable();
  fParticleDefinition=NULL;
  //Set up ROOT input
  fGeneratedFile=NULL;
  fGenTree=NULL;
  fGen4Vectors=NULL;
  fGenPartType=NULL;
  fGenMass=NULL;
  fTrackThis=NULL;
  fGenLorentzVec=NULL;
  fBeamLorentzVec=NULL;
  fBeamLorentzVec=new TLorentzVector(0,0,0,0);//for beam or interactive events

  fThreeVector=G4ThreeVector(0,0,1);
  //default phase space limits
  fTmin=0;
  fTmax=400*CLHEP::MeV;
  fThetamin=0;
  fThetamax=180*CLHEP::deg;
  fBeamXSigma=0.5*CLHEP::cm;
  fBeamYSigma=0.5*CLHEP::cm;
  fTargetZ0=0*CLHEP::cm;
  fTargetThick=5*CLHEP::cm;
  fTargetRadius=2*CLHEP::cm;
  //overlap
  fSplitTheta=0;
 //default mode is g4 command line input
  fMode=EPGA_g4;
  fTargetWarning=0;
  //G4int setup=SetUpROOTInput("/scratch/crystalball/mc/kin_pi0p_100000.root");
  fNevent=0;
  fNToBeTcount=0;
  fNToBeTracked=0;
  fNGenParticles=1;//for interactive use
  fNGenBranches=0;

  fDetCon=NULL;
  fInFileName="";

}



A2PrimaryGeneratorAction::~A2PrimaryGeneratorAction()
{
  if(fGeneratedFile){
    for(Int_t i=0;i<fNGenParticles;i++) delete fGen4Vectors[i];
    //for(Int_t i=0;i<fNGenParticles;i++) delete fParticleDefinition[i];
    for(Int_t i=0;i<fNGenParticles;i++) delete fGenLorentzVec[i];
    delete fGen4Vectors;
    delete fGenLorentzVec;
    delete fGenPartType;
    delete fTrackThis;
    delete fGenMass;
    fGeneratedFile->Close();
    delete fGeneratedFile;
   }
  delete fParticleGun;
  delete fGunMessenger;
  delete fBeamLorentzVec;
}



void A2PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //This function is called at the begining of event
  Float_t Mass;
  Float_t P;
  Float_t r_v,x_v,y_v,test_v;
  G4ThreeVector pvec;
  switch(fMode){
  case EPGA_g4:
    if(!fGenLorentzVec)SetMode(EPGA_g4);//in case not called in macro
    //standard g4 command line event generator
    fParticleGun->GeneratePrimaryVertex(anEvent);
    Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
    P=sqrt(fParticleGun->GetParticleEnergy()*fParticleGun->GetParticleEnergy()-Mass*Mass);
    pvec=(fParticleGun->GetParticleMomentumDirection())*P;
    fBeamLorentzVec->SetXYZM(pvec.x(),pvec.y(),pvec.z(),Mass);
    fGenLorentzVec[1]->SetXYZM(fThreeVector.x(),fThreeVector.y(),fThreeVector.z(),Mass);
    break;

  case EPGA_phase_space:
    //Phase space generator
    PhaseSpaceGenerator(anEvent);
    break;

  case EPGA_Overlap:
    //Phase space generator for overlapping paticles
    OverlapGenerator(anEvent);
    break;

  case EPGA_ROOT:
    if(fGenTree&&fNToBeTracked>0){
      //Get the event from input tree
      fGenTree->GetEvent(fNevent++);
      //Set vertex position
      fThreeVector.setX(fGenPosition[0]*CLHEP::cm);
      fThreeVector.setY(fGenPosition[1]*CLHEP::cm);
      fThreeVector.setZ(fGenPosition[2]*CLHEP::cm);
      while(!IsInTarget(fThreeVector)){
	if(fTargetWarning==0){
	G4cout<<"Warning vertex position from ROOT input file is not inside target cell"<<G4endl;
	G4cout<<"I have taken the liberty of choosing a new one inside!! see void A2PrimaryGeneratorAction::GeneratePrimaries"<<G4endl<<"I will not print this warning again, but will continue to move the vertex inside the target, to stop this please modify A2PrimaryGeneratorAction.cc around line 130"<<G4endl;
	}
	fThreeVector.setZ(fDetCon->GetTarget()->GetCenter().z()+fDetCon->GetTarget()->GetLength()/2*(2*G4UniformRand()-1));
	r_v=0.0;
    	while(r_v == 0.0 ) {
     	x_v = fDetCon->GetTarget()->GetRadius()*(2*G4UniformRand()-1);
     	y_v = fDetCon->GetTarget()->GetRadius()*(2*G4UniformRand()-1);
     	test_v = pow(x_v,2)+pow(y_v,2);
     	if (test_v<pow(fDetCon->GetTarget()->GetRadius(),2)) r_v = test_v;
    	}
	fThreeVector.setX(x_v);
	fThreeVector.setY(y_v);
	if(fTargetWarning==0){G4cout<<fThreeVector<<" from "<<"("<<fGenPosition[0]*CLHEP::cm<<","<<fGenPosition[1]*CLHEP::cm<<","<<fGenPosition[2]*CLHEP::cm<<")*mm"<<G4endl;}
	fGenPosition[0]=fThreeVector.x()/CLHEP::cm;
	fGenPosition[1]=fThreeVector.y()/CLHEP::cm;
	fGenPosition[2]=fThreeVector.z()/CLHEP::cm;
	fTargetWarning=1;
      };
       fParticleGun->SetParticlePosition(fThreeVector);
     //Loop over tracked particles, set particle gun and create vertex for each particle

      for(Int_t i=0;i<fNToBeTracked;i++){
	fParticleGun->SetParticleDefinition(fParticleDefinition[fTrackThis[i]]);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fGen4Vectors[fTrackThis[i]][0]*CLHEP::GeV,fGen4Vectors[fTrackThis[i]][1]*CLHEP::GeV,fGen4Vectors[fTrackThis[i]][2]*CLHEP::GeV).unit());
	fParticleGun->SetParticleEnergy(fGen4Vectors[fTrackThis[i]][3]*CLHEP::GeV-fGenMass[fTrackThis[i]]);
	fParticleGun->GeneratePrimaryVertex(anEvent);
      }
      for(Int_t i=1;i<fNGenParticles;i++){//0 is not used
	//Set ROOT LorentzVector, first calculate momentum
	//	if(!fParticleDefinition[i])continue;
	//	Float_t P=sqrt( fGen4Vectors[i][3]*CLHEP::GeV*fGen4Vectors[i][3]*CLHEP::GeV-fGenMass[i]*fGenMass[i]);
	P=fGen4Vectors[i][4]*CLHEP::GeV;
	if(fParticleDefinition[i])fGenLorentzVec[i]->SetXYZM(P*fGen4Vectors[i][0],P*fGen4Vectors[i][1],P*fGen4Vectors[i][2],fGenMass[i]);
	else fGenLorentzVec[i]->SetXYZM(P*fGen4Vectors[i][0],P*fGen4Vectors[i][1],P*fGen4Vectors[i][2],sqrt(fGen4Vectors[i][3]*fGen4Vectors[i][3]*CLHEP::GeV*CLHEP::GeV-P*P));
      }
      //Assume photon beam!
      P=fGen4Vectors[0][3]*CLHEP::GeV;
      fBeamLorentzVec->SetXYZM(fGen4Vectors[0][0]*P,fGen4Vectors[0][1]*P,fGen4Vectors[0][2]*P,0);
    }
    else{ G4cerr<<"ROOT input mode specidied but no input file given"<<G4endl; exit(1);}
    break;
  default:
    G4cerr<<"Unknown mode given to A2PrimiaryGeneratorAction"<<G4endl;
    exit(1);

  }
}

void A2PrimaryGeneratorAction::PhaseSpaceGenerator(G4Event* anEvent){
  if(fGenPartType[1]==-1){fGenPartType[1]= PDGtoG3(fParticleGun->GetParticleDefinition()->GetPDGEncoding());G4cout<<"P type "<<fGenPartType[1]<<G4endl;}
  //4-momenta
  G4float theta=acos((cos(fThetamax)-cos(fThetamin))*G4UniformRand()+cos(fThetamin));
  G4float phi=2*3.141592653589*G4UniformRand();
  G4float T=fTmin+(fTmax-fTmin)*G4UniformRand();
  G4float Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4float E=T+Mass;
  G4float P=sqrt(E*E-Mass*Mass);
  fThreeVector.setMag(P);
  fThreeVector.setTheta(theta);
  fThreeVector.setPhi(phi);
  fParticleGun->SetParticleMomentumDirection(fThreeVector.unit());
  fParticleGun->SetParticleEnergy(T);
  //save root output
  fBeamLorentzVec->SetXYZM(fThreeVector.x(),fThreeVector.y(),fThreeVector.z(),Mass);
  fGenLorentzVec[1]->SetXYZM(fThreeVector.x(),fThreeVector.y(),fThreeVector.z(),Mass);
  //position vertex
  G4float tx=1E10*CLHEP::m;
  G4float ty=1E10*CLHEP::m;
  // while(tx*tx+ty*ty>fTargetRadius*fTargetRadius||(!IsInTarget(fThreeVector))){
  while(tx*tx+ty*ty>fTargetRadius*fTargetRadius){
    tx=G4RandGauss::shoot(0,fBeamXSigma);
    ty=G4RandGauss::shoot(0,fBeamYSigma);
    fThreeVector.setX(tx);
    fThreeVector.setY(ty);
    fThreeVector.setZ(fTargetZ0+fTargetThick/2*(2*G4UniformRand()-1));
  }

  fParticleGun->SetParticlePosition(fThreeVector);
  fGenPosition[0]=fThreeVector.x()/CLHEP::cm;
  fGenPosition[1]=fThreeVector.y()/CLHEP::cm;
  fGenPosition[2]=fThreeVector.z()/CLHEP::cm;


  //produce event
  fParticleGun->GeneratePrimaryVertex(anEvent);

}
void A2PrimaryGeneratorAction::OverlapGenerator(G4Event* anEvent){
  //phase space genreator + creates an additional particle at angle fSplitTheta to the first

  G4float tx=1E10*CLHEP::m;
  G4float ty=1E10*CLHEP::m;
  while(tx*tx+ty*ty>fTargetRadius*fTargetRadius){
    tx=G4RandGauss::shoot(0,fBeamXSigma);
    ty=G4RandGauss::shoot(0,fBeamYSigma);
  }
  fThreeVector.setX(tx);
  fThreeVector.setY(ty);
  fThreeVector.setZ(fTargetZ0+fTargetThick/2*(2*G4UniformRand()-1));
  //G4cout<<"PGA "<<fTargetZ0/CLHEP::cm<<" "<<fTargetThick/CLHEP::cm<<" "<<(2*G4UniformRand()-1)<<G4endl;
  fParticleGun->SetParticlePosition(fThreeVector);
  fGenPosition[0]=fThreeVector.x()/CLHEP::cm;
  fGenPosition[1]=fThreeVector.y()/CLHEP::cm;
  fGenPosition[2]=fThreeVector.z()/CLHEP::cm;
  G4float theta=acos((cos(fThetamax)-cos(fThetamin))*G4UniformRand()+cos(fThetamin));
  G4float phi=2*3.141592653589*G4UniformRand();
  G4float T=fTmin+(fTmax-fTmin)*G4UniformRand();
  G4float Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4float E=T+Mass;
  G4float P=sqrt(E*E-Mass*Mass);
  //theta=90*CLHEP::deg;
  //phi=30.*CLHEP::deg;
  fThreeVector.setMag(P);
  fThreeVector.setTheta(theta);
  fThreeVector.setPhi(phi);
  fParticleGun->SetParticleMomentumDirection(fThreeVector.unit());
  fParticleGun->SetParticleEnergy(T);

  //intitial particle
  fParticleGun->GeneratePrimaryVertex(anEvent);

  //and overlap
  T=fTmin+(fTmax-fTmin)*G4UniformRand();
  E=T+Mass;
  P=sqrt(E*E-Mass*Mass);
  fThreeVector.setMag(P);

  fThreeVector.setTheta(theta+fSplitTheta);
  fThreeVector.rotate(fParticleGun->GetParticleMomentumDirection(),2*3.141592653589*G4UniformRand());
  fParticleGun->SetParticleEnergy(T);
  fParticleGun->SetParticleMomentumDirection(fThreeVector.unit());
  fParticleGun->GeneratePrimaryVertex(anEvent);

}
void A2PrimaryGeneratorAction::SetUpROOTInput(){
  //Set ROOT input to default mode
  // if(fMode==EPGA_ROOT) {G4cout<<"A2PrimaryGeneratorAction::SetUpROOTInput(TString filename) Already declared input file "<<fGeneratedFile->GetName()<<G4endl;return;}
  if(fInFileName==TString(""))return;
  G4cout<<"A2PrimaryGeneratorAction::SetUpROOTInput() input file set as "<<fInFileName<<G4endl;

  fMode=EPGA_ROOT;
  //Open ROOT file
  // fGeneratedFile=new TFile(filename);
  fGeneratedFile=new TFile(fInFileName);
  if(!fGeneratedFile){
    G4cerr<<"G4int A2PrimaryGeneratorAction::SetUpRootInput() - Didn't find filename fInFileName"<<G4endl;
    G4cerr<<"Need to call /A2/generator/InputFile or use --if=filename option on command line"<<G4endl;
    exit(1);
  }
  //Get ntuple
  fGenTree=dynamic_cast<TTree*>(fGeneratedFile->Get("h1"));
  if(!fGenTree){
    G4cerr<<"G4int A2PrimaryGeneratorAction::SetUpRootInput(TString filename) - Didn't find an ntuple h1 in filename."<<G4endl;
    G4cerr<<"It contains : "<<G4endl;
    fGeneratedFile->ls();
    exit(1);
  }
  //Get the number of branches
  fNGenBranches=fGenTree->GetNbranches();
  //Calculate number of particles assuming 3 position branches and 5 particle variables
  fNGenParticles=(fNGenBranches-3)/5;
  fGen4Vectors=new Float_t*[fNGenParticles];
  fGenLorentzVec=new TLorentzVector*[fNGenParticles];
  fParticleDefinition=new G4ParticleDefinition*[fNGenParticles];
  fGenMass=new Float_t[fNGenParticles];
  fGenPartType=new Int_t[fNGenParticles];
  for(Int_t i=0;i<fNGenParticles;i++){
    fGen4Vectors[i]=NULL;
    fGenLorentzVec[i]=NULL;
    fParticleDefinition[i]=NULL;
  }
  fGenLorentzVec[0]=new TLorentzVector();//dummy, not actually used
  if(fNToBeTracked==0){//default track all
    fNToBeTracked=fNGenParticles-1; //-1 for beam
    fTrackThis=new Int_t[fNToBeTracked];
    for(Int_t i=1;i<=fNToBeTracked;i++) fTrackThis[i-1]=i;
    G4cout<<"A2PrimaryGeneratorAction::SetUpRootInput By default tracking all "<<fNToBeTracked<<" generated particles."<<G4endl;
    G4cout<<"If you do not want to track all particles please set using XXX"<<G4endl;
  }
  else if(fNToBeTracked!=fNToBeTcount) G4cout<<"WARNING A2PrimaryGeneratorAction::SetUpRootInput You have not set enough particles to be tracked!!!!"<<G4endl;
  else G4cout<<"A2PrimaryGeneratorAction::SetUpRootInput You have chosen to track "<<fNToBeTracked<<" generated particles."<<G4endl;
  for(Int_t j=0;j<fNGenParticles;j++) fGen4Vectors[j]=new Float_t[5];
  //Loop over branches and set addresses
  TObjArray* objarray=fGenTree->GetListOfBranches();
  for(Int_t i=0;i<fNGenBranches;i++){
    TBranch *branch=dynamic_cast<TBranch*>(objarray->At(i));
    TString bname=TString(const_cast<char*>(branch->GetName()));
    //Get Position coordinates
    if(bname=="X_vtx")branch->SetAddress(&fGenPosition[0]);
    if(bname=="Y_vtx")branch->SetAddress(&fGenPosition[1]);
    if(bname=="Z_vtx")branch->SetAddress(&fGenPosition[2]);
    //Particle 4 vectors
    //First beam
    if(bname=="Px_bm")branch->SetAddress(&fGen4Vectors[0][0]);
    if(bname=="Py_bm")branch->SetAddress(&fGen4Vectors[0][1]);
    if(bname=="Pz_bm")branch->SetAddress(&fGen4Vectors[0][2]);
    if(bname=="En_bm")branch->SetAddress(&fGen4Vectors[0][3]);

    //Then final state
    if(!bname.Contains("bm")&&!bname.Contains("vtx")){
      char* sbname=const_cast<char*>(bname.Data());
      Int_t index=(TString(sbname[4])+TString(sbname[5])).Atoi(); //Get the number of the particle
      fGenPartType[index]=(TString(sbname[6])+TString(sbname[7])).Atoi();
       if((bname.Contains("Px"))) G4cout<<"A2PrimaryGeneratorAction::SetUpRootInput, adding a "<<GetNamefromG3(fGenPartType[index])<<" as index "<<index<<G4endl;
      if(bname.Contains("Px"))branch->SetAddress(&fGen4Vectors[index][0]);
      if(bname.Contains("Py"))branch->SetAddress(&fGen4Vectors[index][1]);
      if(bname.Contains("Pz"))branch->SetAddress(&fGen4Vectors[index][2]);
      if(bname.Contains("Pt"))branch->SetAddress(&fGen4Vectors[index][4]);
      if(bname.Contains("En"))branch->SetAddress(&fGen4Vectors[index][3]);

      //G4cout<<fGenPartType[index]<<" "<<GetPDGfromG3(fGenPartType[index])<<G4endl;
      fParticleDefinition[index]=NULL;
      if((fParticleDefinition[index]=fParticleTable->FindParticle(GetPDGfromG3(fGenPartType[index])))) ;
      else if((fParticleDefinition[index]=fParticleTable->GetIonTable()->GetIon(GetPDGfromG3(fGenPartType[index])))) ;
      else {
 	G4cout<<"As yet undefined particle check lists in MCNtuple.h"<<G4endl;
 	G4cout<<"G3 number is "<<fGenPartType[index]<<" PDG id "<<GetPDGfromG3(fGenPartType[index])<<G4endl;
	//	exit(1);
      }

      //G4cout<<"here2"<<fParticleDefinition[index]<<G4endl;
      if(fParticleDefinition[index]){
	fGenMass[index]=fParticleDefinition[index]->GetPDGMass();
      //G4cout<<"here3 "<<fGenMass[index]<<" "<<fParticleDefinition[index]->GetParticleName()<<G4endl;
      }
      fGenLorentzVec[index]=new TLorentzVector(0,0,0,fGenMass[index]*CLHEP::GeV);
    }
  }
  for(Int_t i=0;i<fNToBeTracked;i++)
    if(fParticleDefinition[fTrackThis[i]])G4cout<<"Will track "<<fParticleDefinition[fTrackThis[i]]->GetParticleName()<<" with index "<<fTrackThis[i]<<G4endl;
    else {G4cout<<"Sorry cannot track this undefined particle check lists in MCNtuple.h"<<G4endl;exit(1);}
}

// void A2PrimaryGeneratorAction::SetUpROOTInputTParticle(TString filename){

// }
