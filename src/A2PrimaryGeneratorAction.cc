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

#include "PParticle.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "TIDhack.h"



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
    fParticleDefinition=NULL;	//G4int setup=SetUpROOTInput("/scratch/crystalball/mc/kin_pi0p_100000.root");
	//Set up ROOT input
	fGeneratedFile=NULL;
	fGenTree=NULL;

	fThreeVector=G4ThreeVector(0,0,1);

	//default phase space limits
    fTmin         = 0;
    fTmax         = 400*MeV;
    fThetamin     = 0;
    fThetamax     = 180*deg;
    fBeamXSigma   = 0.5*cm;
    fBeamYSigma   = 0.5*cm;
    fTargetZ0     =   0*cm;
    fTargetThick  =   5*cm;
    fTargetRadius =   2*cm;

	//overlap
	fSplitTheta=0;

	//default mode is g4 command line input
    fMode=EPGA_ROOT;

	fTargetWarning=0;

	fNevent=0;
    fNGenParticles=0;

    fGenParticles = NULL;

	fDetCon=NULL;
	fInFileName="";

    fGen4Vectors        = new Float_t*[100];
    fGenLorentzVec      = new TLorentzVector*[100];
    fParticleDefinition = new G4ParticleDefinition*[100];
    fGenMass            = new Float_t[100];
    fGenPartType        = new Int_t[100];

    for(Int_t i=0;i<100;i++){
        fGen4Vectors[i]=NULL;
        fGenLorentzVec[i]=NULL;
        fParticleDefinition[i]=NULL;
    }

    G4ParticleTable* g4part = G4ParticleTable::GetParticleTable();

    pluto2G4[1] = g4part->FindParticle("gamma");
    pluto2G4[2] = g4part->FindParticle("e+");
    pluto2G4[3] = g4part->FindParticle("e-");
    pluto2G4[4] = g4part->FindParticle("nu_e");
    pluto2G4[5] = g4part->FindParticle("mu+");
    pluto2G4[6] = g4part->FindParticle("mu-");
    pluto2G4[7] = g4part->FindParticle("pi0");
    pluto2G4[8] = g4part->FindParticle("pi+");
    pluto2G4[9] = g4part->FindParticle("pi-");
    pluto2G4[10] = g4part->FindParticle("kaon0L");
    pluto2G4[11] = g4part->FindParticle("kaon+");
    pluto2G4[12] = g4part->FindParticle("kaon-");
    pluto2G4[13] = g4part->FindParticle("neutron");
    pluto2G4[14] = g4part->FindParticle("proton");
    pluto2G4[15] = g4part->FindParticle("anti_proton");
    pluto2G4[16] = g4part->FindParticle("kaon0S");
    pluto2G4[17] = g4part->FindParticle("eta");

    pluto2G4[25] = g4part->FindParticle("anti_neutron");

    pluto2G4[41] = g4part->FindParticle("rho0");
    pluto2G4[42] = g4part->FindParticle("rho+");
    pluto2G4[43] = g4part->FindParticle("rho-");

    pluto2G4[45] = g4part->FindParticle("deuteron");
    pluto2G4[46] = g4part->FindParticle("triton");
    pluto2G4[47] = g4part->FindParticle("alpha");
    pluto2G4[49] = g4part->FindParticle("He3");

    pluto2G4[52] = g4part->FindParticle("omega");
    pluto2G4[53] = g4part->FindParticle("eta_prime");
    pluto2G4[55] = g4part->FindParticle("phi");
    pluto2G4[67] = g4part->FindParticle("J/psi");

    G4cout << "Particle table for conversion Pluto <=> Geant4:" << G4endl;
    for (std::map<G4int,G4ParticleDefinition*>::iterator it = pluto2G4.begin();
         it != pluto2G4.end(); it++) {
      G4cout << "Id " << it->first
             << " = " << it->second->GetParticleName() << G4endl;
    }

    fMCEvtID = 0;

}



