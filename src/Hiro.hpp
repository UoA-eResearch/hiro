/*------------------------------------------------------------------------

	Hiro.hpp

	C++ implementation of Hiro Matsumoto's Shore Platform Model with Cosmogenic Isotope production.

	Last modified by Hiro Matsumoto on 03-10-2017

	Matsumoto, H., Dickson, M. E., & Kench, P. S. (2016a)
	An exploratory numerical model of rocky shore profile evolution.
	Geomorphology http://doi.org/10.1016/j.geomorph.2016.05.017

	Matsumoto, H., Dickson, M.E., and Kench, P.S. (2016b)
	Modelling the Development of Varied Shore Profile Geometry on Rocky Coasts.
	Journal of Coastal Research http://dx.doi.org/10.2112/SI75-120.1

	Martin D. Hurst, University of Glasgow
	Hironori Matsumoto, University of Auckland
	Mark Dickson, University of Auckland

	March 2017

	Copyright (C) 2017, Hiro Matsumoto, Martin Hurst

	Developer can be contacted
	martin.hurst@glasgow.ac.uk

	Martin D. Hurst
	School of Geographical and Earth Sciences
	University of Glasgow
	Glasgow
	Scotland
	G12 8QQ

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

------------------------------------------------------------------------*/

/** @file Hiro.hpp
@author Martin D. Hurst, University of Glasgow

@version Version 0.0.1
@brief Hiro object for simulating the evolution of rocky coastal profile
@details This object contains a simple geomorphic model for the evolution of a rocky
  coastal profile
*/

/**
@mainpage
This is the documentation for the Hiro model
These pages describe the software.
@author Martin D. Hurst, University of Glasgow

------------------------------------------------------------------------*/

#ifndef Hiro_HPP
#define Hiro_HPP

#include <vector>

using namespace std;

/*/////////////////////////////////////////////////////////////////////////////////////////
//TEMPLATES
/////////////////////////////////////////////////////////////////////////////////////////*/

//Dummy class for RockyCoastCRN
class RockyCoastCRN;

///@brief Main coastal platform object.
class Hiro
{
  friend class RockyCoastCRN;

	private:

	  /* MEMBER DECLARATIONS */

		// SPATIAL DOMAIN DECLARATIONS
		int NXNodes, NZNodes;               // Number of nodes across the coastline
		double dZ, dX;                      // Vertical and horizontal spacing of nodes
		double InitialGradient;			//initial gradient m/m

		vector<double> Z;			// elevation (m)
		vector<double> Xz;	   // cross shore distance at each elevation (m)

		vector <double> X;		// cross shore distance (m)
		vector <double> Zx;		// elevation at each cross shore distance(m)

		vector <int> XInd;		//Indices of coastal cells in Z direction
		vector <int> ZInd;		//Indices of coastal cells in X direction

		vector< vector<int> > MorphologyArray;		// array to store morphology
		vector< vector<double> > ResistanceArray;	// array to store resistance

		// Local Angle Array
		vector< vector<double> > LocalangleArray;	    // array to store localangle


		//Positions of min and max tide in X and Z
		int MinTideXInd, MaxTideXInd, MinTideZInd, MaxTideZInd;

		//Position of most landward eroded cell
		int MaxXXInd, MaxXZInd;

		//Surf zone properties for wave transformation
		double SurfZoneGradient;
		double SurfZoneWidth;

		// PROCSES DOMAIN DECLARTIONS
		vector<double> Bw_Erosion;		//Back wear erosion
		vector<double> Dw_Erosion;		//Down wear erosion
		vector<double> Weathering;		//Wearthering erosion

		int CosmoFlag;

		// SEA LEVEL DECLARATIONS
		//vector<double> RSLTime;             //Times for relative sea level elevations
		//vector<double> RSLRate;             //Relative sea level elevations, will be length[1] if constant?
		double SeaLevelRise;
		double SeaLevel;
		int SeaLevelInd;
		double SLR_sum;

		// TECTONIC UPLIFT
        int TT;
        double UpliftAmplitude;


		// TIDES DECLARATIONS
		double TidalRange;              		//Tidal Range in metres
//		double TidalPeriod;                 //Tidal Period
//		vector<double> TideLevels;          //Vector of tide levels
//		vector<double> WaterLevels;         //vector containing water levels
//		vector<double> WaterDepths;         //vector containing water depths
		int NTideValues;                //number of tidal values (.size() of tidelevels vector)
		int WaterLevelYInd, WaterLevelXInd;

		// WAVE DECLARATIONS
		double MeanWavePeriod;
		double StdWavePeriod;
	  	double MeanWaveHeight;
  		double StdWaveHeight;
  		double WavePeriod;
		double WaveHeight;
		double BreakingWaveHeight, BrokenWaveHeight;
		double BreakingWaveDist, BrokenWaveDist;
		double BreakingWaveWaterDepth;
		double BreakingWaveAttenuation, BrokenWaveAttenuation;
		double BreakingPointX, BreakingPointZ;
		int BreakingPointXInd, BreakingPointZInd;


		//TIME CONTROL PARAMETERS
		double Time, EndTime, TimeInterval;
		double PrintTime, PrintInterval;

		//PHYSICAL CONSTANTS
		double rho_w;
		double g;

		//Constants and controlling parameters
		//These should be read from an input file?
		double SubmarineDecayConst, StandingWaveConst;
		double BreakingWaveConst, BrokenWaveConst;
		double BreakingWaveDecay, BrokenWaveDecay;
		double WeatheringConst;
		double RockResistance;

