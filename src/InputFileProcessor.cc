#include "InputFileProcessor.hh"

///////////////////////////////////////////////////////////////////////////////
InputFileProcessor::InputFileProcessor(){
	m_input_file_location = "";
	m_spec = nullptr;
	m_sf = nullptr;
	m_sd = nullptr;
	m_fw = nullptr;
	log->Construction("InputFileProcessor::InputFileProcessor() -- InputFileProcessor object constructed");
}
///////////////////////////////////////////////////////////////////////////////
InputFileProcessor::~InputFileProcessor(){
	log->Construction("InputFileProcessor::InputFileProcessor() -- InputFileProcessor object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
void InputFileProcessor::ProcessOptions(){
	// Create TEnv object for processing the input file
	TEnv *config = new TEnv( m_input_file_location.Data() );

	// Get the ROOT file
	TString s = (TString)config->GetValue( "ROOTFile", "" );
	if ( s == "" ){
		log->Error("Could not find \"ROOTFile\" in the input file. Please specify!");
	}

	// Open the TFile
	TFile *f = new TFile( s.Data() );

	// Test if the ROOT file opened
	if ( f->IsZombie() ){
		log->Error( Form( "File containing histogram(s) not found! Tried to open %s.", s.Data() ) );
	}

	// Get the histogram
	s = (TString)config->GetValue( "ROOTHistName", "" );
	TH1F *h = nullptr;

	// See if you can get the histogram
	if ( f->IsOpen() && !s.EqualTo("") ){
		h = (TH1F*)f->Get( s.Data() );
		h->SetDirectory(0); // Decouple from ROOT file
	}

	// Create the spectrum
	if ( h != nullptr ){
		if ( m_spec == nullptr ){
			log->Error("UNSPECIFIED");
		}
		else if ( m_spec->GetHist() == nullptr ){
			m_spec->SetHist( h );
		}
	}

	// Set the output file location for fit parameters
	if ( m_fw != nullptr ){
		m_fw->SetFileLocation( config->GetValue( "FitParameterFile", "FIT_PARAMETER_FILE.dat") );
	}
	else{
		log->Error("FitWriter object not initialised");
	}

	// SPECTRUM OPTIONS
	if ( m_spec != nullptr ){
		int number_of_peaks = config->GetValue( "NumberOfPeaks", 0 );
		m_spec->SetNumberOfFits( config->GetValue( "NumberOfFits", 0 ) );
		m_spec->SetNumberOfIntegrals( config->GetValue( "NumberOfIntegrals", 0 ) );
		m_spec->SetSeparationEnergy( config->GetValue( "SeparationEnergy", -1.0 ) );

		if ( m_spec->GetSeparationEnergy() == -1.0 ){
			log->Warning( "No separation energy specified...I have no way of knowing if the peaks are supposed to be unbound or not! Assuming all bound.");
		}
		
		// Set fit options
		int bg_dim = config->GetValue( "BackgroundDimension", 0 );
		for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
			SFFit *fit = m_spec->GetFit(i);
			fit->SetBGPolyOrder( bg_dim );
			fit->SetFitLimitLB( config->GetValue( Form( "%02d.FitLB", i ), -1.0 ) );
			fit->SetFitLimitUB( config->GetValue( Form( "%02d.FitUB", i ), -1.0 ) );
			
			// Background parameters
			for ( int j = 0; j <= bg_dim; ++j ){
				fit->SetBGPoly( j, config->GetValue( Form( "%02d.Background", j ), -1.0 ) );
				fit->SetBGPolyLB( j, config->GetValue( Form( "%02d.Background_LB", j ), -1.0 ) );
				fit->SetBGPolyUB( j, config->GetValue( Form( "%02d.Background_UB", j ), -1.0 ) );
				fit->SetBGPolyFixed( j, config->GetValue( Form( "%02d.Background_fixed", j ), false ) );
			}
		}

		// Set integral options
		for ( unsigned int i = 0; i < m_spec->GetNumberOfIntegrals(); ++i ){
			SFSpectrumIntegral *integral = m_spec->GetIntegral(i);
			integral->SetIntegralLB( config->GetValue( Form( "%02d.Integral_LB", i ), -1.0 ) );
			integral->SetIntegralUB( config->GetValue( Form( "%02d.Integral_UB", i ), -1.0 ) );

			if ( integral->GetIntegralLB() == integral->GetIntegralUB() ){
				log->Warning("Integral cannot have same LB as UB. The limits must be specified! Disregarding...");
			}
			else{
				double Y1 = config->GetValue( Form( "%02d.IntegralY1", i ), -1.0 );
				double Y2 = config->GetValue( Form( "%02d.IntegralY2", i ), -1.0 );

				if ( Y1 != -1.0 && Y2 != -1.0 ){
					// Background set, so calculate linear parameters
					integral->SetBackgroundFromCoordinates( true );
					integral->SetBGPolyOrder( 1 );
					double X2 = integral->GetIntegralUB();
					double X1 = integral->GetIntegralLB();
					integral->SetBGPoly( 0, ( Y1*X2 - Y2*X1 )/( X2 - X1 ) );
					integral->SetBGPoly( 1, ( Y2 - Y1 )/( X2 - X1 ) );
				}
				else{
					if ( Y1 != -1.0 && Y2 == -1.0 ){
						log->Warning("IntegralY2 not specified, so taking background from fit...");
					}
					else if ( Y1 == -1.0 && Y2 != -1.0 ){
						log->Warning("IntegralY1 not specified, so taking background from fit...");
					}
					
					// Set integral background from fit
					integral->SetBackgroundFromCoordinates( false );
					integral->SetBGPolyOrder( bg_dim );
					int parent_fit = config->GetValue( Form( "%02d.IntegralFitNumber", i ), -1 );
					if ( parent_fit < 0 || parent_fit > (int)m_spec->GetNumberOfFits() ){
						log->Warning("Selected fit outside range. Choosing the first fit as source of background.");
						parent_fit = 0;
					}
					integral->SetParentFit( m_spec->GetFit( parent_fit ) );

				}
			}
		}

		// Guesses for fitting
		m_spec->SetGuessWidth( config->GetValue( "GuessWidth", 100.0 ) );
		m_spec->SetGuessWidthLB( config->GetValue( "GuessWidth_LB", 50.0 ) );
		m_spec->SetGuessWidthUB( config->GetValue( "GuessWidth_UB", 150.0 ) );
		m_spec->SetGuessAmplitudeFractionLB( config->GetValue( "GuessAmplitudeFraction_LB", 0.0 ) );
		m_spec->SetGuessAmplitudeFractionUB( config->GetValue( "GuessAmplitudeFraction_UB", 1.2 ) );
		m_spec->SetGuessMeanHalfWidth( config->GetValue( "GuessMeanHalfWidth", 100.0 ) );
		
		// Bound peak widths
		m_spec->SetBoundPeakWidth( config->GetValue( "BoundPeakWidth", -1.0 ) );
		m_spec->SetBoundPeakWidthLB( config->GetValue( "BoundPeakWidth_LB", -1.0 ) );
		m_spec->SetBoundPeakWidthUB( config->GetValue( "BoundPeakWidth_UB", -1.0 ) );
		m_spec->SetFixedBoundPeakWidth( config->GetValue( "BoundPeakWidth_fixed", false ) );

		// Store peak options
		for ( int i = 0; i < number_of_peaks; ++i ){
			SFPeak *p = new SFPeak();

			p->SetAmplitude( config->GetValue( Form( "%02d.Amplitude", i ), -1.0 ) );
			p->SetAmplitudeLB( config->GetValue( Form( "%02d.Amplitude_LB", i ), -1.0 ) );
			p->SetAmplitudeUB( config->GetValue( Form( "%02d.Amplitude_UB", i ), -1.0 ) );
			p->SetFixedAmplitude( config->GetValue( Form( "%02d.Amplitude_fixed", i ), false ) );

			p->SetMean( config->GetValue( Form( "%02d.Mean", i ), -1.0 ) );
			if ( p->GetMean() == -1 ){
				log->Warning( Form( "Peak %02d did not have a mean assigned. Is this a mistake?", i ) );
			}
			p->SetMeanLB( config->GetValue( Form( "%02d.Mean_LB", i ), -1.0 ) );
			p->SetMeanUB( config->GetValue( Form( "%02d.Mean_UB", i ), -1.0 ) );
			p->SetFixedMean( config->GetValue( Form( "%02d.Mean_fixed", i ), false ) );

			// Decide if it's bound or unbound
			if ( m_spec->GetSeparationEnergy() == -1 || p->GetMean() < m_spec->GetSeparationEnergy() ){
				p->SetBound();
			}
			else{
				p->SetUnbound();
			}

			// Doublets
			if( config->GetValue( Form( "%02d.Doublet", i ), false ) )p->SetDoublet();

			// Fix widths of individual states
			p->SetFixedWidth( config->GetValue( Form( "%02d.Width_fixed", i ), false ) );
			if ( !p->HasFixedWidth() && p->IsBound() && !p->IsDoublet() ){
				// Bound non-doublet and decided to do width things -- print warnings
				// Warn user of setting bound fixed width options
				TString warning = "";
				TString suffix = "";
				bool print_warning = false;

				// Width
				if ( config->GetValue( Form( "%02d.Width", i ), -1e5 ) != -1e5 ){
					print_warning = true;
					warning.Append( Form( "%02d.Width", i ) );
				}

				// Width LB
				if ( config->GetValue( Form( "%02d.Width_LB", i ), -1e5 ) != -1e5 ){
					if ( print_warning ){ warning.Append(", "); suffix.Append("/"); }
					else{ print_warning = true; }
					warning.Append( Form( "%02d.Width_LB", i ) ); suffix.Append("_LB");
				}

				// Width UB
				if ( config->GetValue( Form( "%02d.Width_UB", i ), -1e5 ) != -1e5 ){
					if ( print_warning ){ warning.Append(", "); suffix.Append("/"); }
					else{ print_warning = true; }
					warning.Append( Form( "%02d.Width_UB", i ) ); suffix.Append("_UB");
				}

				// Width fixed
				if ( config->GetValue( Form( "%02d.Width_fixed", i ), -1e5 ) != -1e5 ){
					if ( print_warning ){ warning.Append(", "); suffix.Append("/"); }
					else{ print_warning = true; }
					warning.Append( Form( "%02d.Width_fixed", i ) ); suffix.Append("_fixed");
				}

				// Now print
				if (print_warning){
					log->Warning( Form( "%s specified in config file for bound state. This is being ignored, as all bound states share a common width. To fix the width of bound states, use the \"BoundPeakWidth%s\" keys.", warning.Data(), suffix.Data() ) );
				}
			}
			else{
				// Fixed bound non-doublet
				if ( p->HasFixedWidth() && p->IsBound() && !p->IsDoublet() ){
					log->Warning( "Fixing width of a bound state. Are you sure? Trying anyway...");
				}

				p->SetWidth( config->GetValue( Form( "%02d.Width", i ), -1.0 ) );
				p->SetWidthLB( config->GetValue( Form( "%02d.Width_LB", i ), -1.0 ) );
				p->SetWidthUB( config->GetValue( Form( "%02d.Width_UB", i ), -1.0 ) );
			} 
			
			m_spec->AddPeak(p);
		}
	}
	else{
		log->Error("Unable to read input file correctly");
	}

	// SPECTRUM FITTER OPTIONS
	if ( m_sf == nullptr ){
		log->Warning("SpectrumFitter not initialised!");
	}

	// SPECTRUM DRAWER OPTIONS
	if ( m_sd != nullptr ){
		m_sd->SetPrintPS( config->GetValue( "PrintPS", false ) );
		m_sd->SetPrintEPS( config->GetValue( "PrintEPS", false ) );
		m_sd->SetPrintPDF( config->GetValue( "PrintPDF", false ) );
		m_sd->SetPrintSVG( config->GetValue( "PrintSVG", false ) );
		m_sd->SetPrintTEX( config->GetValue( "PrintTEX", false ) );
		m_sd->SetPrintGIF( config->GetValue( "PrintGIF", false ) );
		m_sd->SetPrintXPM( config->GetValue( "PrintXPM", false ) );
		m_sd->SetPrintPNG( config->GetValue( "PrintPNG", false ) );
		m_sd->SetPrintJPG( config->GetValue( "PrintJPG", false ) );
		m_sd->SetPrintTIFF( config->GetValue( "PrintTIFF", false ) );
		m_sd->SetPrintCXX( config->GetValue( "PrintCXX", false ) );
		m_sd->SetPrintXML( config->GetValue( "PrintXML", false ) );
		m_sd->SetPrintJSON( config->GetValue( "PrintJSON", false ) );
		m_sd->SetPrintROOT( config->GetValue( "PrintROOT", false ) );

		m_sd->SetPrintFileName( config->GetValue( "PrintFileName", "myspectrum" ) );
		m_sd->SetCanvasWidth( config->GetValue( "CanvasWidth", 1200 ) );
		m_sd->SetCanvasHeight( config->GetValue( "CanvasHeight", 900 ) );
		m_sd->SetCanvasTitle( config->GetValue( "CanvasTitle", "TITLE" ) );
		m_sd->SetXAxisTitle( config->GetValue( "XAxisTitle", "X" ) );
		m_sd->SetYAxisTitle( config->GetValue( "YAxisTitle", "Y" ) );
		m_sd->SetXAxisLB( config->GetValue( "XAxisLB", 0.0 ) );
		m_sd->SetXAxisUB( config->GetValue( "XAxisUB", 0.0 ) );
		m_sd->SetXAxisLBDefined( config->Defined( "XAxisLB" ) );
		m_sd->SetXAxisUBDefined( config->Defined( "XAxisUB" ) );
		m_sd->SetInteractiveMode( config->GetValue( "InteractiveMode", false ) );
	}
	else{
		log->Warning("SpectrumDrawer not initialised!");
	}

	f->Close();
	delete f;
	delete config;
	return;
}


































