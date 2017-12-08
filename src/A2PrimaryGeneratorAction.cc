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

#include "G4ios.hh"



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

    //default phase space limits
    fTmin         = 0;
    fTmax         = 400*MeV;
    fThetamin     = 0;
    fThetamax     = 180*deg;
    fBeamXSigma   = 0.5*cm;
    fBeamYSigma   = 0.5*cm;
    // do not set the target center here, not used anyway
    // use it instead to determine if a target center was specified via /A2/generator/SetTargetZ0
    //fTargetZ0     =   0*cm;
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

    fTrackThis = NULL;

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

    // only desitred final state particles here:
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
    pluto2G4[19] = g4part->FindParticle("sigma+");
    pluto2G4[25] = g4part->FindParticle("anti_neutron");
    pluto2G4[45] = g4part->FindParticle("deuteron");

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

    double z;
    // check if target center was changed, otherwise use target center
    const double z0 = std::isfinite(fTargetZ0) ? fTargetZ0 : fDetCon->GetTarget()->GetCenter().z();
    if(fTargetThick == 0.0) {
        z = z0;
    } else {
        const double l = fTargetThick >= 0.0 ? fTargetThick : fDetCon->GetTarget()->GetLength();
        z = z0 + l/2 * (2*G4UniformRand()-1);
    }

    const double phi = 2 * M_PI * G4UniformRand();
    const double u   = G4UniformRand() + G4UniformRand();
    const double r   = (u>1) ? 2-u : u;

    return G4ThreeVector(fTargetRadius * r * sin(phi), fTargetRadius * r * cos(phi), z);
}

G4ParticleDefinition* A2PrimaryGeneratorAction::PlutoIDToGeant( int pluto_id ) const {
    return pluto2G4.at(pluto_id);
}

bool A2PrimaryGeneratorAction::AllPIDsOK(TClonesArray* PlutoArray) const {

    for( int p=0; p< PlutoArray->GetEntries(); ++p) {

        PParticle* part = dynamic_cast<PParticle*>(PlutoArray->At(p));

        if(!part) {
            G4cerr << "Could not cast particle to TParticle. Is this a Pluto Tree??" << G4endl;
            return false;
        }

        if(part->GetDaughterIndex() == -1) {

            try {
                PlutoIDToGeant( part->ID() );
            } catch (...) {
                G4cerr << "Can't find G4 particle ID for final state particle " << part->Name() << G4endl;
                return false;
            }
        }
    }

    return true;
}

void A2PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    //Get the event from input tree
    fGenTree->GetEntry(fNevent);

    //Set vertex position
    const G4ThreeVector primaryVertex = GetRandomVertex();

    fGenPosition[0]=primaryVertex.x()/cm;
    fGenPosition[1]=primaryVertex.y()/cm;
    fGenPosition[2]=primaryVertex.z()/cm;

    TLorentzVector beamVector(0,0,0,0);



    //Loop over tracked particles, set particle gun and create vertex for each particle

    int final_particles = 0;
    bool beam_found = false;

    fSimParticles.clear();

    if(AllPIDsOK(fGenParticles)) {

        for( int p=0; p< fGenParticles->GetEntries(); ++p) {

            PParticle* part = dynamic_cast<PParticle*>(fGenParticles->At(p));

            const G4int PID = part->ID();
            if( PID < 1000 && part->GetDaughterIndex() == -1 ) {

                fParticleGun->SetParticleDefinition( PlutoIDToGeant( part->ID() ));

                fParticleGun->SetParticleMomentumDirection(
                            G4ThreeVector(
                                part->Px()*GeV,
                                part->Py()*GeV,
                                part->Pz()*GeV
                        ).unit());
                fParticleGun->SetParticleEnergy( part->E()*GeV - part->M()*GeV);

                // vertex position in pluto is in mm, no conversion needed
                const G4ThreeVector particleVertex = primaryVertex
                                                       + G4ThreeVector(part->X(),
                                                                       part->Y(),
                                                                       part->Z());


                fParticleGun->SetParticlePosition(particleVertex);

                fParticleGun->GeneratePrimaryVertex(anEvent);

                final_particles++;

                fSimParticles.push_back(part);

            } else if (PID > 1000) {  // beam pseudo-particle, compound of beam photon and tagert particle

                if( beam_found )
                    G4cerr << "Warning: Multiple Beam Particles in Event" << G4endl;

                const G4int beamID   = GetPlutoBeamID(PID);
                const G4int targetID = GetPlutoTargetID(PID);

                if(beamID != 1)
                    G4cerr << "Warning: Beam id is " << beamID << " Expecting photon beam for A2-simulation!" << G4endl;

                try {
                    const double Eg = part->E() - PlutoIDToGeant(targetID)->GetPDGMass()/1000.0 ; // subtract target mass
                    beamVector = TLorentzVector( part->Vect().Unit()*Eg, Eg); // photon 4Vector
                } catch (...) {
                    G4cerr << "Warning: Can't find G4 particle ID for target-particle " << part->Name() << G4endl;
                }

                beam_found = true;
            }
        }
    } else {
        G4cerr << "Skipping Event " << fNevent << G4endl;
    }

    fNGenParticles = final_particles;

    fBeamLorentzVec = beamVector;


    fNevent++;

}

void A2PrimaryGeneratorAction::SetUpROOTInput(){
    fMode=EPGA_ROOT;

    if (fInFileName == TString("")) {
        G4cout << "No input file given, assume interactive mode." << G4endl;
        G4cout << "Need to call /A2/generator/InputFile or use --if=filename option on command line for batch mode." << G4endl;
        return;
    }

    fGeneratedFile = new TFile(fInFileName, "read");

    if(!fGeneratedFile || !(fGeneratedFile->IsOpen())) {
        G4cerr << "Cannot open input file '" << fInFileName << "'" << G4endl;
        exit(1);
    }

    // get data tree
    fGeneratedFile->GetObject("data", fGenTree);
    // tree found?
    if (!fGenTree) {
        G4cerr << "No TTree 'data' found in" << fInFileName << G4endl;
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
