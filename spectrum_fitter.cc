#include "CommandLineInterface.hh"
#include "FitWriter.hh"
#include "InputFileProcessor.hh"
#include "MessageLogger.hh"
#include "Peak.hh"
#include "Spectrum.hh"
#include "SpectrumDrawer.hh"
#include "SpectrumFitter.hh"

#include <TApplication.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TRootCanvas.h>
#include <TString.h>

#include <iostream>

// TODO LIST ----------------------------------------------------------------------------------- //
// * Fix BG polynomial values in input file														 //
// * Add doxygen comments																		 //
// * Make safer getters for getting from vectors
// * Fix error on background integrals...
// --------------------------------------------------------------------------------------------- //


// GLOBAL VARIABLES (for command line interface)
TString g_spectrum_fitter_file_location = "";
TString g_fit_paramater_output_file_location  = "";
bool g_help_flag = false;
bool g_print_debug_messages = false;
MessageLogger* MessageLogger::m_instance_ptr = nullptr;

int main( int argc, char *argv[] ){

	// INITIALISE THE FITTING OPTIONS AND PROCESSES -------------------------------------------- //
	// Command line options
	MessageLogger* log = MessageLogger::GetInstance();
	log->SetPrintConsoleLevel( MessageLogger::LevelWarning );
	log->Debug("MessageLogger instance initialised");

	CommandLineInterface *interface = new CommandLineInterface();
	log->Debug("CommandLineInterface instance initialised");

	// Specify options
	interface->Add("-s", "Spectrum fitter file", &g_spectrum_fitter_file_location );
	interface->Add("-d", "Print debug messages when running", &g_print_debug_messages );
	interface->Add("-h", "Print this help", &g_help_flag );
	log->Debug("Added options to CommandLineInterface instance");

	// Process inputs
	interface->CheckFlags( argc, argv );
	log->Debug("Checked options for CommandLineInterface");

	// Decide whether to print debug messages or not
	if ( g_print_debug_messages ){
		log->SetPrintConsoleLevel( MessageLogger::LevelConstruction );
	}

	// Print help message if this option is picked
	if( g_help_flag ) {
		interface->CheckFlags( 1, argv );
		log->Debug("Help message displayed and exiting main");
		return 0;
	}

	// If nothing given, do nothing
	if ( argc == 1 ){
		log->Debug("No arguments given so exiting main");
		return 0;
	}

	// Check a fitting file was given -> break if not
	if ( g_spectrum_fitter_file_location == "" ){
		log->Error("A fitting file must be given in order to fit this spectrum. Use the \"-s\" flag.");
		return 1;
	}

	// BEGIN PROCESSING THE SPECTRUM ----------------------------------------------------------- //
	// Create a spectrum
	SFSpectrum *spec = new SFSpectrum();
	log->Debug("SFSpectrum instance initialised");

	// Create the processors of the spectrum
	SFSpectrumFitter *sf = new SFSpectrumFitter();
	log->Debug("SFSpectrumFitter instance initialised and added spectrum");

	SFSpectrumDrawer *sd = new SFSpectrumDrawer();
	log->Debug("SFSpectrumDrawer instance initialised and added spectrum");

	// Process the file that controls all of the aspects of the fitting process
	InputFileProcessor *ifp = new InputFileProcessor();
	log->Debug("InputFileProcessor instance initialised");

	SFFitWriter *fw = new SFFitWriter();
	log->Debug("SFFitWriter instance initialised");

	ifp->SetSpectrum(spec);
	ifp->SetSpectrumFitter(sf);
	ifp->SetSpectrumDrawer(sd);
	ifp->SetFitWriter(fw);

	log->Debug("InputFileProcessor class pointers added");

	// Set file options
	ifp->SetFileLocation(g_spectrum_fitter_file_location);
	log->Debug("Input configuration file set");
	
	// Process input spectrum file
	ifp->ProcessOptions();
	log->Debug("InputFileProcessor finished processing input options");

	// Fit the spectrum
	sf->SetSpectrum(spec);
	sf->InitialiseSpectrumGuesses();
	log->Debug("SFSpectrumFitter initialised spectrum guesses");
	sf->GenerateInitialFits();
	log->Debug("SFSpectrumFitter fits generated");
	sf->SetFittingOptions();
	log->Debug("SFSpectrumFitter fit options implemented");
	sf->FitPeaks();
	log->Debug("SFSpectrumFitter peaks fit");
	sf->CalculateIntegrals();
	log->Debug("SFSpectrumFitter integrals calculated");

	// Check whether to open the canvas interactively
	TApplication *app = nullptr;
	if ( sd->GetInteractiveMode() ){
		app = new TApplication( "spectrum_fitter", &argc, argv );
		log->Debug("Interactive mode enabled");
	}

	// Draw the spectrum
	sd->SetSpectrum(spec);
	sd->FormatSpectrum();
	log->Debug("SFSpectrumDrawer formatted spectrum");
	sd->DrawSpectrum();
	log->Debug("SFSpectrumDrawer drawn spectrum");
	sd->PrintCanvas();
	log->Debug("SFSpectrumDrawer canvas saved");

	// Write the fits to a nice convenient format
	fw->SetSpectrum( spec );
	fw->WriteFits();
	log->Debug("SFFitWriter Fits written to file");

	// Do the interactive canvas options
	if ( sd->GetInteractiveMode() && app != nullptr ){
		TCanvas *c = sd->GetCanvas();
		c->ToggleEventStatus();
		TRootCanvas *rc = (TRootCanvas *)c->GetCanvasImp();
		rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
		app->Run();
		log->Debug("Interactive mode started. Waiting for user to close canvas window");
	}

	// Memory management
	log->Debug("Beginning memory management");
	delete app;
	delete ifp;
	delete sd;
	delete sf;
	delete spec;
	delete interface;
	log->Debug("Memory management successful");

	log->Debug("Main application complete");
	delete log;
	return 0;
}