A2PrimaryGeneratorAction::~A2PrimaryGeneratorAction()
{
	if(fGeneratedFile){
        for(Int_t i=0;i<100;i++) delete fGen4Vectors[i];
        //for(Int_t i=0;i<100;i++) delete fParticleDefinition[i];
        for(Int_t i=0;i<100;i++) delete fGenLorentzVec[i];
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
}

/**
 * @brief Generate a random point in the target
 * @return A point in the target
 */
G4ThreeVector A2PrimaryGeneratorAction::GetRandomVertex() {

    const double beamspotradius = 0.5 * cm;

    const double z   = fDetCon->GetTarget()->GetCenter().z() + fDetCon->GetTarget()->GetLength()/2 * (2*G4UniformRand()-1);
    const double phi = 2 * M_PI * G4UniformRand();
    const double u   = G4UniformRand() + G4UniformRand();
    const double r   = (u>1) ? 2-u : u;

    return G4ThreeVector(beamspotradius * r * sin(phi), beamspotradius * r * cos(phi), z);
}

G4ParticleDefinition* A2PrimaryGeneratorAction::PlutoIDToGeant( int pluto_id ) {
    return pluto2G4.at(pluto_id);
}


void A2PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    //Get the event from input tree
    fGenTree->GetEntry(fNevent++);

    //Set vertex position
    fThreeVector = GetRandomVertex();

    fGenPosition[0]=fThreeVector.x()/cm;
    fGenPosition[1]=fThreeVector.y()/cm;
    fGenPosition[2]=fThreeVector.z()/cm;

    fParticleGun->SetParticlePosition(fThreeVector);

    //Loop over tracked particles, set particle gun and create vertex for each particle

    int final_particles = 0;
    bool beam_found = false;

    fSimParticles.clear();

    for( int p=0; p< fGenParticles->GetEntries(); ++p) {

        PParticle* part = dynamic_cast<PParticle*>(fGenParticles->At(p));

        if( part->ID() < 1000 && part->GetDaughterIndex() == -1 ) {

            try {
                fParticleGun->SetParticleDefinition( PlutoIDToGeant( part->ID() ));
            } catch (...) {
                G4cerr << "Can't find G4 particle ID for " << part->Name() << G4endl;
                fParticleGun->SetParticleDefinition( 0 );
            }

            fParticleGun->SetParticleMomentumDirection(
                        G4ThreeVector(
                            part->Px()*GeV,
                            part->Py()*GeV,
                            part->Pz()*GeV
                    ).unit());
            fParticleGun->SetParticleEnergy( part->E()*GeV - part->M()*GeV);
            fParticleGun->GeneratePrimaryVertex(anEvent);

            final_particles++;

            fSimParticles.push_back(part);

        } else if ( part->ID() == 14001 ) {  // beam particle, compound of beam photon and tagert proton

            if( beam_found )
                G4cerr << "Warning: Multiple Beam Particles in Event" << G4endl;

            const double Eg = part->E() - 0.938272; // subtract proton mass

            fBeamLorentzVec = TLorentzVector( part->Vect().Unit()*Eg, Eg); // photon 4Vector
            beam_found = true;
        }
    }

    fNGenParticles = final_particles;

    if( !beam_found ) {
        fBeamLorentzVec = TLorentzVector(0,0,0,0);
    }

}

void A2PrimaryGeneratorAction::SetUpROOTInput(){
    fMode=EPGA_ROOT;

    fGeneratedFile = new TFile(fInFileName, "read");

    if(!fGeneratedFile || !(fGeneratedFile->IsOpen())) {
        G4cerr << "Cannot open input file '" << fInFileName << "'" << G4endl;
        G4cerr << "Need to call /A2/generator/InputFile or use --if=filename option on command line." << G4endl;
		exit(1);
	}

    //Get tree
    try {
        fGeneratedFile->GetObject("data", fGenTree);
    }
    catch (...) {
        G4cerr << "No TTree named 'data' found in " << fInFileName << G4endl;
        exit(1);
    }

    if( 0 != fGenTree->SetBranchAddress("Particles", &fGenParticles)) {
        G4cerr << "Can't set branch address for branch 'Particles' in TTree 'data' in file '" << fInFileName << "'" << G4endl;
        exit(1);
    }

    if( 0 != fGenTree->SetBranchAddress("plutoID", &fMCEvtID)) {
        G4cerr << "Can't set branch address for branch 'plutoID' in TTree 'data' in file '" << fInFileName << "'" << G4endl;
    }

    if( 0 != fGenTree->SetBranchAddress("plutoRandomID", &fMCRndID)) {
        G4cerr << "Can't set branch address for branch 'plutoRandomID' in TTree 'data' in file '" << fInFileName << "'" << G4endl;
    }

    G4cout << "Found " << fGenTree->GetEntries() << " PLUTO events in file." << G4endl;

    /// look for TID tree
    fGeneratedFile->GetObject("data_tid",TIDMessage::tidtree);

}
