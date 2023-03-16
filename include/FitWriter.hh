#ifndef _FIT_WRITER_H_
#define _FIT_WRITER_H_

#include <iomanip>
#include <iostream>
#include <vector>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TString.h>
#include "MessageLogger.hh"
#include "SpectrumIntegral.hh"
#include "Spectrum.hh"

const double ConvertWidthToFWHM = 2*TMath::Sqrt( 2*TMath::Log(2) );

class SFFitWriter{
public:
	SFFitWriter();
	~SFFitWriter();

	void WriteFits();

	inline TString GetFileLocation() const { return m_file_location; }
	inline void SetFileLocation( const TString file_location ){ m_file_location = file_location; };
	inline void SetSpectrum( SFSpectrum *spec ){ m_spec = spec; }

private:
	TString m_file_location;
	std::ofstream m_output_file;
	SFSpectrum *m_spec;
	MessageLogger *log = MessageLogger::GetInstance();
	int m_item_width = 10;

	void WritePeakInformation( SFPeak* peak, unsigned int i );
	void WriteIntegralInformation( SFSpectrumIntegral *integral, unsigned int i );
};

#endif