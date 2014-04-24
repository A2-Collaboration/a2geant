// SVN Info: $Id: TOMCMkinWriter.h 447 2009-06-12 18:32:25Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008-2009
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOMCMkinWriter                                                       //
//                                                                      //
// Writer for MC kinematics ROOT files using the CB mkin file format.   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef P2MKIN_TOMCMkinWriter
#define P2MKIN_TOMCMkinWriter

#include "TNtuple.h"
#include "TLorentzVector.h"
#include "TFile.h"

#include "TOGlobals.h"
#include "TOMCParticle.h"


class TOMCMkinWriter : public TObject
{

private:
    TVector3* fVertex;                          // Reaction vertex [cm]
    TLorentzVector* fBeamP4;                    // beam 4-vector [GeV]
    TOMCParticle** fParticle;                   // list of all particles (not owner)
    UInt_t fNParticle;                          // number of all particles
    Char_t fOutFileName[256];                   // output file name
    TFile* fFile;                               // output ROOT file
    TNtuple* fNtuple;                           // output ntuple
    Float_t* fBuffer;                           // output data buffer
    UInt_t fNEvents;                            // number of stored events

public:
    TOMCMkinWriter() : fVertex(0), fBeamP4(0), fParticle(0), fNParticle(0), 
                       fFile(0), fNtuple(0), fBuffer(0), fNEvents(0) { }
    TOMCMkinWriter(const Char_t* inOutFileName);
    virtual ~TOMCMkinWriter();

    void Initialize(TOMCParticle** inParticle, UInt_t inNParticle);
    void SaveEvent();
    void WriteFile(TList* inList = 0);

    Char_t* GetOutputFileName() { return fOutFileName; }
    
    void SetVertex(Double_t inX, Double_t inY, Double_t inZ);
    void SetBeamP4(TLorentzVector* inBeamP4);
    void SetBeamP4(Double_t inPx, Double_t inPy, Double_t inPz, Double_t inE);

    ClassDef(TOMCMkinWriter, 0)  // Mkin kinematics file writer
};

#endif

