#include "SpectrumIntegral.hh"

///////////////////////////////////////////////////////////////////////////////
SFSpectrumIntegral::SFSpectrumIntegral(){
	m_parent_spectrum = nullptr;
	m_parent_fit = nullptr;
	m_bg_value.resize(0);
	m_bg_err.resize(0);
	m_cov_matrix.resize(0);
	m_lb = -1.0;
	m_ub = -1.0;
	m_integral_value = -1.0;
	m_integral_error = -1.0;
	m_background_from_coordinates = false;

	log->Construction("SFSpectrumIntegral::SFSpectrumIntegral -- SFSpectrumIntegral object created");
}
///////////////////////////////////////////////////////////////////////////////
SFSpectrumIntegral::~SFSpectrumIntegral(){
	m_bg_value.clear();
	m_bg_err.clear();
	m_cov_matrix.clear();
	m_parent_spectrum = nullptr;
	m_parent_fit = nullptr;

	log->Construction("SFSpectrumIntegral::~SFSpectrumIntegral -- SFSpectrumIntegral object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumIntegral::Draw( Option_t *option ){
	TFrame* frame = gPad->GetFrame();
	TLine *lb = new TLine( m_lb, frame->GetY1(), m_lb, frame->GetY2() );
	TLine *ub = new TLine( m_ub, frame->GetY1(), m_ub, frame->GetY2() );

	double y1 = 0.0;
	double y2 = 0.0;

	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		y1 += this->GetBGPoly(i)*TMath::Power( m_lb, (int)i );
		y2 += this->GetBGPoly(i)*TMath::Power( m_ub, (int)i );
	}

	TLine *bg = new TLine( m_lb, y1, m_ub, y2 );

	lb->SetLineColor( kRed );
	ub->SetLineColor( kRed );
	bg->SetLineColor( kRed );

	lb->SetLineStyle( kDashed );
	ub->SetLineStyle( kDashed );

	lb->SetLineWidth(1);
	ub->SetLineWidth(1);
	bg->SetLineWidth(2);

	lb->Draw(option);
	ub->Draw(option);
	bg->Draw(option);

	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumIntegral::CalculateIntegral(){
	// Calculate the total integral and error
	TH1F* h = m_parent_spectrum->GetHist();

	// Find central portion
	double integral = h->Integral( h->FindBin( m_lb ) + 1, h->FindBin( m_ub ) - 1 );
	
	// Add fractional quantities for end bins (linear scaling)
	integral += h->GetBinContent( h->FindBin(m_lb ) )*( h->GetBinLowEdge( h->FindBin(m_lb) + 1 ) - m_lb )/h->GetBinWidth( h->FindBin(m_lb) );
	integral += h->GetBinContent( h->FindBin(m_ub ) )*( m_ub - h->GetBinLowEdge( h->FindBin(m_ub) ) )/h->GetBinWidth( h->FindBin(m_ub) );

	// Calculate background portion + centroid
	double bg_integral = 0.0;
	double bg_error_squared = 0.0;
	double sum_xy = 0.0;
	double sum_y = 0.0;
	double sum_xx = 0.0;
	double sum_x = 0.0;
	double sum_N = 0.0;

	// Calculate the background contributions in the middle bins
	for ( int i = h->FindBin( m_lb ); i <= h->FindBin( m_ub ); ++i ){
		double x1 = TMath::Max( h->GetBinLowEdge(i), m_lb );
		double x2 = TMath::Min( h->GetBinLowEdge(i+1), m_ub );
		double y1 = 0.0;
		double y2 = 0.0;
		for ( unsigned int j = 0; j <= this->GetBGPolyOrder(); ++j ){
			y1 += this->GetBGPoly(j)*TMath::Power( x1, (int)j );
			y2 += this->GetBGPoly(j)*TMath::Power( x2, (int)j );
		}

		// Now calculate the area based on the situation
		double bg_contribution = 0.0;
		double scale_bin_factor = (x2-x1)/h->GetBinWidth(0);
		if ( y1 <=0  && y2<= 0 ){
			// Line below histogram...
			// Add no background
		}
		else if ( h->GetBinContent(i) < y1 && h->GetBinContent(i) < y2 ){
			// Line above histogram...no centroid contribution
			bg_contribution += h->GetBinContent(i)*scale_bin_factor;
		}
		else{
			// Add this on to all options below here
			bg_contribution += 0.5*(y1+y2)*scale_bin_factor;

			// Check if we need to make a correction if the background passes through the top of the bin
			if ( h->GetBinContent(i) > y1 && h->GetBinContent(i) <= y2 ){
				double x3 = FindBackgroundXForGivenY( h->GetBinContent(i), h->GetBinCenter(i) );
				bg_contribution -= 0.5*(x2-x3)*( y2 - h->GetBinContent(i) )/h->GetBinWidth(0);
			}
			else if ( h->GetBinContent(i) <= y1 && h->GetBinContent(i) > y2 ){
				double x3 = FindBackgroundXForGivenY( h->GetBinContent(i), h->GetBinCenter(i) );
				bg_contribution -= 0.5*(x3-x1)*( y1 - h->GetBinContent(i) )/h->GetBinWidth(0);
			}
		}
		bg_integral += bg_contribution;

		// Now work out centroid contribution (N.B. TODO bin center could be modified to improve this approximation...)
		double weighted_y = ( h->GetBinContent(i) - bg_contribution );
		if ( h->GetBinContent(i) > 0.0 )sum_N += weighted_y*scale_bin_factor/h->GetBinContent(i);
		sum_x = h->GetBinCenter(i)*scale_bin_factor;
		sum_xx = h->GetBinCenter(i)*h->GetBinCenter(i)*scale_bin_factor;
		sum_y += weighted_y*scale_bin_factor;
		sum_xy += h->GetBinCenter(i)*weighted_y*scale_bin_factor;
	}

	// Set member variables
	bg_error_squared = bg_integral;	// TODO NOT HAPPY WITH THIS, WILL NEED TO FIX LATER!!!
	m_integral_value = integral - bg_integral;
	m_integral_error = TMath::Sqrt( integral + bg_error_squared );
	m_centroid = sum_xy/sum_y;
	m_centroid_err = TMath::Sqrt( sum_xx - 2*sum_x*sum_xy/sum_y + sum_N*sum_xy*sum_xy/( sum_y*sum_y ) )/sum_y;
	return;
}
///////////////////////////////////////////////////////////////////////////////
double SFSpectrumIntegral::GetBGCovMatrix( unsigned int i, unsigned int j ) const{
	if ( j > i ){
		log->Warning("SFSpectrumIntegral::GetBGCovMatrix -- First element should be larger than second in covariance matrix function...swapping them over!");
		double temp = i; i = j; j = temp;
	}

	if ( i >= 0 && i < m_background_polynomial_level + 1 ){
		if ( j >= 0 && j < i ){
			return m_cov_matrix.at(i).at(j);
		}
	}
	log->Warning("SFSpectrumIntegral::GetBGCovMatrix -- Couldn't access correct element in covariance matrix. Returning 0...");
	return 0.0;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumIntegral::SetBGPolyOrder( const unsigned int p ){
	if ( p >= 0 ){
		m_background_polynomial_level = p;
		m_bg_value.resize(p+1);
		m_bg_err.resize(p+1);
		m_cov_matrix.resize(p+1);
		for ( unsigned int i = 0; i < m_cov_matrix.size(); ++i ){
			m_cov_matrix.at(i).resize(i);
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumIntegral::SetBGCovMatrix( unsigned int i, unsigned int j, const double val ){
	if ( j > i ){
		log->Warning("SFSpectrumIntegral::SetBGCovMatrix -- First element should be larger than second in covariance matrix function...swapping them over!");
		double temp = i; i = j; j = temp;
	}

	if ( i >= 0 && i < m_background_polynomial_level + 1 ){
		if ( j >= 0 && j < i ){
			m_cov_matrix.at(i).at(j) = val;
			return;
		}
	}
	log->Warning("SFSpectrumIntegral::SetBGCovMatrix -- Couldn't access correct element in covariance matrix. No value set!");

	return;
}
///////////////////////////////////////////////////////////////////////////////
double SFSpectrumIntegral::GetBackgroundAtX( const double x ) const{
	double sum = 0.0;
	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		sum += this->GetBGPoly(i)*TMath::Power( x, (int)i );
	}
	return sum;
}
///////////////////////////////////////////////////////////////////////////////
double SFSpectrumIntegral::GetBackgroundAtXError( const double x ) const{
	double sum = 0.0;
	for ( unsigned int i = 0; i <= this->GetBGPolyOrder(); ++i ){
		sum += TMath::Power( TMath::Power( x, (int)i )*this->GetBGPolyErr(i), 2 );
		for ( unsigned int j = 0; j < i; ++j ){
			sum += 2*TMath::Power( x, (int)(i+j) )*this->GetBGCovMatrix(i,j);
		}
	}
	return TMath::Sqrt(sum);
}
///////////////////////////////////////////////////////////////////////////////
double SFSpectrumIntegral::GetBackgroundDerivativeAtX( const double x ) const{
	double sum = 0.0;
	for ( unsigned int i = 1; i <= this->GetBGPolyOrder(); ++i ){
		sum += i*this->GetBGPoly(i)*TMath::Power( x, (int)(i-1) );
	}
	return sum;
}
///////////////////////////////////////////////////////////////////////////////
double SFSpectrumIntegral::FindBackgroundXForGivenY( const double y, const double x_guess ) const{
	double x1 = x_guess - 1;
	double x2 = x_guess;
	int count = 0;

	while ( TMath::Abs( x2 - x1) < 1e-5 ){
		x1 = x2;
		x2 = x1 - ( GetBackgroundAtX(x1) - y )/GetBackgroundDerivativeAtX(x1);
		count++;
	}
	return x2;
}
///////////////////////////////////////////////////////////////////////////////
TString SFSpectrumIntegral::GetStatus(){
	return Form( "%c", ( m_background_from_coordinates ? 'C' : 'F' ) );
}

