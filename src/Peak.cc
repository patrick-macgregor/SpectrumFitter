#include "../include/Peak.hh"

///////////////////////////////////////////////////////////////////////////////
// Constructor -> make all values negative initially to indicate they have not been set by the user
SFPeak::SFPeak(){
	m_mean = -1.0;
	m_wid = -1.0;
	m_amp = -1.0;
	m_area = -1.0;
	m_mean_err = -1.0;
	m_wid_err = -1.0;
	m_amp_err = -1.0;
	m_area_err = -1.0;
	m_mean_lb = -1.0;
	m_wid_lb = -1.0;
	m_amp_lb = -1.0;
	m_mean_ub = -1.0;
	m_wid_ub = -1.0;
	m_amp_ub = -1.0;
	m_mean_fixed = false;
	m_wid_fixed = false;
	m_amp_fixed = false;
	m_mean_limit = 0;
	m_wid_limit = 0;
	m_amp_limit = 0;
	m_unbound = false;
	m_doublet = false;
	log->Construction("SFPeak::SFPeak -- SFPeak object created");
}
///////////////////////////////////////////////////////////////////////////////
SFPeak::~SFPeak(){
	log->Construction("SFPeak::~SFPeak -- SFPeak object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
TString SFPeak::GetStatus(){
	return Form(
		"A%c%c|W%c%c|M%c%c",
		( this->HasFixedAmplitude() ? 'f' : '-' ),
		( this->HasLimitedAmplitude() ? 'l' : '-' ),
		( this->HasFixedWidth() ? 'f' : '-' ),
		( this->HasLimitedWidth() ? 'l' : '-' ),
		( this->HasFixedMean() ? 'f' : '-' ),
		( this->HasLimitedMean() ? 'l' : '-' )
	);
}
///////////////////////////////////////////////////////////////////////////////
// Print peak information to terminal
void SFPeak::PrintPeakTerminal() const {
	int w = 8;
	std::cout << std::setw(w) << "" << std::setw(w) <<  "MEAN" << std::setw(w) << "AMP" << std::setw(w) << "WIDTH" << std::endl;
	std::cout << std::setw(w) << "Value" << std::setw(w) << m_mean << std::setw(w) << m_amp << std::setw(w) << m_wid << std::endl;
	std::cout << std::setw(w) << "Error" << std::setw(w) << m_mean_err << std::setw(w) << m_amp_err << std::setw(w) << m_wid_err << std::endl;
	std::cout << std::setw(w) << "LB" << std::setw(w) << m_mean_lb << std::setw(w) << m_amp_lb << std::setw(w) << m_wid_lb << std::endl;
	std::cout << std::setw(w) << "UB" << std::setw(w) << m_mean_ub << std::setw(w) << m_amp_ub << std::setw(w) << m_wid_ub << std::endl;
	std::cout << std::setw(w) << "Fixed?" << std::setw(w) << ( m_mean_fixed ? "Y" : "N" ) << std::setw(w) << ( m_amp_fixed ? "Y" : "N" ) << std::setw(w) << ( m_wid_fixed ? "Y" : "N" ) << std::endl;
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFPeak::PrintPeakDividerTerminal() const {
	int w = 8;
	std::cout << std::setfill('-');
	std::cout << std::setw(w) << "" << std::setw(w) << "" << std::setw(w) << "" << std::setw(w) << "" << std::endl;
	std::cout << std::setfill(' ');
	return;
}