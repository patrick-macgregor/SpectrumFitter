#include "Spectrum.hh"

///////////////////////////////////////////////////////////////////////////////
SFSpectrum::SFSpectrum(){
	m_hist = nullptr;
	m_list_of_peaks.resize(0);
	m_list_of_fits.resize(0);
	m_list_of_integrals.resize(0);

	m_separation_energy = -1;
	m_bound_width = -1;
	m_bound_width_lb = -1;
	m_bound_width_ub = -1;
	m_bound_width_fixed = false;

	m_guess_width = -1;
	m_guess_width_lb = -1;
	m_guess_width_ub = -1;
	m_guess_amplitude_fraction_lb = -1;
	m_guess_amplitude_fraction_ub = -1;
	m_guess_mean_half_width = -1;

	log->Construction("SFSpectrum::SFSpectrum -- SFSpectrum object constructed");

	return;
}
///////////////////////////////////////////////////////////////////////////////
SFSpectrum::~SFSpectrum(){
	// Remove all reference to the objects in memory
	if ( m_hist != nullptr && !m_hist->IsDestructed() )delete m_hist;

	// Free the peaks
	for ( unsigned int i = 0; i < this->GetNumberOfPeaks(); ++i ){
		delete this->GetPeak(i);
	}

	// Free the fits
	for ( unsigned int i = 0; i < this->GetNumberOfFits(); ++i ){
		delete this->GetFit(i);
	}

	// Free the integrals
	for ( unsigned int i = 0; i < this->GetNumberOfIntegrals(); ++i ){
		delete this->GetIntegral(i);
	}

	// Clear the vectors of everything
	m_list_of_peaks.clear();
	m_list_of_fits.clear();
	m_list_of_integrals.clear();

	log->Construction("SFSpectrum::~SFSpectrum -- SFSpectrum object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrum::SetNumberOfFits( const int n ){
	// Test if number is allowed
	if ( n < 0 ){
		log->Error("SFSpectrum::SetNumberOfFits -- number of fits must be positive!");
	}

	// Test if potential rewrite occurring
	if ( m_list_of_fits.size() > 0 ){
		log->Warning( "SFSpectrum::SetNumberOfFits -- Resizing list of fits with non-zero size..." );
	}
	m_list_of_fits.resize(n);

	for ( int i = 0; i < n; ++i ){
		SFFit *fit = new SFFit();
		fit->SetParentSpectrum(this);
		m_list_of_fits.at(i) = fit;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrum::SetNumberOfIntegrals( const int n ){
	// Test if number is allowed
	if ( n < 0 ){
		log->Error("SFSpectrum::SetNumberOfIntegrals -- number of integrals must be positive!");
	}

	if ( m_list_of_integrals.size() > 0 ){
		log->Warning( "SFSpectrum::SetNumberOfIntegrals -- Resizing list of integrals with non-zero size..." );
	}
	m_list_of_integrals.resize(n);

	for ( int i = 0; i < n; ++i ){
		SFSpectrumIntegral *integral = new SFSpectrumIntegral();
		integral->SetParentSpectrum(this);
		m_list_of_integrals.at(i) = integral;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrum::AddPeak( SFPeak* p ){
	// Add peak to the list
	m_list_of_peaks.push_back(p);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrum::CalculateNumberOfPeaksAndFitParameters(){
	// Loop over fits
	for ( unsigned int i = 0; i < this->GetNumberOfFits(); ++i ){
		SFFit *fit = this->GetFit(i);
		unsigned int num_pars = fit->GetBGPolyOrder() + 1 + 1; // Add bg pars and bound width which is always there...
		unsigned int num_peaks = 0;

		// Loop over peaks
		for ( unsigned int j = 0; j < this->GetNumberOfPeaks(); ++j ){
			SFPeak *peak = this->GetPeak(j);

			if ( peak->GetMean() >= fit->GetFitLimitLB() && peak->GetMean() < fit->GetFitLimitUB() ){
				// Add peak number to the fit
				fit->AddPeakNumber(j);
				num_peaks++;
				
				// Work out if peak is a doublet or unbound and add correct number of parameters...
				if ( peak->IsDoublet() || peak->IsUnbound() || peak->HasFixedWidth() ){
					num_pars += 3;	// Amplitude, width, mean
				}
				else{
					num_pars += 2;	// Amplitude, mean
				}
			}
		}

		// Now set the number of fit parameters for the fit
		fit->SetNumberOfFitParameters( num_pars );
		fit->SetNumberOfPeaks( num_peaks );

		// Now create map from parameter number to peak number
		unsigned int par_ctr = 0;

		for ( unsigned int i = 0; i < num_peaks; ++i ){
			SFPeak *peak = this->GetPeak( fit->GetPeakNumber(i) );
			
			// Set bound width parameter on first peak for convenience...
			if ( i == 0 ){
				fit->SetPeakNumberMap( par_ctr, -1 );
				par_ctr++;
			}
			
			// Set other properties
			if ( peak->IsDoublet() || peak->IsUnbound() || peak->HasFixedWidth() ){
				for ( unsigned int j = 0; j < 3; ++j ){
					fit->SetPeakNumberMap( par_ctr, fit->GetPeakNumber(i) );	// Width scale, amplitude, mean
					par_ctr++;
				}
			}
			else{
				for ( unsigned int j = 0; j < 2; ++j ){
					fit->SetPeakNumberMap( par_ctr, fit->GetPeakNumber(i) ); // Amplitude, mean
					par_ctr++;
				}
			}
		}

		for ( unsigned int i = 0; i <= fit->GetBGPolyOrder(); ++i ){
			fit->SetPeakNumberMap( par_ctr, i );
			par_ctr++;
		}

		// Check par_ctr = num_pars
		if ( par_ctr != num_pars ){
			log->Warning( Form( "SFSpectrum::CalculateNumberOfPeaksAndFitParameters -- Tried assigning parameters to peak numbers and didn't get the right numbers to match...num pars = %d whereas I counted %d?", num_pars, par_ctr ) );
		}

	}
	return;
}






