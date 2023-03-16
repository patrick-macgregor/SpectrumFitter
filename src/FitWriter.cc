#include "FitWriter.hh"

///////////////////////////////////////////////////////////////////////////////
SFFitWriter::SFFitWriter(){
	m_file_location = "";
	m_spec = nullptr;
	log->Construction("SFFitWriter::SFFitWriter -- SFFitWriter object constructed");
}
///////////////////////////////////////////////////////////////////////////////
SFFitWriter::~SFFitWriter(){
	log->Construction("SFFitWriter::~SFFitWriter -- SFFitWriter object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
void SFFitWriter::WriteFits(){

	// Open the output file
	if ( m_file_location == "" ){
		log->Error("File location for fit parameter output file needs to be set!");
	}
	m_output_file.open( m_file_location );

	// Check the file opened
	if ( !m_output_file.is_open() ){
		log->Error( Form( "Failed attempt at opening fit parameter output file at %s", m_file_location.Data() ) );
	}

	// Write a header
	m_output_file << std::left << 
		std::setw(m_item_width) << "Peak num" << "\t" <<
		std::setw(m_item_width) << "Amplitude" << "\t" <<
		std::setw(m_item_width) << "Error" << "\t" <<
		std::setw(m_item_width) << "Sigma" << "\t" <<
		std::setw(m_item_width) << "Error" << "\t" <<
		std::setw(m_item_width) << "Mean" << "\t" <<
		std::setw(m_item_width) << "Error" << "\t" <<
		std::setw(m_item_width) << "Area" << "\t" <<
		std::setw(m_item_width) << "Error" << "\t" <<
		std::setw(m_item_width) << "Info" << "\t" <<
		std::endl;

	// Write peak information
	unsigned int num_peak = 0;
	unsigned int num_integral = 0;
	bool write_integrals = true;
	bool write_peaks = true;
	SFPeak *peak = nullptr;
	SFSpectrumIntegral *integral = nullptr;

	while ( num_peak + num_integral < m_spec->GetNumberOfPeaks() + m_spec->GetNumberOfIntegrals() ){
		// Check if we should break
		if ( !write_peaks && !write_integrals ){
			log->Warning("Could not print everything for some reason?");
			break;
		}

		// Test whether safe to get integral and peak objects
		if ( num_peak >= m_spec->GetNumberOfPeaks() )write_peaks = false;
		if ( num_integral >= m_spec->GetNumberOfIntegrals() )write_integrals = false;

		// Get pointers to objects safely
		if ( write_peaks ){
			peak = m_spec->GetPeak(num_peak);
		}
		else{
			peak = nullptr;
		}
		
		if ( write_integrals ){
			integral = m_spec->GetIntegral(num_integral);
		}
		else{
			integral = nullptr;
		}

		// Write objects to file
		if ( peak != nullptr ){
			if ( integral != nullptr ){
				// Check which one is bigger and write that
				if ( peak->GetMean() < integral->GetCentroid() ){
					WritePeakInformation(peak,num_peak);
					num_peak++;
					if ( num_peak >= m_spec->GetNumberOfPeaks() )write_peaks = false;
				}
				else{
					WriteIntegralInformation( integral, num_integral );
					num_integral++;
					if ( num_integral >= m_spec->GetNumberOfIntegrals() )write_integrals = false;
				}
			}
			else{
				// No integrals, but still have peaks
				WritePeakInformation(peak,num_peak);
				num_peak++;
				if ( num_peak >= m_spec->GetNumberOfPeaks() )write_peaks = false;
			}
		}
		else{
			// All peaks done, but not all integrals
			if ( integral != nullptr ){
				WriteIntegralInformation( integral, num_integral );
				num_integral++;
				if ( num_integral >= m_spec->GetNumberOfIntegrals() )write_integrals = false;
			}
		}
	}


	// Add supplementaries
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit *fit = m_spec->GetFit(i);
		m_output_file << std::left << "Fit " << i << " peaks:\t[";
		for ( unsigned int j = 0; j < fit->GetNumberOfPeaks(); ++j ){
			m_output_file << j << ( j < fit->GetNumberOfPeaks() - 1 ? ", " : "]\t" );
		}
		m_output_file << "Background:\t";
		for ( unsigned int j = 0; j <= fit->GetBGPolyOrder(); ++j ){
			m_output_file << std::setprecision(m_item_width) << std::setw(m_item_width) << fit->GetBGPoly(j) << "\t" << std::setw(m_item_width) << fit->GetBGPolyErr(j) << "\t" << std::setw(m_item_width) << fit->GetBGInfoString() << "\t";
		}
		m_output_file << "Red. chi-sq." << "\t" << std::setprecision(6) << std::setw(m_item_width) << fit->GetReducedChiSquared() << std::endl;
		
		if ( !fit->GetFitResultPtr()->IsValid() ){
			m_output_file << std::left;
			unsigned int num_cols = 10;
			for ( unsigned int j = 0; j < num_cols; ++j ){
				if ( j != 5 ) m_output_file << std::setw(m_item_width) << "***  ***";
				else m_output_file << std::setw(m_item_width) << "FIT INVALID";

				if ( j < num_cols - 1 ){ m_output_file << "\t"; }
			}
			m_output_file << std::endl;
		}
	}

	// Close the file
	m_output_file.close();
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFitWriter::WritePeakInformation( SFPeak* peak, unsigned int i ){
	m_output_file << std::left << 
		std::setw(m_item_width) << Form( "P.%02d", i ) << "\t" <<
		std::setw(m_item_width) << peak->GetAmplitude() << "\t" <<
		std::setw(m_item_width) << peak->GetAmplitudeErr() << "\t" <<
		std::setw(m_item_width) << peak->GetWidth() << "\t" <<
		std::setw(m_item_width) << peak->GetWidthErr() << "\t" <<
		std::setw(m_item_width) << peak->GetMean() << "\t" <<
		std::setw(m_item_width) << peak->GetMeanErr() << "\t" <<
		std::setw(m_item_width) << peak->GetArea() << "\t" <<
		std::setw(m_item_width) << peak->GetAreaErr() << "\t" <<
		std::setw(m_item_width) << peak->GetStatus() << "\t" <<
		std::endl;
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFitWriter::WriteIntegralInformation( SFSpectrumIntegral *integral, unsigned int i ){
	m_output_file << std::left << 
		std::setw(m_item_width) << Form( "I.%02d", i ) << "\t" <<
		std::setw(m_item_width) << "" << "\t" <<
		std::setw(m_item_width) << "" << "\t" <<
		std::setw(m_item_width) << integral->GetIntegralLB() << "\t" <<
		std::setw(m_item_width) << integral->GetIntegralUB() << "\t" <<
		std::setw(m_item_width) << integral->GetCentroid() << "\t" <<
		std::setw(m_item_width) << integral->GetCentroidErr() << "\t" <<
		std::setw(m_item_width) << integral->GetIntegral() << "\t" <<
		std::setw(m_item_width) << integral->GetIntegralErr() << "\t" <<
		std::setw(m_item_width) << integral->GetStatus() << "\t" <<
		std::endl;
	return;
}