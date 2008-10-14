// -*- mode: c++; mode: auto-fill; mode: flyspell-prog; -*-
/*
 *   This source code is part of the Eutelescope package of Marlin.
 *   You are free to use this source files for your own development as
 *   long as it stays in a public research context. You are not
 *   allowed to use it for commercial purpose. You must put this
 *   header with author names in all development based on this file.
 *
 */
// root includes
#include <TFile.h>
#include <TList.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TString.h>
#include <TPaveLabel.h>
#include <TPaveStats.h>
#include <TDirectoryFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TProfile.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>
#include <TCutG.h>
#include <TSystem.h>

// system includes
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// global variables
//! Global variable containing the run name
TString runName = "Unknown run";

//! Global variable with the name of pedestal processor folder
TString pedeProcessorFolderName = "PedestalAndNoiseCalculator";

//! Global variable with the cluster histogramming processor folder
TString clusterHistoFolderName = "FillHisto";

//! Global variable with the Eta calculator folder
TString etaFolderName = "ETA";

//! Global variable with the Correlator folder
TString correlatorFolderName = "Correlator";

// real function prototypes
//! Show cluster plot
/*! This function can be used to plot in a nice way the standard
 *  control histograms concerning clusters.
 *
 *  The function will open the ROOT file specified by \a filename and
 *  look for the folder containing the cluster histograms. The name of
 *  this folder can be changed by the user using the \a
 *  clusterHistoFolderName global variable.
 *
 *  The following canvases will be plotted and saved as picture files:
 *
 *  \li <b>SNRCanvas</b>. This canvas contains two histograms for each
 *  detector up to three detectors. If the total number of sensors is
 *  exceeding 3, another canvas will be created. The histogram on the
 *  left hand side is Seed pixel signal to noise ratio distribution,
 *  the one on the right hand side is the cluster 3x3 SNR. Both
 *  distribution are drawn along with their Landau fit functions.
 *
 *  \li <b>HitMapCanvas</b>. This canvas contains one 2D histogram for
 *  each detector up to six detectors. If the total number of sensors is
 *  exceeding 6, another canvas will be created. Each histogram
 *  represents the seed pixel position accumulated overall the
 *  events. The color code is also draw.
 *
 *  \li <b>NoiseCanvas</b>. This canvas has not to be confused with
 *  the single pixel noise canvas generated by showPedeNoisePlot. It
 *  draws one histogram per sensor, up to 6 sensors per canvas. Each
 *  histogram represents the cluster noise distribution calculated as
 *  the sum in quadrature of all the pixels belonging to the
 *  clusters. Since not all clusters are made by the same amount of
 *  pixels, having a multi-peaked distribution is normal.
 *
 *  @param filename The ROOT file name.
 *
 */
void showClusterPlot( const char * filename );



//! Show pedestal and noise plot
/*! This function can be used to plot in a nice way the standard
 *  control histograms concerning pedestal and noise.
 *
 *  The function will open the ROOT file specified by \a filename and
 *  look for the folder containing the pedestal, noise and status
 *  histograms. The name of this folder can be changed by the user
 *  using the pedeProcessorFolderName global variable.
 *
 *  The following canvases will be plotted and saved as picture files:
 *
 *  \li <b>PedeCanvas</b> This canvas contains two histograms for each
 *  sensor, up to three sensors per canvas. The 2D histogram on the
 *  left represents the pedestal distribution on the sensors surface,
 *  while the 1D plot is the pedestal distribution.
 *
 *  \li <b>NoiseCanvas</b> Same as the PedeCanvas but now representing
 *  noise figures instead of pedestal values.
 *
 *  \li <b>StatusCanvas</b> This canvas contains one 2D histogram for
 *  each detector up to six sensors per canvas. The red dots
 *  corresponds to masked pixels.
 *
 *  @param filename The ROOT file name.
 */
void showPedeNoisePlot( const char * filename,  const char * detector = "mimotel"  );

//! Show Eta function plot
/*! This function can be used to plot in a nice way the standard
 *  control histograms concerning Eta function.
 *
 *  The function will open the ROOT file specified by \a filename and
 *  look for the folder containing the Eta histograms. The name of
 *  this folder can be changed by the user using the
 *  etaProcessorFolderName global variable.
 *
 *  The following canvases will be plotted and saved as picture files:
 *
 *  \li <b>EtaXCanvas</b>. This canvas contains two histograms for
 *  each sensor, up to three sensors per canvas. The plot on left is
 *  the cluster center position as calculated by the Center Of Gravity
 *  algorithm. The plot on the right hand side is the corresponding
 *  Eta function. All these plots are related to the X direction.
 *
 *  \li <b>EtaYCanvas</b>. As the EtaXCanvas, but related to the Y
 *  direction.
 *
 *  \li <b>CoGCanvas</b>. This canvas contains one 2D histograms for
 *  each sensor, up to six sensors per canvas. The histogram
 *  represents the center of gravity position of the cluster center
 *  relative to the central pixel.
 *
 *  \param filename The ROOT file name.
 */
void showEtaPlot( const char * filename );

//! Show correlation plot
/*! This function can be used to plot in a nice way the standard
 *  control histrograms concerning Correlator processor.
 *
 *  Depending if the correlation is done on clusters or hits or both,
 *  the function will display one or two series of canvases. For each
 *  canvases, the user can see the 2D correlation plot and the
 *  straight line fit resulting from the projection of this 2D plot
 *  onto the x axis.
 *
 *  In order not to be overwhelmed by the combinatorial background,
 *  the function is automatically guessing a region of interest around
 *  the correlation line and projecting only these values. The ROI is
 *  identified on the 2D plot with a dashed spline. 
 *
 *  The correlation histograms are shown for following telescope
 *  plane, it is to say plane N vs plane N+1. So if there are 6 planes
 *  in the setup there will be only 5 correlation histograms with the
 *  corresponding fits.
 */
void showCorrelationPlot( const char * filename );

// utility prototypes
//! Open a ROOT file
/*! This utility function is used to open a ROOT file, if this is not
 *  yet opened. In case, the file is opened but a Zombie, than it is
 *  closed and re-opened.
 */
TFile * closeAndReopenFile( const char * filename );

//! Set the run name for canvas title
/*! This function can be used to set the global variable \a
 *  runName. This string is automatically added at the canvas title
 *  pad.
 */
void setRunName(const char * name );

//! Close canvases
/*! Before opening a new canvas, the user should call this function in
 *  order to close all the other canvases having the same name to
 *  avoid confusion.
 */
void closeCanvases( const char * canvasBaseName );

//! Prepare the output folder
/*! Each show plot function is saving the canvases as figure. This
 *  function is used to create a folder named after the \a runName and
 *  another folder named according to the kind of plots.
 */
const char * prepareOutputFolder( const char * type );

//! Usage
/*! Simple function to print out which are the functions available.
 */
void usage();

//! Convert to string
/*! This template function is used to convert to string any type
 *  having a streamer. This utility can be used to "sum" numbers to
 *  string, for example.
 */
template<typename T>
std::string toString(T data ) {
  std::stringstream ss;
  ss << data ;
  return ss.str();

}




//  LocalWords:  filename pedeProcessorFolderName PedeCanvas NoiseCanvas
//  LocalWords:  StatusCanvas mimotel etaProcessorFolderName iostream sstream
//  LocalWords:  PedestalAndNoiseCalculator FillHisto histogramming