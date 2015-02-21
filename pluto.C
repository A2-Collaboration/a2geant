Double_t thetaCrit(double Egmax) {
    return 0.000510999 / Egmax;
}

void sim(Double_t Egmin, Double_t Egmax, bool saveIntermediate, bool enableBulk, Int_t nevents, char* file, char* reaction)
{
	gROOT->Reset();
	gRandom->SetSeed();

	// smear the beam
	PBeamSmearing *smear = new PBeamSmearing("beam_smear", "Beam Smearing");
	smear->SetReaction("g + p");

	TF1* tagger_spectrum = new TF1("bremsstrahlung","(1.0/x)", Egmin, Egmax);
	smear->SetMomentumFunction( tagger_spectrum );

	TF1* theta_smear = new TF1( "angle", "x / ( x*x + [0] )^2", 0.0, 5.0 * thetaCrit(Egmax) );
	theta_smear->SetParameter( 0, thetaCrit(Egmax) * thetaCrit(Egmax) );
    smear->SetAngularSmearing( theta_smear );

	makeDistributionManager()->Add(smear);

	PStaticData* sdata = makeStaticData();
	sdata->AddDecay("w --> eta + g", "w", "eta,g", 4.6E-4);
    cout << "Reaction: " << reaction << endl;

	PReaction reaction(Egmax, "g", "p", reaction, file, saveIntermediate, 0, 0, 0);

    if( enableBulk ) {
        PPlutoBulkDecay* p1 = new PPlutoBulkDecay();
        p1->SetRecursiveMode(1);
        p1->SetTauMax(0.001);
        reactrion.AddBulk(p1);
    }

	reaction.Loop(nevents);
}
