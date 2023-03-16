#ifndef _SPECTRUM_INTEGRAL_HH_
#define _SPECTRUM_INTEGRAL_HH_

#include <vector>
#include <TFrame.h>
#include <TLine.h>
#include <TMath.h>
#include <TObject.h>
#include <TPad.h>
#include <TString.h>
#include "MessageLogger.hh"
#include "Fit.hh"
#include "Spectrum.hh"

// Need to forward-declare this because SFSpectrumIntegral refers to parent spectrum...
class SFSpectrum;
class SFFit;

class SFSpectrumIntegral : public TObject{
public:
	// Constructor + destructor
	SFSpectrumIntegral();
	~SFSpectrumIntegral();

	// Useful functions
	void Draw( Option_t *option = "" );
	void CalculateIntegral();
	TString GetStatus();

	// Getters
	inline SFSpectrum* GetSpectrum() const { return m_parent_spectrum; }
	inline SFFit* GetFit() const { return m_parent_fit; }

	inline unsigned int GetBGPolyOrder() const { return m_background_polynomial_level; }
	inline double GetBGPoly( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_value ); }
	inline double GetBGPolyErr( const unsigned int n ) const { return this->GetBGQuantity<double>( n, m_bg_err ); }
	double GetBGCovMatrix( unsigned int i, unsigned int j ) const;

	inline double GetIntegralLB() const { return m_lb; }
	inline double GetIntegralUB() const { return m_ub; }
	inline double GetIntegral() const { return m_integral_value; }
	inline double GetIntegralErr() const { return m_integral_error; }
	inline double GetCentroid() const { return m_centroid; }
	inline double GetCentroidErr() const { return m_centroid_err; }

	inline bool IsBackgroundFromCoordinates() const { return m_background_from_coordinates; }

	// Setters
	inline void SetParentSpectrum( SFSpectrum* spec ){ m_parent_spectrum = spec; }
	inline void SetParentFit( SFFit* fit ){ m_parent_fit = fit; }

	void SetBGParameters( SFFit *fit, TFitResultPtr r );
	void SetBGPolyOrder( const unsigned int p );
	inline void SetBGPoly( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_value, val ); }
	inline void SetBGPolyErr( const unsigned int n, const double val ){ this->SetBGQuantity<double>( n, m_bg_err, val ); }
	void SetBGCovMatrix( unsigned int i, unsigned int j, const double val );

	inline void SetIntegralLB( const double x ){ m_lb = x; }
	inline void SetIntegralUB( const double x ){ m_ub = x; }

	inline void SetBackgroundFromCoordinates( const bool x ){ m_background_from_coordinates = x; }


private:
	SFSpectrum *m_parent_spectrum;
	SFFit *m_parent_fit;
	unsigned int m_background_polynomial_level;
	std::vector<double> m_bg_value;
	std::vector<double> m_bg_err;
	std::vector<std::vector<double>> m_cov_matrix;

	double m_lb;
	double m_ub;
	double m_integral_value;
	double m_integral_error;
	double m_centroid;
	double m_centroid_err;

	bool m_background_from_coordinates;

	MessageLogger *log = MessageLogger::GetInstance();

	// Private functions
	inline bool IsGoodBackgroundNumber( const unsigned int n ) const { return ( n >= 0 && n < m_background_polynomial_level + 1 ); }
	double GetBackgroundAtX( const double x ) const;
	double GetBackgroundAtXError( const double x ) const;
	double GetBackgroundDerivativeAtX( const double x ) const;
	double FindBackgroundXForGivenY( const double y, const double x_guess ) const;


	template <typename T>
	T GetBGQuantity( const unsigned int n, const std::vector<T> &my_vec ) const {
		if ( this->IsGoodBackgroundNumber(n) ){
			return my_vec.at(n);
		}
		return (T)0;
	}

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

	ClassDef(SFSpectrumIntegral, 0);
};




#endif