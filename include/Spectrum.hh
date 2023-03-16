// Class to hold information about a series of peaks, as well as point to a particular histogram
#ifndef _SPECTRUM_HH_
#define _SPECTRUM_HH_

#include <vector>
#include <TH1F.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TMath.h>
#include <TObject.h>
#include "Fit.hh"
#include "MessageLogger.hh"
#include "Peak.hh"
#include "SpectrumIntegral.hh"

// Need to forward declare this as it refers 
class SFFit;
class SFSpectrumIntegral;


class SFSpectrum : public TObject{

public:
	// Enum for parameter type
	enum FitParameterType : unsigned char {
		FitParameterNULL = 0, FitParameterWidth, FitParameterAmplitude, FitParameterMean, FitParameterBackground
	};

	// Constructor/Destructor
	SFSpectrum();
	~SFSpectrum();

	// Getters
	inline TH1F* GetHist() const { return m_hist; }
	inline SFPeak* GetPeak( const int n ) const {
		if ( n < 0 || n > (int)m_list_of_peaks.size() )return nullptr;
		return m_list_of_peaks.at(n);
	}
	inline SFFit* GetFit( const int n ) const {
		if ( n < 0 || n > (int)m_list_of_fits.size() )return nullptr;
		return m_list_of_fits.at(n);
	}
	inline SFSpectrumIntegral* GetIntegral( const int n ) const {
		if ( n < 0 || n > (int)m_list_of_integrals.size() )return nullptr;
		return m_list_of_integrals.at(n);
	}

	inline unsigned int GetNumberOfPeaks() const { return m_list_of_peaks.size(); }
	inline unsigned int GetNumberOfFits() const { return m_list_of_fits.size(); }
	inline unsigned int GetNumberOfIntegrals() const { return m_list_of_integrals.size(); }

	inline double GetSeparationEnergy() const { return m_separation_energy; }

	inline double GetGuessWidth() const { return m_guess_width; }
	inline double GetGuessWidthLB() const { return m_guess_width_lb; }
	inline double GetGuessWidthUB() const { return m_guess_width_ub; }
	inline double GetGuessAmplitudeFractionLB() const { return m_guess_amplitude_fraction_lb; }
	inline double GetGuessAmplitudeFractionUB() const { return m_guess_amplitude_fraction_ub; }
	inline double GetGuessMeanHalfWidth() const { return m_guess_mean_half_width; }
	
	inline double GetBoundPeakWidth() const { return m_bound_width; }
	inline double GetBoundPeakWidthLB() const { return m_bound_width_lb; }
	inline double GetBoundPeakWidthUB() const { return m_bound_width_ub; }
	inline bool HasFixedBoundPeakWidth() const { return m_bound_width_fixed; }

	// Setters
	inline void SetHist( TH1F* h ){ m_hist = h; }
	inline void SetSeparationEnergy( const double x ){ m_separation_energy = x; }
	void SetNumberOfFits( const int n );
	void SetNumberOfIntegrals( const int n );

	inline void SetGuessWidth( const double x ){ m_guess_width = x; }
	inline void SetGuessWidthLB( const double x ){ m_guess_width_lb = x; }
	inline void SetGuessWidthUB( const double x ){ m_guess_width_ub = x; }
	inline void SetGuessAmplitudeFractionLB( const double x ){ m_guess_amplitude_fraction_lb = x; }
	inline void SetGuessAmplitudeFractionUB( const double x ){ m_guess_amplitude_fraction_ub = x; }
	inline void SetGuessMeanHalfWidth( const double x ){ m_guess_mean_half_width = x; }
	
	inline void SetBoundPeakWidth( const double x ){ m_bound_width = x; }
	inline void SetBoundPeakWidthLB( const double x ){ m_bound_width_lb = x; }
	inline void SetBoundPeakWidthUB( const double x ){ m_bound_width_ub = x; }
	inline void SetFixedBoundPeakWidth( const bool x ){ m_bound_width_fixed = x; }


	// Other functions
	void AddPeak( SFPeak* p );
	int GetNumberOfPeaksInRange( double lb, double ub ) const;
	void CalculateNumberOfPeaksAndFitParameters();

private:
	// Hist and fit pointers
	TH1F* m_hist;
	std::vector <SFPeak*> m_list_of_peaks;
	std::vector <SFFit*> m_list_of_fits;
	std::vector <SFSpectrumIntegral*> m_list_of_integrals;

	double m_separation_energy;
	double m_bound_width;
	double m_bound_width_lb;
	double m_bound_width_ub;
	bool m_bound_width_fixed;

	double m_guess_width;
	double m_guess_width_lb;
	double m_guess_width_ub;
	double m_guess_amplitude_fraction_lb;
	double m_guess_amplitude_fraction_ub;
	double m_guess_mean_half_width;

	MessageLogger *log = MessageLogger::GetInstance();

	ClassDef(SFSpectrum, 0);
};




#endif