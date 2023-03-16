#ifndef _SPECTRUM_FITTER_HH_
#define _SPECTRUM_FITTER_HH_

#include <vector>
#include <TCanvas.h>
#include <TMath.h>
#include <TString.h>
#include "Fit.hh"
#include "MessageLogger.hh"
#include "Spectrum.hh"

class SFSpectrumFitter{
public:
	SFSpectrumFitter();
	~SFSpectrumFitter();

	void InitialiseSpectrumGuesses();
	void GenerateInitialFits();
	void SetFittingOptions();
	void FitPeaks();
	void CalculateIntegrals();
	void SaveFitsToFile();
	void PrintFitCanvas();
	void PrintFitTerminal();

	// Getters
	inline SFSpectrum* GetSpectrum(){ return m_spec; }

	// Setters
	inline void SetSpectrum( SFSpectrum* s){ m_spec = s; }

private:
	SFSpectrum *m_spec;

	// Private FUNCTIONS
	MessageLogger *log = MessageLogger::GetInstance();
	void CheckForFitParameterGuessErrors();
	void CheckForFitParameterValueErrors( SFFit* fit );
	void UpdateSpectrumWithFitParameters( SFFit* fit );
	int IsParameterAtLimit( int par_num, TFitResultPtr r );
	
};

#endif