#ifndef _SPECTRUM_DRAWER_HH_
#define _SPECTRUM_DRAWER_HH_

#include <iostream>
#include <vector>
#include <TCanvas.h>
#include <TFrame.h>
#include <TLatex.h>
#include <TLine.h>
#include <TPaveStats.h>
#include <TPolyLine.h>
#include <TText.h>
#include <TString.h>
#include "MessageLogger.hh"
#include "Spectrum.hh"

class SFSpectrumDrawer{
public:
	SFSpectrumDrawer();
	~SFSpectrumDrawer();

	// Functions
	void FormatSpectrum();
	void DrawSpectrum();
	void PrintCanvas();

	// Setters
	void SetSpectrum( SFSpectrum *spec );

	inline void SetPrintPS( bool b ){ m_file_format_select.at(0) = b; }
	inline void SetPrintEPS( bool b ){ m_file_format_select.at(1) = b; }
	inline void SetPrintPDF( bool b ){ m_file_format_select.at(2) = b; }
	inline void SetPrintSVG( bool b ){ m_file_format_select.at(3) = b; }
	inline void SetPrintTEX( bool b ){ m_file_format_select.at(4) = b; }
	inline void SetPrintGIF( bool b ){ m_file_format_select.at(5) = b; }
	inline void SetPrintXPM( bool b ){ m_file_format_select.at(6) = b; }
	inline void SetPrintPNG( bool b ){ m_file_format_select.at(7) = b; }
	inline void SetPrintJPG( bool b ){ m_file_format_select.at(8) = b; }
	inline void SetPrintTIFF( bool b ){ m_file_format_select.at(9) = b; }
	inline void SetPrintCXX( bool b ){ m_file_format_select.at(10) = b; }
	inline void SetPrintXML( bool b ){ m_file_format_select.at(11) = b; }
	inline void SetPrintJSON( bool b ){ m_file_format_select.at(12) = b; }
	inline void SetPrintROOT( bool b ){ m_file_format_select.at(13) = b; }

	inline void SetPrintFileName( TString name ){ m_print_file_name = name; }
	inline void SetCanvasWidth( int d ){ m_canvas_width = d; }
	inline void SetCanvasHeight( int d ){ m_canvas_height = d; }
	inline void SetCanvas( TCanvas *c ){ m_canvas = c; }
	inline void SetCanvasTitle( TString s ){ m_canvas_title = s; }
	inline void SetXAxisTitle( TString s ){ m_x_axis_title = s; }
	inline void SetYAxisTitle( TString s ){ m_y_axis_title = s; }
	inline void SetXAxisLB( double x ){ m_x_axis_lb = x; }
	inline void SetXAxisUB( double x ){ m_x_axis_ub = x; }
	inline void SetXAxisLBDefined( bool x ){ m_x_axis_lb_defined = x; }
	inline void SetXAxisUBDefined( bool x ){ m_x_axis_ub_defined = x; }
	inline void SetInteractiveMode( bool b ){ m_interactive_mode = b; }

	// Getters
	inline SFSpectrum* GetSpectrum(){ return m_spec; }

	inline bool GetPrintPS(){ return m_file_format_select.at(0); }
	inline bool GetPrintEPS(){ return m_file_format_select.at(1); }
	inline bool GetPrintPDF(){ return m_file_format_select.at(2); }
	inline bool GetPrintSVG(){ return m_file_format_select.at(3); }
	inline bool GetPrintTEX(){ return m_file_format_select.at(4); }
	inline bool GetPrintGIF(){ return m_file_format_select.at(5); }
	inline bool GetPrintXPM(){ return m_file_format_select.at(6); }
	inline bool GetPrintPNG(){ return m_file_format_select.at(7); }
	inline bool GetPrintJPG(){ return m_file_format_select.at(8); }
	inline bool GetPrintTIFF(){ return m_file_format_select.at(9); }
	inline bool GetPrintCXX(){ return m_file_format_select.at(10); }
	inline bool GetPrintXML(){ return m_file_format_select.at(11); }
	inline bool GetPrintJSON(){ return m_file_format_select.at(12); }
	inline bool GetPrintROOT(){ return m_file_format_select.at(13); }

	inline TString GetPrintFileName(){ return m_print_file_name; }
	inline int GetCanvasWidth(){ return m_canvas_width; }
	inline int GetCanvasHeight(){ return m_canvas_height; }
	inline TCanvas* GetCanvas(){ return m_canvas; }
	inline TString GetCanvasTitle(){ return m_canvas_title; }
	inline TString GetXAxisTitle(){ return m_x_axis_title; }
	inline TString GetYAxisTitle(){ return m_y_axis_title; }
	inline double GetXAxisLB(){ return m_x_axis_lb; }
	inline double GetXAxisUB(){ return m_x_axis_ub; }
	inline bool GetXAxisLBDefined(){ return m_x_axis_lb_defined; }
	inline bool GetXAxisUBDefined(){ return m_x_axis_ub_defined; }
	inline bool GetInteractiveMode(){ return m_interactive_mode; }

private:
	static const std::vector<TString> m_file_formats;

	SFSpectrum *m_spec;
	TString m_print_file_name;
	std::vector<bool> m_file_format_select;
	std::vector<TPolyLine*> m_peak_marker_triangle;
	std::vector<TText*> m_peak_marker_text;

	TCanvas *m_canvas;
	int m_canvas_width;
	int m_canvas_height;
	TString m_canvas_title;

	TString m_x_axis_title;
	TString m_y_axis_title;

	double m_x_axis_lb;
	double m_x_axis_ub;
	bool m_x_axis_lb_defined;
	bool m_x_axis_ub_defined;

	bool m_interactive_mode;

	MessageLogger *log = MessageLogger::GetInstance();

	// Private functions
	void MakePeakMarker( const unsigned int n );

};

#endif