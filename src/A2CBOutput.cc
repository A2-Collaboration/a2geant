#include "A2CBOutput.hh"
#include "G4RunManager.hh"
#include "TClonesArray.h"
#include <list>

#define MAX_NPART 100

A2CBOutput::A2CBOutput(){
	fFile=NULL;
	fTree=NULL;
	fPGA=const_cast<A2PrimaryGeneratorAction*>(static_cast<const A2PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()));

	fDET=const_cast<A2DetectorConstruction*>(static_cast<const A2DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()));
	//Need to get the number of initial particles

	//Get the LorentzVectors of the initial particles

	fBeamLorentzVec=fPGA->GetBeamLorentzVec();//Will take the default beam if no ntuple

	fvertex=fPGA->GetVertex();

	//create Tree
	fTree=new TTree("h12","Crystals");
	fTree->SetAutoSave();

	//Collection IDs for decoding hit collections
	fCBCollID = -1;
	fTAPSCollID = -1;

	//TOF stuff
	if(fDET->GetNToFbars()>0){
		fToFTot=fDET->GetNToFbars();
		ftofi=new Int_t[fToFTot];
		ftofe=new Float_t[fToFTot];
		ftoft=new Float_t[fToFTot];
		ftofx=new Float_t[fToFTot];
		ftofy=new Float_t[fToFTot];
		ftofz=new Float_t[fToFTot];
	}

}
A2CBOutput::~A2CBOutput(){
	if(fTree)delete fTree;
}
//Float_t dircos[4][3];
void A2CBOutput::SetBranches(){

	if(!fTree){
		G4cout<<"A2CBOutput::SetBranches() Can't set branches have to set fTree first!"<<G4endl;
		return;
	}
	Int_t basket =64000;

    TClonesArray* trueinfo = fPGA->GetTruePtr();
    G4cout << " Fetched True Ptr" << G4endl;

  fTree->Branch("nhits",&fnhits,"fnhits/I",basket);                 //g4
  fTree->Branch("npart",&fnpart,"fnpart/I",basket);                 //true info: number of true particles
  fTree->Branch("ntaps",&fntaps,"fntaps/I",basket);                 //g4
  fTree->Branch("nvtaps",&fnvtaps,"fnvtaps/I",basket);              //g4
  fTree->Branch("vhits",&fvhits,"fvhits/I",basket);                 //g4
  fTree->Branch("plab",fplab,"fplab[fnpart]/F",basket);             //true info(Impulse? TLorentzVector::Rho() )
  fTree->Branch("tctaps",ftctaps,"ftctaps[fntaps]/F",basket);       //g4
  fTree->Branch("vertex",fvertex,"fvertex[3]/F",basket);            //vertex position
  fTree->Branch("beam",fbeam,"fbeam[5]/F",basket);                  //true info: Beam parameters (x,y,z) of uni, Rho E
  fTree->Branch("dircos",fdircos,"fdircos[fnpart][3]/F",basket);    //true info (3vector unit)
  fTree->Branch("ecryst",fecryst,"fecryst[fnhits]/F",basket);       //g4
  fTree->Branch("tcryst",ftcryst,"ftcryst[fnhits]/F",basket);       //g4
  fTree->Branch("ectapfs",fectapfs,"fectapfs[fntaps]/F",basket);    //NOT IMPLELEMTED
  fTree->Branch("ectapsl",fectapsl,"fectapsl[fntaps]/F",basket);    //g4
  fTree->Branch("elab",felab,"felab[fnpart]/F",basket);             //true info (E)
  fTree->Branch("eleak",&feleak,"feleak/F",basket);                 //NOT IMPLEMENTED
  fTree->Branch("enai",&fenai,"fenai/F",basket);                    //NOT IMPLEMENTED??
  fTree->Branch("etot",&fetot,"fetot/F",basket);                    //g4
  fTree->Branch("eveto",feveto,"feveto[fvhits]/F",basket);          //g4
  fTree->Branch("tveto",ftveto,"ftveto[fvhits]/F",basket);          //g4
  fTree->Branch("evtaps",fevtaps,"fevtaps[fnvtaps]/F",basket);      //g4
  fTree->Branch("icryst",ficryst,"ficryst[fnhits]/I",basket);       //g4
  fTree->Branch("ictaps",fictaps,"fictaps[fntaps]/I",basket);       //g4
  fTree->Branch("ivtaps",fivtaps,"fictaps[fnvtaps]/I",basket);      //g4
  fTree->Branch("idpart",fidpart,"fidpart[fnpart]/I",basket);       //true info (G3 Particle ID)
  fTree->Branch("iveto",fiveto,"fiveto[fvhits]/I",basket);          //g4
  fTree->Branch("nmwpc",&fnmwpc,"fnmwpc/I",basket);                 //g4
  fTree->Branch("imwpc",fimwpc,"fimwpc[fnmwpc]/I",basket);          //g4
  fTree->Branch("mposx",fmposx,"fmposx[fnmwpc]/F",basket);          //g4
  fTree->Branch("mposy",fmposy,"fmposy[fnmwpc]/F",basket);          //g4
  fTree->Branch("mposz",fmposz,"fmposz[fnmwpc]/F",basket);          //g4
  fTree->Branch("emwpc",femwpc,"femwpc[fnmwpc]/F",basket);          //g4
  //tof stuff
  if(fToFTot>0){                                                    //g4: tof is all from g4
    fTree->Branch("ntof",&fntof,"fntof/I",basket);
    fTree->Branch("tofi",ftofi,"ftofi[fntof]/I",basket);
    fTree->Branch("tofe",ftofe,"ftofe[fntof]/F",basket);
    fTree->Branch("toft",ftoft,"ftoft[fntof]/F",basket);
    fTree->Branch("tofx",ftofx,"ftofx[fntof]/F",basket);
    fTree->Branch("tofy",ftofy,"ftofy[fntof]/F",basket);
    fTree->Branch("tofz",ftofz,"ftofz[fntof]/F",basket);
  }
  fTree->Branch("mc_evt_id", fPGA->MCEvtIDPtr(),"mc_evt_id/L",basket);
  fTree->Branch("mc_rnd_id", fPGA->MCRndIDPtr(),"mc_evt_id/L",basket);

}
void A2CBOutput::WriteHit(G4HCofThisEvent* HitsColl){
    fnpart=fPGA->GetNGenParticles()-1; //-1 for beam
	G4int CollSize=HitsColl->GetNumberOfCollections();
	//G4cout<<"Collection size "<<CollSize<<" "<<HitsColl->GetHC(0)->GetName()<<" "<<HitsColl->GetHC(1)->GetName()<<G4endl;
	//G4cout<<"Collection size "<<CollSize<<G4endl;
	fnhits=fntaps=fnvtaps=fvhits=fntof= fnmwpc=0;
	fetot=0;
	G4int hci=0;
	for(G4int i=0;i<CollSize;i++){
		//Get the ball hit info to be written to output
		A2HitsCollection* hc;
		//Look for the hits collections with entries 
		while(!(hc=static_cast<A2HitsCollection*>(HitsColl->GetHC(hci++))));
		//if(!hc)continue; //no hits in that detector
		G4int hc_nhits=hc->entries();
		//G4cout<<i<<" "<<hc->GetName()<< " "<<hc_nhits<<G4endl;
		if(hc->GetName()=="A2SDHitsCBSD"||hc->GetName()=="A2SDHitsVisCBSD"){
			fnhits=hc_nhits;
			for(Int_t i=0;i<fnhits;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				fecryst[i]=hit->GetEdep()/GeV;
				ftcryst[i]=hit->GetTime()/ns;
				ficryst[i]=hit->GetID();
				fetot+=fecryst[i];
			}
		}
		if(hc->GetName()=="A2SDHitsTAPSSD"||hc->GetName()=="A2SDHitsTAPSVisSD"){
			fntaps=hc_nhits;
			for(Int_t i=0;i<fntaps;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				fectapsl[i]=hit->GetEdep()/GeV;
				fictaps[i]=hit->GetID();
				ftctaps[i]=hit->GetTime()/ns;
				//fetot+=fectapsl[i];//***TEMP!!!!
			}
		}
		if(hc->GetName()=="A2SDHitsTAPSVSD"||hc->GetName()=="A2SDHitsTAPSVVisSD"){
			fnvtaps=hc_nhits;
			for(Int_t i=0;i<fnvtaps;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				fevtaps[i]=hit->GetEdep()/GeV;
				fivtaps[i]=hit->GetID();
			}
		}
		if(hc->GetName()=="A2SDHitsPIDSD"){
			fvhits=hc_nhits;
			for(Int_t i=0;i<fvhits;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				feveto[i]=hit->GetEdep()/GeV;
				ftveto[i]=hit->GetTime()/ns;
				fiveto[i]=hit->GetID();
			}
		}
		if(hc->GetName().contains("A2MWPCSD")){
			// fnmwpc=hc_nhits;
			for(Int_t i=0;i<hc_nhits;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				fimwpc[fnmwpc+i]   = hit->GetID();
				fmposx[fnmwpc+i] = (Float_t)hit->GetPos().getX();
				fmposy[fnmwpc+i] = (Float_t)hit->GetPos().getY();
				fmposz[fnmwpc+i] = (Float_t)hit->GetPos().getZ();
				femwpc[fnmwpc+i] = (Float_t)hit->GetEdep()/GeV;
				//	G4cout<<fnmwpc+i<<" "<<fimwpc[fnmwpc+i]<<" "<<fmpos[fnmwpc+i][0]<<" "<<fmpos[fnmwpc+i][1]<<" "<<fmpos[fnmwpc+i][2]<<G4endl;
			}
			fnmwpc+=hc_nhits;
		}
		if(hc->GetName()=="A2SDHitsTOFSD"){
			fntof=hc_nhits;
			for(Int_t i=0;i<fntof;i++){
				A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(i));
				ftofe[i]=hit->GetEdep()/GeV;
				ftoft[i]=hit->GetTime()/ns;
				ftofx[i]=hit->GetPos().x()/cm;
				ftofy[i]=hit->GetPos().y()/cm;
				ftofz[i]=hit->GetPos().z()/cm;
				ftofi[i]=hit->GetID();
			}
		}
	}

}
void A2CBOutput::WriteGenInput(){

    A2PrimaryGeneratorAction::ParticleList* simp = fPGA->SimParticles();

    fnpart = simp->size();

	//Note fvertex is already the pointer to fPGA::fGenPosition 
	//Get the generated input info to be written to output 

    TVector3 vec=fBeamLorentzVec->Vect().Unit();
	fbeam[0]=vec.X();
	fbeam[1]=vec.Y();
	fbeam[2]=vec.Z();
    fbeam[3]=fBeamLorentzVec->Rho();    // pluto already uses GeV internally
    fbeam[4]=fBeamLorentzVec->E();


    A2PrimaryGeneratorAction::ParticleList::iterator p;

    int i =0;
    for(  p=simp->begin(); p != simp->end(); ++p, ++i ){

        PParticle* part = *p;

        //vec = fGenLorentzVec[i]->Vect().Unit();
        vec = part->Vect().Unit();

        fdircos[i][0]=static_cast<Float_t>(vec.X());
        fdircos[i][1]=static_cast<Float_t>(vec.Y());
        fdircos[i][2]=static_cast<Float_t>(vec.Z());


        //felab[i]=fGenLorentzVec[i+1]->E()/GeV;
        felab[i]=part->E();

        //fplab[i]=fGenLorentzVec[i+1]->Rho()/GeV;
        fplab[i]=part->P();

        //fidpart[i]=fGenPartType[i+1];
        fidpart[i] = part->ID(); //TODO: convert to geant3 ID
      }

}