		//Wave pressure parameters, check these with Hiro at some point
		double StandingWavePressure_Bw, BreakingWavePressure_Bw, BrokenWavePressure_Bw;
		double StandingWavePressure_Dw, BreakingWavePressure_Dw, BrokenWavePressure_Dw;

		//Wave height attenuation constant
		double WaveAttenuConst;

		//downwear decay const as a function of water depth
		//depends on wave height so defined inline
		double DepthDecay;
		double CliffWeatheringRate;
		double CliffFailureDepth;
		double CliffHeight;

		//This will need to be populated in the initialise tides function
		vector<double> WeatheringEfficacy;
		vector<double> ErosionShapeFunction;
		vector<double> WavePressure;
		vector<double> StandingWavePressure;
		vector<double> BreakingWavePressure;
		vector<double> BrokenWavePressure;

		//New
		vector<double> BreakingWaveDist_new;
		vector<double> BreakingWaveConst_new;

		int PressureDistMinInd, PressureDistMaxInd;

		double NDV;    // No data value


        /* FUNCTION DECLARATIONS */

		//Initialise Functions
		void Initialise();
		void Initialise(double dZ, double dX);
		void Initialise(double dZ_in, double dX_in, double Gradient, double CliffHeight);

	protected:

	public:

		/* PlatformCRN Initialisation functions */

		/// @brief Empty initialisation function for Hiro
		/// @author Martin D. Hurst
		/// @date 27/02/2017
		Hiro()
		{
			Initialise();
		}

		Hiro(double dZ, double dX)
		{
			Initialise(dZ, dX);
		}

		Hiro(double dZ, double dX, double Gradient, double CliffHeight)
		{
			Initialise(dZ, dX, Gradient, CliffHeight);
		}

		void ResetModel()
		{
			Initialise(dZ,dX, InitialGradient, CliffHeight);
		}

		//Initialise Tides
		void InitialiseTides(double TideRange);

		//Initialise Waves
		void InitialiseWaves(double WaveHeight_Mean, double WaveHeight_StD, double WavePeriod_Mean, double WavePeriod_StD);

		//Update Sea Level
		void InitialiseSeaLevel(double SLR);
		void UpdateSeaLevel();
		void UpdateSeaLevel(double InputSeaLevel);
        void UpdateSeaLevel_v1(double InputSeaLevel);

		//Tectonic uplift
		void TectonicUplift(double UpliftAmplitude);

		//Sample a wave
		void GetWave();

		// Function to modify the geological parameters
		void InitialiseGeology(double CliffHeightNew, double CliffFailureDepthNew, double RockResistanceNew, double WeatheringConstNew);

		// Function to initialise weathering shape function
		void InitialiseWeathering();

		// NEW
        void InitialiseWavePressure_1(double WaveHeight_Mean);
        void InitialiseWavePressure_25(double WaveHeight_Mean);

		/// @brief Launch the main program loop to evolve Hiro coast
		/// @details This function evolves a rocky coastal platform through time.
		///	@author Martin D. Hurst
		/// @date 27/02/2017
		void EvolveCoast();

		void CalculateBackwearing();
		void CalculateDownwearing();

		//New CalculateBackwearing & CalculateDownwearing
		void CalculateBackwearing_v1(double WaveAttenuConst);
		void CalculateDownwearing_v1(double WaveAttenuConst);

		void ErodeBackwearing();
		void ErodeDownwearing();
		void MassFailure();

		void IntertidalWeathering();
		void SupratidalWeathering();

		void UpdateMorphology();

		// File name holder
		string OutputFileName;
		string OutputConcentrationFileName;

		/// @brief Writes the platform morphology to file
		/// @details This function writes the elevations of the platform surface at the current time to
		///   a file. If the file exists, this is appended.
		///	@author Martin D. Hurst
		/// @date 27/02/2017
		void WriteProfile(string OutputFileName, double Time);

		/// @brief Writes the ResistanceArray to file
		/// @details This function writes the rock Resistance at the current time to
		///   a file.
		/// @author Martin D. Hurst
		/// @date 09/03/2017
		void WriteResistanceArray(string OutputFileName, double Time);
		void WriteMorphologyArray(string OutputFileName, double Time);

		/// @brief Writes the LocalangleArray to file
		/// @details This function writes the localangle at the current time to
		///   a file.
		/// @author HIRO
		/// @date 27/09/2017
		void WriteLocalangleArray(string OutputFileName, double Time);

		/// @brief Get X coordinates
		/// @return X coordinates
		///	@author Martin D. Hurst
		/// @date 27/02/2017
		vector<double> get_X() const { return X; }

		/// @brief Get surface CRN concentration
		/// @return Surface CRN concentration
		///	@author Martin D. Hurst
		/// @date 27/02/2017
		vector<double> get_Z() const { return Z; }

		double get_SeaLevel() const { return SeaLevel; }

		void Set_InitialGradient(double Gradient)
		{
			InitialGradient = Gradient;
		}

		void Set_WaveCoefficients(double StandingWaveCoef, double BreakingWaveCoef, double BrokenWaveCoef)
		{
			StandingWaveConst = StandingWaveCoef;
			BreakingWaveConst = BreakingWaveCoef;
			BrokenWaveConst= BrokenWaveCoef;
		}
};

#endif

