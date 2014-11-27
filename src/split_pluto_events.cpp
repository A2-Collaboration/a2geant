#include <iostream>

#include <list>

#include "PParticle.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"
#include <cstdlib>
#include <time.h>

using namespace std;



class EventSplitter {
private:
    TTree* output_tree;
    TFile* output_file;
    TClonesArray* output_branch;
    Long64_t    eventid;
    Long64_t   randomid;
    UInt_t      partindex;
    Long64_t    original_eventid;
    Long64_t   original_randomid;

public:
    EventSplitter();
    virtual ~EventSplitter() { CloseOutput(); }

    bool IsLeaf( const PParticle* p );
    bool IsValid( const PParticle* p );
    bool IsPion( const PParticle* p );

    void ProcessFile( const string &file );
    void ProcessEvent( TClonesArray* event );
    bool OpenOutput( const string &file );
    void CloseOutput();



};

EventSplitter::EventSplitter():
    output_tree(NULL),
    output_file(NULL),
    output_branch(NULL),
    eventid(0),
    randomid(0),
    partindex(0),
    original_eventid(-1),
    original_randomid(0)
{
    srand(time(NULL));
}

bool EventSplitter::IsLeaf( const PParticle *p ) {
    return p->GetDaughterIndex() == -1;
}

bool EventSplitter::IsValid( const PParticle *p ) {
    return (p->ID() < 1000);
}

bool EventSplitter::IsPion(const PParticle *p) {
    return (p->ID() == 7 || p->ID() ==8 || p->ID() ==9 );
}

void EventSplitter::ProcessFile(const string &file) {

    TFile* infile = new TFile(file.c_str(),"read");

    if( !infile || !(infile->IsOpen())) {
        cerr << "Can't open " << file << endl;
        return;
    }

    TTree* data = NULL;

    try {
        infile->GetObject("data", data);
    } catch (...) {
        cerr << "No data tree in " << file << endl;
        infile->Close();
        delete infile;
    }

    cout << file << " :" << data->GetEntries() << " events in file" << endl;

    TClonesArray* arr = NULL;

    data->SetBranchAddress("plutoID", &original_eventid);
    data->SetBranchAddress("plutoRandomID", &original_randomid);

    if( 0 == data->SetBranchAddress("Particles", &arr) ) {

        for( Long64_t i = 0; i < data->GetEntries(); ++i ) {
            data->GetEntry(i);
            ProcessEvent(arr);
        }

    } else {
        cerr << "Branch not found!" << endl;
    }

    infile->Close();
    delete infile;
}

bool EventSplitter::OpenOutput(const string &file) {

    output_file = new TFile(file.c_str(),"recreate");

    if( !output_file || !output_file->IsOpen() ) {
        cerr << "Can't create output file " << file << endl;
        return false;
    }

    output_tree = new TTree("data","1 particle/event");

    output_branch = new TClonesArray("PParticle", 1);

    output_tree->Branch("Particles", output_branch);
    output_tree->Branch("plutoID", &eventid);
    output_tree->Branch("plutoRandomID", &randomid);
    output_tree->Branch("particleIndex", &partindex);
    output_tree->Branch("original_plutoID", &original_eventid);
    output_tree->Branch("original_plutoRandomID", &original_randomid);

    return true;

}

void EventSplitter::CloseOutput() {
    if( output_tree && output_file ) {
        cout << output_tree->GetEntries() << " events in output" << endl;
        output_file->cd();
        output_tree->Write();
        output_file->Write();
        output_file->Close();
        delete output_file;
        delete output_branch;
    }

}

void EventSplitter::ProcessEvent(TClonesArray *event) {

    for( partindex=0; partindex<event->GetEntries(); ++partindex) {

        PParticle* particle = NULL;
        try {
            particle = dynamic_cast<PParticle*>(event->At(partindex));
        } catch (...) {
            cerr << "invalid particle found!" << endl;
            exit(2);
        }

         if( IsValid(particle) && IsLeaf(particle) ) {
             (*output_branch)[0] = particle;
             randomid = rand();
             output_tree->Fill();
             eventid++;
         }
    }
}

int main(int argc, char** argv ) {

    if(argc < 3) {
        cerr << argv[0] << " file1 [file2] [file3] ... output" << endl;
        exit(2);
    }

    EventSplitter splitter;

    if (!splitter.OpenOutput( argv[argc-1] ) )
        exit(1);

    for( unsigned int f = 1; f < argc-1; ++f )
        splitter.ProcessFile(argv[f]);
    return 0;
}
