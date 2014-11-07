#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

// pluto++
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include "PParticle.h"
#include "PBeamSmearing.h"
#include "PReaction.h"
#include "PPlutoBulkDecay.h"
#include "PDataBase.h"

// ROOT
#include "TClonesArray.h"
#include "TMath.h"

#include "anyoption.h"

using namespace std;

// Options (default values)
static Double_t Egmax = 1.604;  // beam energy, max Tagg Energy [GeV]
static Double_t Egmin = 1.450;  // beam energy, min Tagg Energy [GeV]
static bool enableBulk = true;
static bool saveIntermediate = true;
static Int_t numEvents = 1000;
static string reactonString = "p omega";
static string prefix = "pluto_";
static string suffix = "00";
static string outfile = "";

static const Double_t me = 0.000510999;  // mass electron [GeV]

/**
 * @brief thetaCrit
 * @return
 */
inline Double_t thetaCrit() {
    return me / Egmax;
}

std::string generateFilename() {
    std::stringstream s;
    s << prefix;

    string fixedName(reactonString);
    std::replace( fixedName.begin(), fixedName.end(), ' ', '_');
    s << "-" << fixedName;

    if(enableBulk)
        s << "-bulk";

    s << "-" << suffix;

    return s.str();
}

void ReadCmdline(int argc, char** argv ) {

     AnyOption opt;

     opt.setVerbose();
     opt.autoUsagePrint(true);

     opt.addUsage(" --numEvents <n>           Number of events to simulate");
     opt.addUsage(" --saveIntermediate        Save intermedite particles");
     opt.addUsage(" --enableBulk              Enable bulk decay");

     opt.addUsage(" --reaction                Reactoin String, PLUTO notation");
     opt.addUsage(" --Emin                    Minimal photon energy [GeV]");
     opt.addUsage(" --Emax                    Maximal photon energy [GeV]");

     opt.addUsage(" --help -h");

     opt.setOption("numEvents");

     opt.setFlag("saveIntermediate");
     opt.setFlag("enableBulk");
     opt.setOption("Emin");
     opt.setOption("Emax");
     opt.setOption("prefix");
     opt.setOption("suffix");
     opt.setOption("reaction");
     opt.setOption("Output");

     opt.setFlag("help",'h');

 //    opt.processFile(CFG_FILE);

     opt.processCommandArgs( argc, argv );

     if( opt.getFlag('h') || opt.getValue("--help") ) {
          opt.printUsage();
          exit(0);
     }

     if ( opt.getValue("numEvents") )
         numEvents = atoi(opt.getValue("numEvents"));

     if ( opt.getValue("reaction") )
         reactonString = opt.getValue("reaction");

     if ( opt.getValue("prefix") )
         prefix = opt.getValue("prefix");

     if ( opt.getValue("suffix") )
         suffix = opt.getValue("suffix");

     if ( opt.getValue("Emin") )
         Egmin = atof(opt.getValue("Emin"));

     if ( opt.getValue("Emax") )
         Egmax = atof(opt.getValue("Emax"));
     if ( opt.getValue("Output") )
         outfile = opt.getValue("Output");


     saveIntermediate = ( NULL != opt.getValue("saveIntermediate") );
     enableBulk = ( NULL != opt.getValue("enableBulk") );

}

void PrintConfig() {
    cout << "\n\n Simulating " << numEvents << " events:\n\n";
    cout << "  Reactrion:  g p --> " << reactonString << "\n\n";
    cout << "  Photon Engery : " << Egmin << " to " << Egmax << " GeV\n\n";
    cout << "  Saving to " << generateFilename() << "\n\n";
    cout << "  saveIntermediate particles: ";
    if( saveIntermediate )
        cout << "yes";
    else
        cout << "no";
    cout << "\n\n";
    cout << "  enable Bulk decay: ";
    if( enableBulk )
        cout << "yes";
    else
        cout << "no";
    cout << "\n\n";

}

int main( int argc, char** argv ) {

    ReadCmdline( argc, argv );
    PrintConfig();

    PBeamSmearing *smear = new PBeamSmearing("beam_smear", "Beam smearing");
    smear->SetReaction("g + p");

    TF1* tagger_spectrum = new TF1("bremsstrahlung","(1.0/x)", Egmin, Egmax);
    smear->SetMomentumFunction( tagger_spectrum );

    TF1* theta_smear = new TF1( "angle", "x / ( x*x + [0] )^2", 0.0, 5.0 * thetaCrit() );
    theta_smear->SetParameter( 0, thetaCrit() * thetaCrit() );

    smear->SetAngularSmearing( theta_smear );

    makeDistributionManager()->Add(smear);

    PStaticData* sdata = makeStaticData();
    sdata->AddDecay("w --> eta + g", "w", "eta,g", 4.6E-4);

    if( outfile.empty() )
        outfile = generateFilename();

    // PReaction constructor requires non-const char*. so... make copies... ARGH!
    char* rs = strdup(reactonString.c_str());
    char* gf = strdup(outfile.c_str());

    cout << "filename " << gf << endl;

    PReaction reactrion(Egmax, "g", "p", rs, gf, saveIntermediate, 0, 0, 0);

    if( enableBulk ) {
        PPlutoBulkDecay* p1 = new PPlutoBulkDecay();
        p1->SetRecursiveMode(1);
        p1->SetTauMax(0.001);
        reactrion.AddBulk(p1);
    }

    reactrion.Print();   //The "Print()" statement is optional

    reactrion.Loop(numEvents);

    cout << "Simulation finished." << endl;

    free(rs);
    free(gf);

    return 0;
}

