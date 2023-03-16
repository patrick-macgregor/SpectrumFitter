#include "Fit.hh"

///////////////////////////////////////////////////////////////////////////////
SFFit::SFFit(){
	m_number_of_peaks = -1;
	m_number_of_parameters = -1;

	m_fit_limit_lb = -1;
	m_fit_limit_ub = -1;

	m_list_of_fit_parameter_types.resize(0);
	m_parameter_number_to_peak_number_map.resize(0);
	m_list_of_peak_numbers.resize(0);

	m_fit = nullptr;
	m_fit_individual.resize(0);
	m_fit_result = nullptr;
	
	m_background_polynomial_level = -1;
	m_bg_value.resize(0);
	m_bg_err.resize(0);
	m_bg_lb.resize(0);
	m_bg_ub.resize(0);
	m_bg_fixed.resize(0);
	m_bg_limit.resize(0);
	m_bg_cov_matrix.resize(0);

	log->Construction("SFFit::SFFit -- SFFit object constructed");
}
///////////////////////////////////////////////////////////////////////////////
SFFit::~SFFit(){
	// Delete objects
	for ( unsigned int i = 0; i < this->GetNumberOfPeaks(); ++i ){
		delete this->GetIndividualFit(i);
	}
	delete m_fit;

	m_list_of_fit_parameter_types.clear();
	m_parameter_number_to_peak_number_map.clear();
	m_list_of_peak_numbers.clear();
	m_fit_individual.clear();
	m_bg_value.clear();
	m_bg_err.clear();
	m_bg_lb.clear();
	m_bg_ub.clear();
	m_bg_fixed.clear();
	m_bg_limit.clear();
	m_bg_cov_matrix.clear();

	log->Construction("SFFit::~SFFit -- SFFit object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
double SFFit::GetReducedChiSquared() const{
	if ( (int)m_fit_result != -1 ){
		return m_fit_result->Chi2()/m_fit_result->Ndf();
	}
	return 0.0;
}
///////////////////////////////////////////////////////////////////////////////
double SFFit::GetCommonWidth() const{
	if ( (int)m_fit_result != -1 ){
		return m_fit_result->Parameter(0);
	}
	return 0.0;
}
///////////////////////////////////////////////////////////////////////////////
TString SFFit::GetBGInfoString() const {
	TString s = "";
	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		s.Append( Form(
			"%d%c%c",
			i,
			( this->IsBGPolyFixed(i) ? 'f' : '-' ),
			( this->IsBGPolyLimited(i) ? 'l' : '-' )
		) );
		if ( i < this->GetBGPolyOrder() ){
			s.Append('|');
		}
	}
	return s;
}
///////////////////////////////////////////////////////////////////////////////
TF1* SFFit::GetIndividualFit( const unsigned int n ) const{
	if ( this->IsGoodPeakNumber(n) ){
		return m_fit_individual.at(n);
	}
	return nullptr;
}
///////////////////////////////////////////////////////////////////////////////
SFFit::FitParameterType SFFit::GetFitParameterType( const unsigned int n ) const{
	if ( this->IsGoodParameterNumber(n) ){
		return m_list_of_fit_parameter_types.at(n);
	}
	return FitParameterType::FitParameterNULL;
}
///////////////////////////////////////////////////////////////////////////////
double SFFit::GetBGCovMatrix( unsigned int i, unsigned int j ) const{
	if ( j > i ){
		log->Warning("SFFit::GetBGCovMatrix -- First element should be larger than second in covariance matrix function...swapping them over!");
		double temp = i; i = j; j = temp;
	}

	if ( i >= 0 && i <= this->GetBGPolyOrder() ){
		if ( j >= 0 && j < i ){
			return m_bg_cov_matrix.at(i).at(j);
		}
	}
	log->Warning("SFFit::GetBGCovMatrix -- Couldn't access correct element in covariance matrix. Returning 0...");
	return 0.0;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetIndividualFit(const unsigned int n, TF1* fit){
	if ( this->IsGoodPeakNumber(n) ){
		m_fit_individual.at(n) = fit;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetFitParameterType(const unsigned int n, const FitParameterType type){
	if ( this->IsGoodParameterNumber(n) ){
		m_list_of_fit_parameter_types.at(n) = type;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetBGCovMatrix( unsigned int i, unsigned int j, const double val ){
	if ( j > i ){
		log->Warning("SFFit::SetBGCovMatrix -- First element should be larger than second in covariance matrix function...swapping them over!");
		double temp = i; i = j; j = temp;
	}

	if ( i >= 0 && i <= this->GetBGPolyOrder() ){
		if ( j >= 0 && j < i ){
			m_bg_cov_matrix.at(i).at(j) = val;
			return;
		}
	}
	log->Warning("SFFit::SetBGCovMatrix -- Couldn't access correct element in covariance matrix. No value set!");
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetNumberOfPeaks( const int n){
	// Check value is positive
	if ( n < 0 ){
		log->Error("SFFit::SetNumberOfPeaks -- n must be positive.");
	}

	m_number_of_peaks = n;
	m_fit_individual.resize(n);
	for ( int i = 0; i < n; ++i ){
		m_fit_individual.at(i) = nullptr;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetNumberOfFitParameters( const int n ){
	// Check value is positive
	if ( n < 0 ){
		log->Error("SFFit::SetNumberOfFitParameters -- n must be positive.");
	}

	m_number_of_parameters = n;
	m_list_of_fit_parameter_types.resize(n);
	m_parameter_number_to_peak_number_map.resize(n);
	for ( unsigned int i = 0; i < m_list_of_fit_parameter_types.size(); ++i ){
		m_list_of_fit_parameter_types.at(i) = FitParameterType::FitParameterNULL;
		m_parameter_number_to_peak_number_map.at(i) = -1;
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFFit::SetBGPolyOrder( const int p ){
	if ( p >= 0 ){
		m_background_polynomial_level = p;
		m_bg_value.resize(p+1);
		m_bg_err.resize(p+1);
		m_bg_lb.resize(p+1);
		m_bg_ub.resize(p+1);
		m_bg_fixed.resize(p+1);
		m_bg_limit.resize(p+1);
		m_bg_cov_matrix.resize(p+1);
		for ( unsigned int i = 0; i < m_bg_cov_matrix.size(); ++i ){
			m_bg_cov_matrix.at(i).resize(i);
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
// Generate a background string with first parameter n
TString SFFit::GenerateBackgroundString( const unsigned int n ) const {
	TString fit = "";
	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		// Add a nicely formatted constant * x^y
		if ( i == 0 ){
			fit.Append( Form("[%i]", n+i ) );
		}
		else if ( i == 1 ){
			fit.Append( Form("[%i]*x", n+i ) );
		} 
		else{
			fit.Append( Form("[%i]*x**%d", n+i, i) );
		}

		// Add + if needed
		if ( i < this->GetBGPolyOrder() )fit.Append(" + ");
	}
	return fit;
}
///////////////////////////////////////////////////////////////////////////////
// Generate a Gaussian string with the following modes
// 0 -- COMMON WIDTH -- generate a standard Gaussian which uses the common width (parameter 0)
// 1 -- SCALED COMMON WIDTH -- generate a Gaussian which scales the common width (with the scaling factor >= 1)
// 2 -- FIXED WIDTH -- use an entirely different width, with the promise it will be fixed later...
TString SFFit::GenerateGaussianString(const unsigned int n, const int mode) const {
	if ( mode == 0 ){
		return Form( "[%i]*exp( -0.5*((x-[%i])/[0])^2)", n, n+1 );
	}

	if ( mode == 1 ){
		return Form( "[%i]*exp( -0.5*((x-[%i])/([%i]*[0]))^2)", n+1, n+2, n );
	}

	if ( mode == 2 ){
		return Form( "[%i]*exp( -0.5*((x-[%i])/[%i])^2)", n+1, n+2, n );
	}

	log->Warning("Trying to generate Gaussian string with no guidance. No Gaussian string being returned...");
	return "";
}
///////////////////////////////////////////////////////////////////////////////
// Generate a total fit string ( as well as labelling the different parameters)
TString SFFit::GenerateTotalFitString( const bool is_individual_fit ){
	TString fit_string = "";

	// For generating individual fits
	if ( is_individual_fit ){
		fit_string.Append( GenerateGaussianString(1,0) );
		fit_string.Append(" + ");
		fit_string.Append( GenerateBackgroundString(3) );
		return fit_string;
	}

	// Generate fits with multiple peaks (and store parameter values!)
	int par_num = 1;
	this->SetFitParameterType( 0, FitParameterType::FitParameterWidth );

	// Loop over peaks in the spectrum
	for ( unsigned int i = 0; i < this->GetNumberOfPeaks(); ++i ){
		SFPeak *peak = m_parent_spectrum->GetPeak( this->GetPeakNumber(i) );
		
		
		if ( peak->HasFixedWidth() == true ){
			fit_string.Append( GenerateGaussianString( par_num, 2 ) );
			this->SetFitParameterType( par_num+0, FitParameterType::FitParameterWidth );
			this->SetFitParameterType( par_num+1, FitParameterType::FitParameterAmplitude );
			this->SetFitParameterType( par_num+2, FitParameterType::FitParameterMean );
			par_num += 3;
		}
		// Doublet or unbound
		else if ( peak->IsDoublet() || peak->IsUnbound()){
			fit_string.Append( GenerateGaussianString( par_num, 1 ) );
			this->SetFitParameterType( par_num+0, FitParameterType::FitParameterWidthScale );
			this->SetFitParameterType( par_num+1, FitParameterType::FitParameterAmplitude );
			this->SetFitParameterType( par_num+2, FitParameterType::FitParameterMean );
			par_num += 3;
		}
		
		else{
			// Bound non-doublet
			fit_string.Append( GenerateGaussianString( par_num, 0 ) );
			this->SetFitParameterType( par_num+0, FitParameterType::FitParameterAmplitude );
			this->SetFitParameterType( par_num+1, FitParameterType::FitParameterMean );
			par_num += 2;
		}
		fit_string.Append(" + ");
	}

	// Add background to fit string
	fit_string.Append( GenerateBackgroundString( par_num ) );
	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		this->SetFitParameterType( par_num+i, FitParameterType::FitParameterBackground );
	}

	if ( log->GetPrintConsoleLevel() >= MessageLogger::Level::LevelDebug ){
		for ( unsigned int i = 0; i < this->GetNumberOfFitParameters(); ++i ){
			TString type_name = "";
			if ( this->GetFitParameterType(i) == FitParameterType::FitParameterWidth )type_name = "width";
			else if ( this->GetFitParameterType(i) == FitParameterType::FitParameterWidthScale )type_name = "wscale";
			else if ( this->GetFitParameterType(i) == FitParameterType::FitParameterAmplitude )type_name = "amplitude";
			else if ( this->GetFitParameterType(i) == FitParameterType::FitParameterMean )type_name = "mean";
			else if ( this->GetFitParameterType(i) == FitParameterType::FitParameterBackground )type_name = "background";
			else if ( this->GetFitParameterType(i) == FitParameterType::FitParameterNULL )type_name = "null";

			log->Debug( Form( "SFFit::GenerateTotalFitString -- PAR %02d: %s", i, type_name.Data() ) );
		}
	}

	return fit_string;
}