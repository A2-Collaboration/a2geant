// SVN Info: $Id: TOMCParticle.h 546 2010-02-18 22:00:45Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOMCParticle                                                         //
//                                                                      //
// Class representing a particle in the MC simulation.                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef P2MKIN_TOMCParticle
#define P2MKIN_TOMCParticle

#include "TParticlePDG.h"
#include "TLorentzVector.h"
#include "TRandom.h"
#include "TGenPhaseSpace.h"
#include "TF1.h"

#include "TOGlobals.h"


// implemented decay modes
enum EParticleDecayMode {
    kNoDecay,                       // no decay
};
typedef EParticleDecayMode ParticleDecayMode_t;


class TOMCParticle : public TObject
{

private:
    TParticlePDG* fPDG;                         // PDG particle properties
    TLorentzVector* fP4;                        // 4-vector
    Int_t fNDecay;                              // number of decay particles
    TOMCParticle** fDecay;                      //[fNDecay] list of decay particles (this class is owner)
    ParticleDecayMode_t fDecayMode;             // decay mode
    TF1* fMassDistr;                            // particle mass distribution (e.g. for resonances) [MeV]

    void SetUpDecay(ParticleDecayMode_t inDecayMode);

public:
    TOMCParticle() : fPDG(0), fP4(0), fNDecay(0), fDecay(0), fDecayMode(kNoDecay), fMassDistr(0) { }
    TOMCParticle(TParticlePDG* inPDG, ParticleDecayMode_t inDecayMode, Bool_t initDecay = kTRUE);
    TOMCParticle(const Char_t* inName, ParticleDecayMode_t inDecayMode, Bool_t initDecay = kTRUE);
    TOMCParticle(Int_t inID, Bool_t isGeant3_ID, ParticleDecayMode_t inDecayMode, Bool_t initDecay = kTRUE);
    virtual ~TOMCParticle();

    Bool_t CalculateDecay();

    void SetP4(TLorentzVector* inP4) { if (fP4) *fP4 = *inP4; }
    void SetP4(Double_t inPx, Double_t inPy, Double_t inPz, Double_t inE) { if (fP4) fP4->SetPxPyPzE(inPx, inPy, inPz, inE); }
    void SetMassDistribution(TF1* inDistr) { fMassDistr = inDistr; }

    Int_t GetPDG_ID() const { return fPDG->PdgCode(); }
    Int_t GetGeant3_ID() const;
    const Char_t* GetName() const { return fPDG->GetName(); }
    Double_t GetPDGMassGeV() const { return fPDG->Mass(); }
    TLorentzVector* GetP4() const { return fP4; }
    ParticleDecayMode_t GetDecayMode() const { return fDecayMode; }
    Int_t GetTotalNDecayParticles() const;
    Int_t GetNDecayParticles() const { return fNDecay; }
    TOMCParticle* GetDecayParticle(Int_t index) const;
    TF1* GetMassDistribution() const { return fMassDistr; }

    ClassDef(TOMCParticle, 0)  // MC particle class
};

#endif

