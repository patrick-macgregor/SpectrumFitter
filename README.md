# SpectrumFitter
Fits spectra in ROOT based on an input file that can be manipulated easily instead of editing a large code base

## Requirements
You need to have `ROOT` installed. See the official [ROOT website](https://root.cern) for help. You'll also need `make` installed

## Running the code
To run this code, you will need:
- A ROOT file containing a histogram with some Gaussian peaks (and a polynomail background)
- A configuration file that can be used as input

The input options to the script are:
- [-s <string> : Spectrum fitter file             ]
- [-d          : Print debug messages when running]
- [-h          : Print this help                  ]

and this can be run like

	$ spectrum_fitter -s config.dat -d

## Example
An example is provided in the example/ directory. There you will find a config file with default options laid out as well as a script used to generate a ROOT file, which can be run by doing

	$ root -l -x -q GenerateHistogramFile.cc

---

## Configuration file options
The following options can be used for configuring the fit:
```
ROOTFile: -				# The directory location of the ROOT file containing the histogram that is to be fitted
ROOTHistName: -				# The name of the histogram in the ROOTFile
FitParameterFile: -			# The name of the file that is to contain the fit parameters from the fit
	
NumberOfPeaks: -			# The total number of peaks in the spectrum
NumberOfFits: -				# The total number of fits to be applied to the spectrum (peaks in multiple fits will be fit multiple times)
NumberOfIntegrals: -			# The total number of integrals to be calculated for this spectrum
SeparationEnergy: -			# The separation energy for the spectrum

BackgroundDimension: -			# The order of the background polynomial (0 = flat, 1 = linear, 2 = quadratic, etc.)
BB.Background: -			# Set polynomial term BB to this value
BB.Background_LB: -			# Set polynomial term BB lower bound
BB.Background_UB: -			# Set polynomial term BB upper bound
BB.Background_fixed: -			# Fix polynomial term BB to the value specified (0 = free, 1 = fixed)

FF.FitLB: -				# Set fit FF lower bound
FF. FitUB: -				# Set fit FF upper bound

II.Integral_LB: -			# Set integral II lower bound
II.Integral_UB: -			# Set integral II upper bound
II.IntegralY1: -			# Set integral II Y1 - the Y value at which to draw the background
II.IntegralY2: -			# Set integral II Y2 - the Y value at which to end the background
II.IntegralFitNumber: -			# Ensures the integral Y1 and Y2 are taken from the background in the fit here

PP.Amplitude: -				# Sets the value of the amplitude of peak PP
PP.Amplitude_LB: -			# Sets the lower bound of the amplitude of peak PP
PP.Amplitude_UB: -			# Sets the upper bound of the amplitude of peak PP
PP.Amplitude_fixed: -			# Fixes the amplitude of peak PP (0 = free, 1 = fixed)
PP.Mean: -				# Sets the value of the amplitude of peak PP
PP.Mean_LB: -				# Sets the lower bound of the amplitude of peak PP
PP.Mean_UB: -				# Sets the upper bound of the amplitude of peak PP
PP.Mean_fixed: -			# Fixes the amplitude of peak PP (0 = free, 1 = fixed)
PP.Width: -						# Sets the value of the amplitude of peak PP
PP.Width_LB: -				# Sets the lower bound of the amplitude of peak PP
PP.Width_UB: -				# Sets the upper bound of the amplitude of peak PP
PP.Width_fixed: -			# Fixes the amplitude of peak PP (0 = free, 1 = fixed)
PP.Doublet: -				# Specifies whether peak PP is a doublet or not

GuessWidth: -				# The default guess for the width of the peaks (not bound peaks)
GuessWidth_LB: -			# The LB guess for the width of the peaks
GuessWidth_UB: -			# The UB guess for the width of the peaks

BoundPeakWidth: -			# The guess for the bound peak width (parameter 0 in the fits)
BoundPeakWidth_LB: -			# The lower bound for the bound-state-peak width
BoundPeakWidth_UB: -			# The upper bound for the bound-state-peak width
BoundPeakWidth_fixed: -			# Fix the bound-state-peak width (0 = free, 1 = fixed)

GuessAmplitudeFraction_LB: -		# Sets a lower bound on the amplitude expressed as a fraction of the number of counts in the bin containing the bin
GuessAmplitudeFraction_UB: -		# Sets an upper bound on the amplitude expressed as a fraction of the number of counts in the bin containing the bin
GuessMeanHalfWidth: -			# Subtracted and added to the mean to create the LB and UB for the mean of each peak

PrintPS: -				# Print the final spectrum in the PS format
PrintEPS: -				# Print the final spectrum in the EPS format
PrintPDF: -				# Print the final spectrum in the PDF format
PrintSVG: -				# Print the final spectrum in the SVG format
PrintTEX: -				# Print the final spectrum in the TEX format
PrintGIF: -				# Print the final spectrum in the GIF format
PrintXPM: -				# Print the final spectrum in the XPM format
PrintPNG: -				# Print the final spectrum in the PNG format
PrintJPG: -				# Print the final spectrum in the JPG format
PrintTIFF: -				# Print the final spectrum in the TIFF format
PrintCXX: -				# Print the final spectrum in the CXX format
PrintXML: -				# Print the final spectrum in the XML format
PrintJSON: -				# Print the final spectrum in the JSON format
PrintROOT: -				# Print the final spectrum in the ROOT format

PrintFileName: -			# The file name of the spectrum when printed (no file extension to be added)
CanvasWidth: -				# The width of the canvas in pixels
CanvasHeight: -				# The height of the canvas in pixels
CanvasTitle: -				# The title of the canvas
XAxisTitle: -				# The X axis title
YAxisTitle: -				# The Y axis title
XAxisLB: -				# The lower bound of the X axis
XAxisUB: -				# The upper bound of the X axis

InteractiveMode: -			# Turns on interactive mode (opens the canvas in the interactive window)
```
