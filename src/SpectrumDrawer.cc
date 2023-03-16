#include "SpectrumDrawer.hh"

///////////////////////////////////////////////////////////////////////////////
const std::vector<TString> SFSpectrumDrawer::m_file_formats = { "ps", "eps", "pdf", "svg", "tex", "gif", "xpm", "png", "jpg", "tiff", "cxx", "xml", "json", "root" };
///////////////////////////////////////////////////////////////////////////////
SFSpectrumDrawer::SFSpectrumDrawer(){
	m_spec = nullptr;
	m_file_format_select.resize( m_file_formats.size() );
	for ( unsigned int i = 0; i < m_file_format_select.size(); ++i ){
		m_file_format_select.at(i) = false;
	}
	m_peak_marker_triangle.resize(0);
	m_peak_marker_text.resize(0);
	m_canvas_width = -1;
	m_canvas_height = -1;
	m_print_file_name = "";
	m_canvas_title = "";
	m_x_axis_title = "";
	m_y_axis_title = "";
	m_x_axis_lb = -1;
	m_x_axis_ub = -1;
	log->Construction("SFSpectrumDrawer::SFSpectrumDrawer -- SFSpectrumDrawer object constructed");
}
///////////////////////////////////////////////////////////////////////////////
SFSpectrumDrawer::~SFSpectrumDrawer(){
	// Delete objects from memory
	for ( unsigned int i = 0; i < m_peak_marker_text.size(); ++i ){
		delete m_peak_marker_text.at(i);
	}

	for ( unsigned int i = 0; i < m_peak_marker_triangle.size(); ++i ){
		delete m_peak_marker_triangle.at(i);
	}

	delete m_canvas;
	
	// Resize vectors
	m_peak_marker_text.resize(0);
	m_peak_marker_triangle.resize(0);
	m_file_format_select.resize(0);
	log->Construction("SFSpectrumDrawer::~SFSpectrumDrawer -- SFSpectrumDrawer object destroyed");
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumDrawer::SetSpectrum( SFSpectrum *spec ){
	m_spec = spec;
	m_peak_marker_triangle.resize( m_spec->GetNumberOfPeaks() );
	m_peak_marker_text.resize( m_spec->GetNumberOfPeaks() );
	for ( unsigned int i = 0; i < m_spec->GetNumberOfPeaks(); ++i ){
		m_peak_marker_text.at(i) = nullptr;
		m_peak_marker_triangle.at(i) = nullptr;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumDrawer::FormatSpectrum(){
	// Format the canvas
	if ( m_canvas == nullptr ){
		m_canvas = new TCanvas( "CANVAS", m_canvas_title, m_canvas_width, m_canvas_height );
	}

	// FORMAT THE HISTOGRAM
	TH1F *h = m_spec->GetHist();
	h->SetLineWidth(1);
	h->SetLineColor(kBlack);
	h->GetXaxis()->SetTitle( m_x_axis_title );
	h->GetYaxis()->SetTitle( m_y_axis_title );
	h->SetTitle( m_canvas_title );

	// FORMAT THE FITS
	// Format the bnd fit
	// TODO draw only if it's been fit
	for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
		SFFit* sffit = m_spec->GetFit(i);
		TF1* fit = sffit->GetFit();
		fit->SetLineColor(kRed);
		fit->SetLineWidth(3);
		fit->SetNpx(800);

		for ( unsigned int j = 0; j < sffit->GetNumberOfPeaks(); ++j ){
			fit = m_spec->GetFit(i)->GetIndividualFit(j);
			fit->SetLineWidth(1);
			fit->SetNpx(800);

			// Change colouring based on peak properties
			SFPeak * peak = m_spec->GetPeak( sffit->GetPeakNumber(j) );
			EColor fit_colour = kBlue;

			if ( peak->IsBound() ){
				if ( peak->IsDoublet() ){
					fit_colour = kCyan;
				}
				else if ( peak->HasFixedWidth() ){
					fit_colour = kViolet;
				}
			}
			else{
				if ( peak->IsDoublet() ){
					fit_colour = (EColor)(kOrange + 7);
				}
				else if ( peak->HasFixedWidth() ){
					fit_colour = kYellow;
				}
				else{
					fit_colour = kGreen;
				}
			}
			fit->SetLineColor( fit_colour );
		}
	}

	// OTHER FORMATTING OPTIONS
	// Set the range of the user
	double first = 0;
	double last = 1;
	if ( m_x_axis_lb_defined ){ first = m_x_axis_lb; }
	else{ first = h->GetBinLowEdge(0) + h->GetBinWidth(0); }
	if ( m_x_axis_ub_defined ){ last = m_x_axis_ub; }
	else{ last = h->GetBinLowEdge(h->GetNbinsX()) + h->GetBinWidth(h->GetNbinsX()); }

	h->GetXaxis()->SetRangeUser(first,last);

	// Update the canvas one last time
	m_canvas->Modified(); m_canvas->Update();
	
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumDrawer::DrawSpectrum(){
	// First draw the histogram on the canvas
	if ( m_canvas != nullptr ){
		m_canvas->cd();
		TH1F *h = m_spec->GetHist();
		if ( h != nullptr ){
			h->Draw("SAME");

			// Set pad margins
			m_canvas->SetLeftMargin(0.10);
			m_canvas->SetRightMargin(0.04);
			m_canvas->SetTopMargin(0.10);
			m_canvas->SetBottomMargin(0.08);

			// Stats box
			m_canvas->GetPad(0)->Update(); // Force stats box to be drawn
			TPaveStats *st = (TPaveStats*)h->FindObject("stats");
			if ( st != nullptr ){
				st->SetX1NDC(0.86); st->SetX2NDC(0.96);
				st->SetY1NDC(0.84); st->SetY2NDC(0.9);
				st->SetOptStat( 1000001 );
			}
			else{
				log->Warning( "Could not find stats box" );
			}
		}
		
		// Loop over fits in the spectrum
		for ( unsigned int i = 0; i < m_spec->GetNumberOfFits(); ++i ){
			// Draw total fits
			if ( m_spec->GetFit(i) != nullptr )m_spec->GetFit(i)->GetFit()->Draw("SAME");

			// Draw individual fits
			for ( unsigned int j = 0; j < m_spec->GetFit(i)->GetNumberOfPeaks(); ++j ){
				if ( m_spec->GetFit(i)->GetIndividualFit(j) != nullptr )m_spec->GetFit(i)->GetIndividualFit(j)->Draw("SAME");

				// Create peak markers
				MakePeakMarker( m_spec->GetFit(i)->GetPeakNumber(j) );
			}
			
		}

		// Draw peak markers
		for ( unsigned int i = 0; i < m_spec->GetNumberOfPeaks(); ++i ){
			if ( m_peak_marker_text.at(i) != nullptr )m_peak_marker_text.at(i)->Draw("SAME");
			if ( m_peak_marker_triangle.at(i) != nullptr )	m_peak_marker_triangle.at(i)->Draw("f SAME");
		}

		// Integral markers
		for ( unsigned int i = 0; i < m_spec->GetNumberOfIntegrals(); ++i ){
			m_spec->GetIntegral(i)->Draw();
		}
		
		// Separation energy line
		m_canvas->Modified(); m_canvas->Update();	// Makes the UI min and max update
		TLine *l_sep_en = new TLine( m_spec->GetSeparationEnergy(), m_canvas->GetUymin(), m_spec->GetSeparationEnergy(), m_canvas->GetUymax() );
		l_sep_en->SetLineWidth(1);
		l_sep_en->SetLineStyle(kDashed);
		l_sep_en->SetLineColor(kBlack);
		l_sep_en->Draw("SAME");

		float side_border = 0.02;
		float top_border = 0.1;
		TLatex *t_sep_en = new TLatex( m_spec->GetSeparationEnergy() + side_border*( m_canvas->GetUxmax() - m_canvas->GetUxmin() ), m_canvas->GetUymax() - top_border*( m_canvas->GetUymax() - m_canvas->GetUymin() ), Form( "S_{n} = %4.0f keV", m_spec->GetSeparationEnergy() ) );
		t_sep_en->SetTextAlign(13);
		t_sep_en->SetTextFont(42);
		t_sep_en->SetTextSize(0.03);
		t_sep_en->Draw("SAME");

	}
	else{
		log->Error("Canvas object not created. Use the FormatSpectrum() function first!");
	}
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumDrawer::PrintCanvas(){
	if ( m_canvas != nullptr ){
		for ( unsigned int i = 0; i < m_file_format_select.size(); ++i ){
			if ( m_file_format_select.at(i) ){
				m_canvas->Print( m_print_file_name.Append(".").Append( m_file_formats.at(i) ) );
			}
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void SFSpectrumDrawer::MakePeakMarker( const unsigned int n ){
	SFPeak * peak = m_spec->GetPeak( n );

	double x1 = m_canvas->GetFrame()->GetX1();
	double x2 = m_canvas->GetFrame()->GetX2();
	double y1 = m_canvas->GetFrame()->GetY1();
	double y2 = m_canvas->GetFrame()->GetY2();

	double xhalfwidth = 0.005*(x2-x1);
	double yheight = 0.02*(y2-y1);

	// Triangle coordinates
	double x[4] = {
		peak->GetMean() - xhalfwidth,
		peak->GetMean(),
		peak->GetMean() + xhalfwidth,
		peak->GetMean() - xhalfwidth
	};

	double base = peak->GetAmplitude() + 1;
	TH1F *h  = m_spec->GetHist();
	for ( int i = -2; i < 3; ++i ){
		base = TMath::Max( base, h->GetBinContent( h->FindBin( peak->GetMean() ) - i ) );
	}
	double y[4] = {
		base + 1.5*yheight,
		base + 0.5*yheight,
		base + 1.5*yheight,
		base + 1.5*yheight,
	};

	TPolyLine *p = new TPolyLine(4,x,y);
	p->SetFillColor(kRed);
	p->SetLineColor(kRed);

 	m_peak_marker_triangle.at(n) = p;

 	// Text coordinates
 	TString peak_number_string = Form( "%02d", n );
 	TText *t = new TText( peak->GetMean(), base + 2*yheight, peak_number_string.Data() );
 	t->SetTextAlign(21);
	t->SetTextFont(42);
	t->SetTextSize(0.02);

	m_peak_marker_text.at(n) = t;
	return;
}

