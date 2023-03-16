#ifndef _INPUT_FILE_PROCESSOR_HH_
#define _INPUT_FILE_PROCESSOR_HH_

#include <iostream>
#include <TEnv.h>
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>
#include "Fit.hh"
#include "FitWriter.hh"
#include "MessageLogger.hh"
#include "Spectrum.hh"
#include "SpectrumFitter.hh"
#include "SpectrumDrawer.hh"

class InputFileProcessor{
public:
	// Constructor and destructor
	InputFileProcessor();
	~InputFileProcessor();

	// Setters
	inline void SetSpectrum( SFSpectrum *s ){ m_spec = s; }
	inline void SetSpectrumFitter( SFSpectrumFitter *sf ){ m_sf = sf; }
	inline void SetSpectrumDrawer( SFSpectrumDrawer *sd ){ m_sd = sd; }
	inline void SetFitWriter( SFFitWriter *fw ){ m_fw = fw; }
	inline void SetFileLocation( const TString s ){ m_input_file_location = s; }

	// Getters
	inline SFSpectrum* GetSpectrum() const { return m_spec; }
	inline SFSpectrumFitter* GetSpectrumFitter() const { return m_sf; }
	inline SFSpectrumDrawer* GetSpectrumDrawer() const { return m_sd; }
	inline SFFitWriter* GetFitWriter() const { return m_fw; }
	inline TString GetFileLocation() const { return m_input_file_location; }

	// Other functions
	void ProcessOptions();

private:
	TString m_input_file_location;	// Location of config file for specifying options
	SFSpectrum *m_spec;				// Pointer to the spectrum
	SFSpectrumFitter *m_sf;			// Pointer to the spectrum fitter object
	SFSpectrumDrawer *m_sd;			// Pointer to the spectrum drawer object
	SFFitWriter *m_fw;				// Pointer to the fit writer object
	MessageLogger *log = MessageLogger::GetInstance();	// Pointer to the logger class
};


#endif