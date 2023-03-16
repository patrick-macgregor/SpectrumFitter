#include "SpectrumFitter.hh"

///////////////////////////////////////////////////////////////////////////////
// Constructor
SFSpectrumFitter::SFSpectrumFitter(){
	m_spec = nullptr;
	log->Construction("SFSpectrumFitter::SFSpectrumFitter -- SFSpectrumFitter object created");
}
///////////////////////////////////////////////////////////////////////////////
// Destructor
SFSpectrumFitter::~SFSpectrumFitter(){
	log->Construction("SFSpectrumFitter::~SFSpectrumFitter -- SFSpectrumFitter object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
// Initialise guesses for all the variables used in the fitting
void SFSpectrumFitter::InitialiseSpectrumGuesses(){
	// Access pointers
	SFPeak *p;
	SFFit *fit;
	TH1F* h = m_spec->GetHist();

	// Loop over peaks
	for ( unsigned int i = 0; i < m_spec->GetNumberOfPeaks(); ++i ){
		// Get the peak
		p = m_spec->GetPeak(i);

		if ( p->GetMean() < 0 ){
			log->Warning("Random peak alert!");
		}
		else{
			// Set guesses for all uninitialised variables
			if ( p->GetMeanLB() < 0 )p->SetMeanLB( TMath::Max( p->GetMean() - m_spec->GetGuessMeanHalfWidth(), 0.0 ) );
			if ( p->GetMeanUB() < 0 )p->SetMeanUB( p->GetMean() + m_spec->GetGuessMeanHalfWidth() );

			// Set width parameters
			if ( p->GetWidth() < 0 )p->SetWidth( m_spec->GetGuessWidth() );
			if ( p->GetWidthLB() < 0 )p->SetWidthLB( m_spec->GetGuessWidthLB() );
			if ( p->GetWidthUB() < 0 )p->SetWidthUB( m_spec->GetGuessWidthUB() );

			// Set bound peak width guesses too
			if ( m_spec->GetBoundPeakWidth() < 0 )m_spec->SetBoundPeakWidth( m_spec->GetGuessMeanHalfWidth() );
			if ( m_spec->GetBoundPeakWidthLB() < 0 )m_spec->SetBoundPeakWidthLB( m_spec->GetGuessWidthLB() );
			if ( m_spec->GetBoundPeakWidthUB() < 0 )m_spec->SetBoundPeakWidthUB( m_spec->GetGuessWidthUB() );

			if ( p->GetAmplitude() < 0 )p->SetAmplitude( h->GetBinContent( h->FindBin( p->GetMean() ) ) );
			if ( p->GetAmplitudeLB() < 0 )p->SetAmplitudeLB( m_spec->GetGuessAmplitudeFractionLB()*p->GetAmplitude() );
			if ( p->GetAmplitudeUB() < 0 )p->SetAmplitudeUB( m_spec->GetGuessAmplitudeFractionUB()*p->GetAmplitude() );

			// Check if peak has been set to unbound but mean is below separation energy
			if ( p->IsUnbound() && p->GetMean() < m_spec->GetSeparationEnergy() ){
				log->Warning( Form( "Peak %02d has been labelled unbound but has a mean (%8.4f) below the separation energy (%8.4f)", i, p->GetMean(), m_spec->GetSeparationEnergy() ) );
			}
			
		}
	}

	// Fit-specific
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		fit = m_spec->GetFit(i);

		for ( unsigned int j = 0; j <= fit->GetBGPolyOrder(); ++j ){
			if ( fit->GetBGPoly(j) == -1.0 ){
				if ( j == 0 )fit->SetBGPoly( j, 2.0 );
				else fit->SetBGPoly( j, 0.0 );
			}
			if ( fit->GetBGPolyLB(j) == -1.0 ){
				if ( j == 0 )fit->SetBGPolyLB( j, -0.0001 );
				else fit->SetBGPolyLB( j, -1e6 );
			}
			if ( fit->GetBGPolyUB(j) == -1.0 ){
				fit->SetBGPolyUB( j, 1e6 );
			}
		}
		if ( fit->GetFitLimitLB() < -5000 )fit->SetFitLimitLB( h->GetBinLowEdge(1) );
		if ( fit->GetFitLimitUB() < -5000 )fit->SetFitLimitUB( h->GetBinLowEdge(1 + h->GetNbinsX() ) );


	}

	CheckForFitParameterGuessErrors();
	log->Debug("SFSpectrumFitter::CheckForFitParameterGuessErrors -- Checked for parameter guess errors");
	m_spec->CalculateNumberOfPeaksAndFitParameters();
	log->Debug("SFSpectrumFitter::CalculateNumberOfPeaksAndFitParameters -- Assigned number of peaks and fit parameters to SFFit objects in the spectrum");

	// Print guesses for all peaks
	log->Debug("SFSpectrumFitter::InitialiseSpectrumGuesses -- PEAK SPECTRUM READY FOR FITTING WITH THE FOLLOWING GUESSES");
	log->Debug( Form( "NumberOfPeaks:        %d", m_spec->GetNumberOfPeaks() ) );
	log->Debug( Form( "NumberOfFits:         %d", m_spec->GetNumberOfFits() ) );
	log->Debug( Form("SeparationEnergy:   %8.4f", m_spec->GetSeparationEnergy() ) );
	for ( unsigned int i = 0; i < m_spec->GetNumberOfPeaks(); ++i ){
		p = m_spec->GetPeak(i);
		log->Debug( Form( "%02d.Mean:            %8.4f", i, p->GetMean() ) );
		log->Debug( Form( "%02d.Mean_LB:         %8.4f", i, p->GetMeanLB() ) );
		log->Debug( Form( "%02d.Mean_UB:         %8.4f", i, p->GetMeanUB() ) );
		log->Debug( Form( "%02d.Amplitude:       %8.4f", i, p->GetAmplitude() ) );
		log->Debug( Form( "%02d.Amplitude_LB:    %8.4f", i, p->GetAmplitudeLB() ) );
		log->Debug( Form( "%02d.Amplitude_UB:    %8.4f", i, p->GetAmplitudeUB() ) );
		log->Debug( Form( "%02d.Width:           %8.4f", i, p->GetWidth() ) );
		log->Debug( Form( "%02d.Width_LB:        %8.4f", i, p->GetWidthLB() ) );
		log->Debug( Form( "%02d.Width_UB:        %8.4f", i, p->GetWidthUB() ) );
		log->Debug( Form( "%02d.Doublet:         %d", i, p->IsDoublet() ) );
		log->Debug( Form( "%02d.Unbound:         %d", i, p->IsUnbound() ) );
		log->Debug( Form( "%02d.Mean_fixed:      %d", i, p->HasFixedMean() ) );
		log->Debug( Form( "%02d.Width_fixed:     %d", i, p->HasFixedWidth() ) );
		log->Debug( Form( "%02d.Amplitude_fixed: %d", i, p->HasFixedAmplitude() ) );
	}
	log->Debug("");
	
	// Print fit info
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit *fit = m_spec->GetFit(i);
		log->Debug( Form( "FIT %d", i ) );
		log->Debug( Form( "    BackgroundDimension:   %d", fit->GetBGPolyOrder() ) );
		log->Debug( Form( "    FitLB:                 %8.4f", fit->GetFitLimitLB() ) );
		log->Debug( Form( "    FitUB:                 %8.4f", fit->GetFitLimitUB() ) );
		//log->Debug( Form( "    FitBound:              %d", fit->IsBound() ) );
		for ( unsigned int j = 0; j <= fit->GetBGPolyOrder(); ++j ){
			log->Debug( Form( "    %02d.Background:       %8.4f", j, fit->GetBGPoly(j) ) );
			log->Debug( Form( "    %02d.Background_LB:    %8.4f", j, fit->GetBGPolyLB(j) ) );
			log->Debug( Form( "    %02d.Background_UB:    %8.4f", j, fit->GetBGPolyUB(j) ) );
			log->Debug( Form( "    %02d.Background_fixed: %d", j, fit->IsBGPolyFixed(j) ) );
			log->Debug( Form( "    NumberOfPeaks:         %d", fit->GetNumberOfPeaks() ) );
			log->Debug( Form( "    NumberOfParameters:    %d", fit->GetNumberOfFitParameters() ) );
		}
	}
	log->Debug("");

	// Print integral info
	for ( unsigned int i = 0; i < m_spec->GetNumberOfIntegrals(); ++i ){
		SFSpectrumIntegral *integral = m_spec->GetIntegral(i);
		log->Debug( Form( "INTEGRAL %d", i ) );
		log->Debug( Form( "    %02d.Integral_LB:             %8.4f", i, integral->GetIntegralLB() ) );
		log->Debug( Form( "    %02d.Integral_UB:             %8.4f", i, integral->GetIntegralUB() ) );
		log->Debug( Form( "    %02d.IntegralFromCoordinates: %d", i, integral->IsBackgroundFromCoordinates() ) );
	}

	log->Debug("");
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::CheckForFitParameterGuessErrors(){

	// LOOP over the number of peaks in the spectrum
	for ( unsigned int i = 0; i < m_spec->GetNumberOfPeaks(); ++i ){
		SFPeak *peak = m_spec->GetPeak(i);

		// Are any peaks excluded from the fits?
		bool print_warning = 1;
		for ( unsigned int j = 0; j < m_spec->GetNumberOfFits(); ++j ){
			SFFit *fit = m_spec->GetFit(j);
			if ( peak->GetMean() >= fit->GetFitLimitLB() && peak->GetMean() < fit->GetFitLimitUB() ){
				print_warning = 0;
			}
		}
		if ( print_warning ){
			log->Warning( Form( "Peak %02d is not included in any fits", i ) );
		}

		// Are all of the bound peaks below the separation energy?
		if ( peak->IsBound() && peak->GetMean() > m_spec->GetSeparationEnergy() ){
			log->Warning( Form( "Peak %02d is labelled bound but has energy above the separation energy (%7.2f)", i, m_spec->GetSeparationEnergy() ) );
		}

		// Are any parameters or bounds negative?
		if ( peak->GetAmplitude() <= 0 ){
			log->Warning( Form( "Peak %02d has an amplitude <= 0...", i ) );
		}
		if ( peak->GetMean() < 0 ){
			log->Warning( Form( "Peak %02d has a mean < 0...", i ) );
		}
		if ( peak->GetWidth() <= 0 ){
			log->Warning( Form( "Peak %02d has a width <= 0...", i ) );
		}
		if ( peak->GetAmplitudeLB() < 0 ){
			log->Warning( Form( "Peak %02d has an amplitude lower bound < 0...(%7.2f)", i, peak->GetAmplitudeLB() ) );
		}
		if ( peak->GetMeanLB() < 0 ){
			log->Warning( Form( "Peak %02d has a mean lower bound < 0...(%7.2f)", i, peak->GetMeanLB() ) );
		}
		if ( peak->GetWidthLB() < 0 ){
			log->Warning( Form( "Peak %02d has a width lower bound < 0...(%7.2f)", i, peak->GetWidthLB() ) );
		}
		if ( peak->GetAmplitudeUB() < 0 ){
			log->Warning( Form( "Peak %02d has an amplitude upper bound < 0...(%7.2f)", i, peak->GetAmplitudeUB() ) );
		}
		if ( peak->GetMeanUB() < 0 ){
			log->Warning( Form( "Peak %02d has a mean upper bound < 0...(%7.2f)", i, peak->GetMeanUB() ) );
		}
		if ( peak->GetWidthUB() < 0 ){
			log->Warning( Form( "Peak %02d has a width upper bound < 0...(%7.2f)", i, peak->GetWidthUB() ) );
		}

		// Are any UBs less than LBs?
		if ( peak->GetWidthUB() < peak->GetWidthLB() ){
			log->Warning( Form( "Peak %02d has a width lower bound (%7.2f) < width upper bound (%7.2f)...", i, peak->GetWidthLB(), peak->GetWidthUB() ) );
		}
		if ( peak->GetAmplitudeUB() < peak->GetAmplitudeLB() ){
			log->Warning( Form( "Peak %02d has a amplitude lower bound (%7.2f) < amplitude upper bound (%7.2f)...", i, peak->GetAmplitudeLB(), peak->GetAmplitudeUB() ) );
		}
		if ( peak->GetMeanUB() < peak->GetMeanLB() ){
			log->Warning( Form( "Peak %02d has a mean lower bound (%7.2f) < mean upper bound (%7.2f)...", i, peak->GetMeanLB(), peak->GetMeanUB() ) );
		}

		// Do bound and unbound spectrum overlap LBs UBs?
		if ( peak->IsBound() && peak->GetMeanUB() > m_spec->GetSeparationEnergy() ){
			log->Warning( Form( "Peak %02d has an upper bound above the separation energy...", i ) );
		}
		if ( peak->IsUnbound() && peak->GetMeanLB() < m_spec->GetSeparationEnergy() ){
			log->Warning( Form( "Peak %02d has a lower bound below the separation energy...", i ) );
		}

		// Are any of the means within each other to a certain threshold i.e. are any states on top of each other?
		double peak_overlap_threshold = 100;
		for ( unsigned int j = 0; j < i; ++j ){
			SFPeak *peak2 = m_spec->GetPeak(j);
			if ( TMath::Abs( peak->GetMean() - peak2->GetMean() ) < peak_overlap_threshold ){
				log->Warning( Form( "Peaks %02d and %02d are within %4.1f keV of each other!", j, i, peak_overlap_threshold ) );
			}
		}
	} // Loop over peaks

	// Other checks
	// Is separation energy negative?
	if ( m_spec->GetSeparationEnergy() <= 0 ){
		log->Warning( "Separation energy < 0" );
	}

	// Is BG 0th order poly par >= 0?
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit *fit = m_spec->GetFit(i);
		if ( fit->GetBGPoly(0) < -0.0001 ){
			log->Warning( Form( "Flat portion of background below 0...(%7.2f) in fit %d", fit->GetBGPoly(0), i ) );
		}
		if ( fit->GetBGPolyLB(0) < 0 ){
			log->Warning( Form( "Flat portion of background lower bound below 0...(%7.2f) in fit %d", fit->GetBGPolyLB(0), i ) );
		}
		if ( fit->GetBGPolyUB(0) < 0 ){
			log->Warning( Form( "Flat portion of background upper bound below 0...(%7.2f) in fit %d", fit->GetBGPolyUB(0), i ) );
		}

		// Loop over background order
		for ( unsigned int j = 0; j <= fit->GetBGPolyOrder(); ++j ){
			// Check background overlaps
			if ( fit->GetBGPolyUB(i) < fit->GetBGPolyLB(i) ){
				log->Warning( Form( "Background parameter %02d has a lower bound (%7.2f) < upper bound (%7.2f)...", j, fit->GetBGPolyLB(j), fit->GetBGPolyUB(j) ) );
			}
		
			// Check BG limits
			if ( fit->GetBGPolyUB(j) == 1e6 && ( ( j == 0 && fit->GetBGPolyLB(j) != 0.0 ) || ( j > 0 && fit->GetBGPolyLB(j) != -1e6 ) ) ){
				log->Warning( Form("Must have background LB and UB both set to impose limits. Only LB set here... (order %02d)", j ) );
			}
			else if ( fit->GetBGPolyUB(j) != 1e6 && ( ( j == 0 && fit->GetBGPolyLB(j) == -0.0001 ) || ( j > 0 && fit->GetBGPolyLB(j) == -1e6 ) ) ){
				log->Warning( Form("Must have background LB and UB both set to impose limits. Only UB set here... (order %02d)", j ) );
			}
		}
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::CheckForFitParameterValueErrors( SFFit* fit ){
	// Get it to shut up
	fit->GetNumberOfPeaks();

	// TODO

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::GenerateInitialFits(){
	// Loop over fit number
	TF1* fit_func;
	SFFit* fit;
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		fit = m_spec->GetFit(i);
		TString fit_func_string = fit->GenerateTotalFitString( 0 );
		fit_func = new TF1( Form( "%d_FitFunc", i ), fit_func_string, fit->GetFitLimitLB(), fit->GetFitLimitUB() );
		fit->SetFit( fit_func );

		log->Debug( Form( "SFSpectrumFitter::GenerateInitialFits -- %d fit string: %s", i, fit_func_string.Data() ) );

		// Generate individual fits too
		for ( unsigned int j = 0; j < fit->GetNumberOfPeaks(); ++j ){
			fit_func = new TF1( Form( "%d_FitFuncInd_%02d", i, j ), fit->GenerateTotalFitString( 1 ), fit->GetFitLimitLB(), fit->GetFitLimitUB() );
			fit->SetIndividualFit( j, fit_func );
		}
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::SetFittingOptions(){
	// Loop over the fits
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit *fit = m_spec->GetFit(i);
		TF1* fit_func = fit->GetFit();

		// Now set/fix all of the parameters
		for ( unsigned int j = 0; j < fit->GetNumberOfFitParameters(); ++j ){
			// Get peak info for this parameter
			SFPeak *p;
			int peak_num = fit->GetPeakNumberMap(j);
			SFFit::FitParameterType type = fit->GetFitParameterType(j);

			// Set the bound-fit width
			if ( j == 0 && peak_num == -1 && type == SFFit::FitParameterWidth ){
				if ( m_spec->HasFixedBoundPeakWidth() ){
					fit_func->FixParameter( j, m_spec->GetBoundPeakWidth() );
				}
				else{
					fit_func->SetParameter( j, m_spec->GetBoundPeakWidth() );
					fit_func->SetParLimits( j, m_spec->GetBoundPeakWidthLB(), m_spec->GetBoundPeakWidthUB() );
				}
			}

			// Start assigning values
			if ( peak_num >= 0 ){
				p = m_spec->GetPeak( peak_num );

				// WIDTHS
				if ( type == SFFit::FitParameterWidth ){
					// Should only be setting these if a doublet or unbound
					if ( p->IsDoublet() || p->IsUnbound() || p->HasFixedWidth() ){
							fit_func->FixParameter( j, p->GetWidth() );
					}
					else{
						log->Warning("Parameter labelled width does not satisfy the requirements...");
					}
				}
				// WIDTH SCALING FACTORS
				// Shouldn't have any fixed widths here...
				if ( type == SFFit::FitParameterWidthScale ){
					if ( p->HasFixedWidth() ){
						log->Warning("Parameter labelled width-scale does not satisfy the requirements...");
					}
					else{
						fit_func->SetParameter( j, 1.01 );
						fit_func->SetParLimits( j, 1.0, 3.0 );
					}
				}
				// AMPLITUDES
				else if ( type == SFFit::FitParameterAmplitude ){
					if ( p->HasFixedAmplitude() ){
						fit_func->FixParameter( j, p->GetAmplitude() );
					}
					else{
						fit_func->SetParameter( j, p->GetAmplitude() );
						fit_func->SetParLimits( j, p->GetAmplitudeLB(), p->GetAmplitudeUB() );
					}
				}
				// MEANS
				else if ( type == SFFit::FitParameterMean){
					if ( p->HasFixedMean() ){
						fit_func->FixParameter( j, p->GetMean() );
					}
					else{
						fit_func->SetParameter( j, p->GetMean() );
						fit_func->SetParLimits( j, p->GetMeanLB(), p->GetMeanUB() );
					}
				}
				// BACKGROUNDS
				else if ( type == SFFit::FitParameterBackground ){
					// Peak number denotes the order of the parameter
					if ( fit->IsBGPolyFixed(peak_num) ){
						fit_func->FixParameter( j, fit->GetBGPoly(peak_num) );
					}
					else{
						fit_func->SetParameter( j, fit->GetBGPoly(peak_num) );
						fit_func->SetParLimits( j, fit->GetBGPolyLB(peak_num), fit->GetBGPolyUB(peak_num) );
					}
				}
			}
			// Names
			TString num = ( fit->GetPeakNumberMap(j) >= 0 ? Form( "%02d-", fit->GetPeakNumberMap(j) ) : "" );
			TString name = "fail";
			if ( type == SFFit::FitParameterWidth ){
				name = "width";
			}
			else if ( type == SFFit::FitParameterWidthScale ){
				name = "wscale";
			}
			else if ( type == SFFit::FitParameterAmplitude ){
				name = "amp";
			}
			else if ( type == SFFit::FitParameterMean ){
				name = "mean";
			}
			else if ( type == SFFit::FitParameterBackground ){
				name = "bg";
			}
			else if ( type == SFFit::FitParameterNULL ){
				name = "null";
			}
			fit_func->SetParName( j, num + name );

		} // Loop over fit parameters

	} // Loop over fits
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::UpdateSpectrumWithFitParameters( SFFit *fit ){
	
	// Prepare to store covariance terms between amplitudes and widths
	std::vector< std::vector< int > > cov_index_amp_wid;
	cov_index_amp_wid.resize( fit->GetNumberOfPeaks() );
	TFitResultPtr fit_result = fit->GetFitResultPtr();

	for ( unsigned int j = 0; j < fit->GetNumberOfPeaks(); ++j ){
		cov_index_amp_wid.at(j).resize(2);
		cov_index_amp_wid.at(j).at(0) = -1;
		cov_index_amp_wid.at(j).at(1) = -1;
	}

	// Loop over fit parameters
	for ( unsigned int j = 0; j < fit_result->NPar(); ++j ){
		SFFit::FitParameterType type = fit->GetFitParameterType(j);
		int peak_num = fit->GetPeakNumberMap(j);
		SFPeak *peak = m_spec->GetPeak(peak_num);
		bool null_peak_flag = ( peak == nullptr );

		// Store the value based on type
		if ( type == SFFit::FitParameterWidth ){
			if ( j == 0 ){
				// Set the values for the global width
				for ( unsigned int k = 0; k < fit->GetNumberOfPeaks(); ++k ){
					m_spec->GetPeak( fit->GetPeakNumber(k) )->SetWidth( fit_result->Parameter(j) );
					m_spec->GetPeak( fit->GetPeakNumber(k) )->SetWidthErr( fit_result->ParError(j) );
					m_spec->GetPeak( fit->GetPeakNumber(k) )->SetLimitedWidth( IsParameterAtLimit(j, fit_result) );
					cov_index_amp_wid.at(k).at(0) = j;
				}
			}
			// Custom width -- set the values of a custom width
			else{
				if ( !null_peak_flag ){
					peak->SetWidth( fit_result->Parameter(j) );
					peak->SetWidthErr( fit_result->ParError(j) );
					peak->SetLimitedWidth( IsParameterAtLimit(j, fit_result) );
					cov_index_amp_wid.at(peak_num).at(0) = j;
				}
				else{
					log->Warning("Trying to access peak with weird peak number and weird properties...investigate further!");
				}
			}
		}
		else if ( type == SFFit::FitParameterWidthScale && !null_peak_flag ){
			peak->SetWidth( fit_result->Parameter(j)*fit_result->Parameter(0) );
			peak->SetWidthErr( fit_result->Parameter(j)*fit_result->Parameter(0)*TMath::Sqrt( TMath::Power( fit_result->ParError(j)/fit_result->Parameter(j), 2 ) + TMath::Power( fit_result->ParError(0)/fit_result->Parameter(0), 2 ) + 2*fit_result->CovMatrix(0,j)/( fit_result->Parameter(j)*fit_result->Parameter(0) ) ) );
			peak->SetLimitedWidth( IsParameterAtLimit(j, fit_result) );
		}
		else if ( type == SFFit::FitParameterMean && !null_peak_flag ){
			peak->SetMean( fit_result->Parameter(j) );
			peak->SetMeanErr( fit_result->ParError(j) );
			peak->SetLimitedMean( IsParameterAtLimit(j, fit_result) );
		}
		else if ( type == SFFit::FitParameterAmplitude && !null_peak_flag ){
			peak->SetAmplitude( fit_result->Parameter(j) );
			peak->SetAmplitudeErr( fit_result->ParError(j) );
			peak->SetLimitedAmplitude( IsParameterAtLimit(j, fit_result) );
			cov_index_amp_wid.at(peak_num).at(1) = j;
		}
		else if ( type == SFFit::FitParameterBackground ){
			fit->SetBGPoly( peak_num, fit_result->Parameter(j) );
			fit->SetBGPolyErr( peak_num, fit_result->ParError(j) );
			fit->SetBGPolyLimited( peak_num, IsParameterAtLimit(j, fit_result) );

			// Covariance matrix terms for background integrals...
			for ( unsigned int k = 0; k < j; ++k ){
				if( fit->GetFitParameterType(k) == SFFit::FitParameterBackground ){
					fit->SetBGCovMatrix( fit->GetPeakNumberMap(j), fit->GetPeakNumberMap(k), fit_result->CovMatrix( j, k ) );
				}
			}
			
		}
		else{
			log->Warning("Trying to access peak with weird peak number and weird properties...investigate further!");
		}

	} // Loop over parameters

	// Now calculate areas and store them too
	double sqrt2pi = TMath::Sqrt( TMath::TwoPi() );
	double cov = 0;

	for ( unsigned int j = 0; j < fit->GetNumberOfPeaks(); ++j ){
		SFPeak *peak = m_spec->GetPeak( fit->GetPeakNumber(j) );

		// Check area has not already been set...
		if ( peak->GetArea() >= 0 ){
			log->Warning( Form( "Peak %02d area has already been set -- perhaps by another fit? Will overwrite...", j ) );
		}

		peak->SetArea( peak->GetAmplitude()*peak->GetWidth()*sqrt2pi/m_spec->GetHist()->GetBinWidth(0) );

		// Calculate error
		if ( cov_index_amp_wid.at(j).at(0) == -1 || cov_index_amp_wid.at(j).at(1) == -1 ){
			log->Warning( "Covariant term in area error calculation is not going to work here...");
		}
		else{
			cov = fit_result->CovMatrix( cov_index_amp_wid.at(j).at(0), cov_index_amp_wid.at(j).at(1) );
			peak->SetAreaErr( peak->GetArea()*TMath::Sqrt( TMath::Power( peak->GetAmplitudeErr()/peak->GetAmplitude(), 2 ) + TMath::Power( peak->GetWidthErr()/peak->GetWidth(), 2 ) + 2*cov/( peak->GetWidth()*peak->GetAmplitude() ) ) );
		}

	} // Loop over peaks

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumFitter::FitPeaks(){
	// Fit the histogram (bound)
	// 0 -> don't draw fit automatically
	// S -> return TFitResultPtr
	// L -> hist represents counts, so fits better when empty bins are present. Log-likelihood method rather than chi-squared...
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit *fit = m_spec->GetFit(i);
		TFitResultPtr r = m_spec->GetHist()->Fit( fit->GetFit(), "0SL" );
		fit->SetFitResultPtr(r);

		log->Debug( Form( "SFSpectrumFitter::FitPeaks -- Fitted spectrum with guessed parameters (fit %d)", i ) );

		// Store the fit parameters in the spectrum + peak objects
		UpdateSpectrumWithFitParameters( fit );
		log->Debug("SFSpectrumFitter::UpdateSpectrumWithFitParameters -- Updated peaks with information from fit");

		// Check the bound fit parameters
		CheckForFitParameterValueErrors(fit);
		log->Debug("SFSpectrumFitter::CheckForFitParameterValueErrors -- Checked parameter values for any questionable properties");


		// Copy the bound pars to individual bound fits
		TF1* ind_fit = nullptr;;

		// Loop over the bound peaks in the spectrum
		for ( unsigned int j = 0; j < fit->GetNumberOfPeaks(); ++j ){
			ind_fit = fit->GetIndividualFit(j);
			SFPeak *peak = m_spec->GetPeak( fit->GetPeakNumber(j) );

			ind_fit->FixParameter( 0, peak->GetWidth() );
			ind_fit->FixParameter( 1, peak->GetAmplitude() );
			ind_fit->FixParameter( 2, peak->GetMean() );

			ind_fit->SetParName( 0, Form( "%02d-width", j ) );
			ind_fit->SetParName( 0, Form( "%02d-amp", j ) );
			ind_fit->SetParName( 0, Form( "%02d-mean", j ) );

			for ( unsigned int k = 0; k <= fit->GetBGPolyOrder(); ++k ){
				ind_fit->FixParameter( 3+k, fit->GetBGPoly(k) );
				ind_fit->SetParName( 3+k, Form( "%02d-bg", k ) );
			}
		}
		log->Debug("SFSpectrumFitter::FitPeaks -- Created the individual fits");

	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
int SFSpectrumFitter::IsParameterAtLimit( int par_num, TFitResultPtr r ){
	double lb, ub, par_value;
	double threshold = 1e-6;
	r->ParameterBounds( par_num, lb, ub );
	par_value = r->Parameter( par_num );
	if ( TMath::Abs( par_value - lb ) < threshold )return 1;
	if ( TMath::Abs( ub - par_value ) < threshold )return 2;
	return 0;
}

void SFSpectrumFitter::CalculateIntegrals(){
	// Loop over integrals
	for ( unsigned int i = 0; i < m_spec->GetNumberOfIntegrals(); ++i ){
		SFSpectrumIntegral *integral = m_spec->GetIntegral(i);

		// Now assign background parameters to the integral objects now that fitting has been done
		if( !integral->IsBackgroundFromCoordinates() ){
			SFFit *fit = integral->GetFit();
			if ( fit == nullptr ){
				log->Warning("SFSpectrumFitter::CalculateIntegrals -- Background not being set by fit here!");

				// Test if the background parameters are sensible
				bool questionable_bg = false;
				for ( unsigned int j = 0; j <= integral->GetBGPolyOrder(); ++j ){
					if ( integral->GetBGPoly(j) == -1.0 ){
						questionable_bg = true;

						if ( questionable_bg ){
							log->Warning("SFSpectrumFitter::CalculateIntegrals -- I don't believe your background is sensible, so I'm going to say your integral is zero for now. Please amend input to get a better integral...");
							//integral->SetIntegral(-1.0);
							//integral->SetIntegralErr(-1.0);
							return;
						}
					}
				}
			}

			// Assign with a background fit - make sure they have the same order!
			if ( fit->GetBGPolyOrder() != integral->GetBGPolyOrder() ){
				log->Warning("Parent fit and integral have different orders of background...this should have been fixed earlier");
			}
			
			for ( unsigned int j = 0; j <= fit->GetBGPolyOrder(); ++j ){
				integral->SetBGPoly( j, fit->GetBGPoly(j) );
				integral->SetBGPolyErr( j, fit->GetBGPolyErr(j) );

				// Covariance matrix
				for ( unsigned int k = 0; k < j; ++k ){
					integral->SetBGCovMatrix(j,k, fit->GetBGCovMatrix(j,k) );
				}
			}
		}

		// Now calculate the integrals
		integral->CalculateIntegral();
	}

	return;
}