#include <iostream>
#include <string>
#include <stdlib.h>
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "PParticle.h"
#include "TRandom2.h"
#include "TVector3.h"
#include <limits>

using namespace std;

TTree* tree = NULL;
TFile* file = NULL;
TRandom2 rnd;

#define NPART 7
const int particles[NPART] = { 1, 2, 3,  7, 8,  9, 14 };
//                             g e+ e- pi0 pi+ pi-  p

int RandomInt( const int max) {
    return floor(rnd.Uniform(max));
}

int GetRandomID() {
    return particles[ RandomInt(NPART) ];
}

int main( int argc, char** argv) {

    if( argc < 6 ) {
        cerr << argv[0] << " <outfile> <#events> <#particles> <PID (-1==random)> <Emax> [theta max (deg)]" << endl;
        exit(1);
    }

    PStaticData* db = makeStaticData();

    const string outfile(argv[1]);
    const UInt_t nevents = atoi(argv[2]);
    const UInt_t nparts = atoi(argv[3]);
    const int id = atoi(argv[4]);
    const double Emax = atof(argv[5]);

    const double max_theta = (argc==7) ? atof(argv[6]) * TMath::DegToRad() : std::numeric_limits<double>::infinity();

    file = new TFile( outfile.c_str(), "recreate");
    if( ! file || !file->IsOpen() ) {
        cerr << "Can't open output file " << outfile << endl;
        exit(1);
    }

    tree = new TTree("data","Random Particles");

    TClonesArray* particles = new TClonesArray("PParticle", nparts);
    particles->SetOwner(kTRUE);

    tree->Branch("Particles", particles);

    rnd.SetSeed();

    for( UInt_t i=0; i< nevents; ++i ) {

        particles->Clear();

        for( UInt_t j=0;j<nparts; ++j ) {

            int pID;
            double m;
            double E;

            if( id == -1 ) {
                pID = GetRandomID();
            } else {
                pID = id;
            }

            m = db->GetParticleMass(pID);

            E = rnd.Uniform(Emax)+m;

            double p = sqrt(E*E-m*m);

            TVector3 dir;
            do {
                rnd.Sphere(dir[0], dir[1], dir[2], p);
            } while (dir.Theta() > max_theta);

            PParticle* part = new PParticle( pID, dir);

            (*particles)[j] = part;
        }

        tree->Fill();

    }

    file->Write();

    file->Close();

    return 0;

}
