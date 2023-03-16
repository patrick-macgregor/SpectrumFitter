// Class to hold information about a fit to a (portion of a) spectrum
#ifndef _FIT_HH_
#define _FIT_HH_

#include "MessageLogger.hh"
#include "Peak.hh"
#include "Spectrum.hh"
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TObject.h>
#include <iostream>
#include <vector>

// Need to forward-declare this because SFFit refers to parent spectrum...
class SFSpectrum;

class SFFit : public TObject{
public:
	// Enum for parameter type
	enum FitParameterType : unsigned char {
		FitParameterNULL = 0, FitParameterWidth, FitParameterWidthScale, FitParameterAmplitude, FitParameterMean, FitParameterBackground
	};

	// Constructor/destructor
	SFFit();
	~SFFit();


	// Getters
	double GetReducedChiSquared() const;
	double GetCommonWidth() const;
	TString GetBGInfoString() const;

	TF1* GetIndividualFit( const unsigned int n) const;
	FitParameterType GetFitParameterType(const unsigned int n) const;
	double GetBGCovMatrix( unsigned int i, unsigned int j ) const;


	// Setters
	void SetIndividualFit(const unsigned int n, TF1* fit);
	void SetFitParameterType(const unsigned int n, const FitParameterType type);
	void SetBGCovMatrix( unsigned int i, unsigned int j, const double val );

	void SetNumberOfPeaks( const int n);
	void SetNumberOfFitParameters( const int n );
	void SetBGPolyOrder( const int p );


	// Inline getters
	inline unsigned int GetNumberOfPeaks() const { return m_number_of_peaks; }
	inline unsigned int GetNumberOfFitParameters() const { return m_number_of_parameters; }

	inline TF1* GetFit() const { return m_fit; }
	inline TFitResultPtr GetFitResultPtr() const { return m_fit_result; }
	inline double GetFitLimitLB() const { return m_fit_limit_lb; }
	inline double GetFitLimitUB() const { return m_fit_limit_ub; }
	
	inline unsigned int GetBGPolyOrder() const { return m_background_polynomial_level; }

	inline double GetBGPoly( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_value ); }
	inline double GetBGPolyErr( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_err ); }
	inline double GetBGPolyLB( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_lb ); }
	inline double GetBGPolyUB( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_ub ); }
	inline bool IsBGPolyFixed( const unsigned int n ) const { return this->GetBGQuantity<bool>( n, m_bg_fixed ); }
	inline int IsBGPolyLimited( const unsigned int n ) const { return this->GetBGQuantity<int>( n, m_bg_limit ); }

	inline int GetPeakNumberMap( const unsigned int n ){ return m_parameter_number_to_peak_number_map.at(n); }
	inline int GetPeakNumber( const unsigned int n ){ return m_list_of_peak_numbers.at(n); }

	// Inline setters
	inline void SetFit( TF1* fit ){ m_fit = fit; }
	inline void SetFitResultPtr( TFitResultPtr r ){ m_fit_result = r; }
	inline void SetFitLimitLB( const double lb ){ m_fit_limit_lb = lb; }
	inline void SetFitLimitUB( const double ub ){ m_fit_limit_ub = ub; }
	
	inline void SetBGPoly( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_value, val ); }
	inline void SetBGPolyErr( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_err, val ); }
	inline void SetBGPolyLB( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_lb, val ); }
	inline void SetBGPolyUB( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_ub, val ); }
	inline void SetBGPolyFixed( const unsigned int n, const bool val ){ this->SetBGQuantity<bool>( n, m_bg_fixed, val ); }
	inline void SetBGPolyLimited( const unsigned int n , const int val){ this->SetBGQuantity<int>( n, m_bg_limit, val ); }

	inline void AddPeakNumber( const unsigned int n ){ m_list_of_peak_numbers.push_back(n); }
	inline void SetPeakNumberMap( const unsigned int n, const unsigned int val ){ m_parameter_number_to_peak_number_map.at(n) = val; }

	inline void SetParentSpectrum( SFSpectrum* spec ){ m_parent_spectrum = spec; }


	// Other public functions
	TString GenerateTotalFitString( const bool is_individual_fit );

private:
	unsigned int m_number_of_peaks;
	unsigned int m_number_of_parameters;

	double m_fit_limit_lb;
	double m_fit_limit_ub;

	std::vector<FitParameterType> m_list_of_fit_parameter_types;
	std::vector<unsigned int> m_parameter_number_to_peak_number_map;
	std::vector<unsigned int> m_list_of_peak_numbers;

	TF1 * m_fit;
	std::vector<TF1*> m_fit_individual;

	TFitResultPtr m_fit_result;
	SFSpectrum *m_parent_spectrum;
	
	unsigned int m_background_polynomial_level;
	std::vector<double> m_bg_value;
	std::vector<double> m_bg_err;
	std::vector<double> m_bg_lb;
	std::vector<double> m_bg_ub;
	std::vector<bool> m_bg_fixed;
	std::vector<int> m_bg_limit;
	std::vector<std::vector<double>> m_bg_cov_matrix;

	MessageLogger *log = MessageLogger::GetInstance();


	// Private functions
	inline bool IsGoodNumber( const unsigned int n, const unsigned int compare ) const { return ( n >= 0 && n < compare ); }
	inline bool IsGoodPeakNumber( const unsigned int n ) const { return IsGoodNumber( n, m_number_of_peaks ); }
	inline bool IsGoodParameterNumber( const unsigned int n ) const { return IsGoodNumber( n, m_number_of_parameters ); };
	inline bool IsGoodBackgroundNumber( const unsigned int n ) const { return IsGoodNumber( n, m_background_polynomial_level + 1 ); }


	// General getter for BG quantities
	template <typename T>
	T GetBGQuantity( const unsigned int n, const std::vector<T> &my_vec ) const {
		if ( this->IsGoodBackgroundNumber(n) ){
			return my_vec.at(n);
		}
		return (T)0;
	}


	// General setter for BG quantities
	template <typename T>
	void SetBGQuantity( const unsigned int n, std::vector<T> &my_vec, const T value ){
		if ( this->IsGoodBackgroundNumber(n) ){
			my_vec.at(n) = value;
		}
		else{
			log->Warning( Form( "Cannot access array element %d in vector of size %lu", n, my_vec.size() ) );
		}
		return;
	}


	// Generate fit strings
	TString GenerateBackgroundString( const unsigned int n ) const;
	TString GenerateGaussianString( const unsigned int n, const int mode ) const;

	ClassDef(SFFit, 0);

};

#endif