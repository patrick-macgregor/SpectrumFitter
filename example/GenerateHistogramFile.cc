




// Units are in keV
std::vector<double> mean = { 0, 200, 400, 600, 800, 1200 };
std::vector<double> amp = { 5.0, 4.0, 3.0, 2.0, 1.0, 2.0 };
int bin_size = 5;
double amp_bg = 1.0;
double width_bound = 50;
double width_unbound = 100;

const int N = 1e4;


// Generates a series of Gaussian peaks with a flat background
void GenerateHistogramFile(){
	TFile *f  = new TFile( "my_hist.root", "RECREATE" );
	TRandom *r_peak = new TRandom();
	TRandom *r_decide = new TRandom();
	TRandom *r_bg = new TRandom();

	// Create histogram
	double lb = mean.at(0) - width_bound*bin_size;
	double ub = mean.at( mean.size() - 1 ) + width_bound*bin_size;
	int nbins = ( ub - lb )/bin_size;

	TH1F* my_hist = new TH1F( "my_hist", Form( "Model spectrum; Energy (keV); Counts per %d keV", bin_size ), nbins, lb, ub );

	// Work out how to populate the histogram
	std::vector<double> amp_bnds;
	amp_bnds.resize( amp.size() );
	double freq_sum = 0.0;
	for ( int i = 0; i < amp.size(); ++i ){
		freq_sum += amp.at(i);
	}
	freq_sum += amp_bg;

	double temp_sum = 0;
	for ( int i = 0; i < amp_bnds.size(); ++i ){
		amp_bnds.at(i) = temp_sum + amp.at(i)/freq_sum;
		temp_sum = amp_bnds.at(i);
	}


	// Now generate events
	for ( int i = 0; i < N; ++i ){
		// Get the decision
		double decision = r_decide->Uniform();
		
		int index = amp.size();

		// Test the decision
		for ( int j = 0; j < amp_bnds.size(); ++j ){
			if ( decision < amp_bnds.at(j) ){
				index = j;
				break;
			}
		}

		// Now determine what to do
		if ( index == amp.size() ){
			// Background
			my_hist->Fill( r_bg->Uniform()*( ub - lb ) + lb );
		}
		else{
			my_hist->Fill( r_peak->Gaus( mean.at(index), ( index == amp.size() - 1 ? width_unbound : width_bound ) ) );
		}

	}

	// Write the hist to file
	my_hist->Write();
	f->Close();

	return;
}