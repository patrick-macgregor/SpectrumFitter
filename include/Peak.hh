// Holds peak information
#ifndef _PEAK_HH_
#define _PEAK_HH_

#include <iostream>
#include <iomanip>
#include <TObject.h>
#include <TString.h>
#include "MessageLogger.hh"

class SFPeak : public TObject{
public:
	SFPeak();
	~SFPeak();

	// Getters
	inline double GetMean() const { return m_mean; }
	inline double GetWidth() const { return m_wid; }
	inline double GetAmplitude() const { return m_amp; }
	inline double GetMeanErr() const { return m_mean_err; }
	inline double GetWidthErr() const { return m_wid_err; }
	inline double GetAmplitudeErr() const { return m_amp_err; }
	inline double GetMeanLB() const { return m_mean_lb; }
	inline double GetWidthLB() const { return m_wid_lb; }
	inline double GetAmplitudeLB() const { return m_amp_lb; }
	inline double GetMeanUB() const { return m_mean_ub; }
	inline double GetWidthUB() const { return m_wid_ub; }
	inline double GetAmplitudeUB() const { return m_amp_ub; }
	inline bool HasFixedMean() const { return m_mean_fixed; }
	inline bool HasFixedWidth() const { return m_wid_fixed; }
	inline bool HasFixedAmplitude() const { return m_amp_fixed; }
	inline int HasLimitedMean(){ return m_mean_limit; }
	inline int HasLimitedWidth(){ return m_wid_limit; }
	inline int HasLimitedAmplitude(){ return m_amp_limit; }
	inline bool IsBound() const { return !m_unbound; }
	inline bool IsUnbound() const { return m_unbound; }
	inline bool IsDoublet() const { return m_doublet; }
	inline double GetArea() const { return m_area; }
	inline double GetAreaErr() const { return m_area_err; }

	TString GetStatus();

	// Setters
	inline void SetMean( const double x ){ m_mean = x; }
	inline void SetWidth( const double x ){ m_wid = x; }
	inline void SetAmplitude( const double x ){ m_amp = x; }
	inline void SetMeanErr( const double x ){ m_mean_err = x; }
	inline void SetWidthErr( const double x ){ m_wid_err = x; }
	inline void SetAmplitudeErr( const double x ){ m_amp_err = x; }
	inline void SetMeanLB( const double x ){ m_mean_lb = x; }
	inline void SetWidthLB( const double x ){ m_wid_lb = x; }
	inline void SetAmplitudeLB( const double x ){ m_amp_lb = x; }
	inline void SetMeanUB( const double x ){ m_mean_ub = x; }
	inline void SetWidthUB( const double x ){ m_wid_ub = x; }
	inline void SetAmplitudeUB( const double x ){ m_amp_ub = x; }
	inline void SetFixedMean( const bool b ){ m_mean_fixed = b; }
	inline void SetFixedWidth( const bool b ){ m_wid_fixed = b; }	// N.B. this should not be used for the fixed bound width...this is only for peaks with fixed widths apart from the bound width!
	inline void SetFixedAmplitude( const bool b ){ m_amp_fixed = b; }
	inline void SetLimitedMean( const int a ){ m_mean_limit = a; }
	inline void SetLimitedWidth( const int a ){ m_wid_limit = a; }
	inline void SetLimitedAmplitude( const int a ){ m_amp_limit = a; }
	inline void SetBound(){ m_unbound = false; }
	inline void SetUnbound(){ m_unbound = true; }
	inline void SetDoublet(){ m_doublet = true; }
	inline void SetArea( const double x ){ m_area = x; }
	inline void SetAreaErr( const double x ){ m_area_err = x; }


	// Advanced setters
	inline void SetMeanBounds( const double lb, const double ub ){ m_mean_lb = lb; m_mean_ub = ub; }
	inline void SetWidthBounds( const double lb, const double ub ){ m_wid_lb = lb; m_wid_ub = ub; }
	inline void SetAmplitudeBounds( const double lb, const double ub ){ m_amp_lb = lb; m_amp_ub = ub; }

	void SetMeanGuess( const double mean, const double mean_lb, const double mean_ub){
		this->SetMean(mean);
		this->SetMeanBounds(mean_lb,mean_ub);
		return;
	}

	void SetWidthGuess( const double width, const double width_lb, const double width_ub){
		this->SetWidth(width);
		this->SetWidthBounds(width_lb,width_ub);
		return;
	}

	void SetAmplitudeGuess( const double amp, const double amp_lb, const double amp_ub){
		this->SetAmplitude(amp);
		this->SetAmplitudeBounds(amp_lb,amp_ub);
		return;
	}

	// Useful functions
	void PrintPeakTerminal() const;
	void PrintPeakDividerTerminal() const ;

private:
	// 3 variables: mean, amplitude, width
	// 6 properties for each variable: lb, ub, value, error, fixed, limit
	// 1 boolean to say if the state is unbound
	// 1 boolean to say if the state is a doublet
	// Stores the area (which is calculated after fitting)
	double m_mean;
	double m_wid;
	double m_amp;

	double m_mean_err;
	double m_wid_err;
	double m_amp_err;
	
	double m_mean_lb;
	double m_wid_lb;
	double m_amp_lb;

	double m_mean_ub;
	double m_wid_ub;
	double m_amp_ub;

	bool m_mean_fixed;
	bool m_wid_fixed;
	bool m_amp_fixed;

	int m_mean_limit;
	int m_wid_limit;
	int m_amp_limit;

	double m_area;
	double m_area_err;

	bool m_unbound;
	bool m_doublet;
	MessageLogger *log = MessageLogger::GetInstance();

	ClassDef( SFPeak, 0 );
};



#endif
