/*------------------------------------------------------------------------

	Hiro_CRN_Driver.cpp

	Last mofified 21-09-2017 by Hiro Matsumoto

	Driver file for running the shore platform model of Matsumoto et al. (2016) with Cosmogenic Isotope accumulation (Hurst et al. 2017).

	C++ implementation of Hiro Matsumoto's Shore Platform Model with Cosmogenic Isotope production.

	Matsumoto, H., Dickson, M. E., & Kench, P. S. (2016a)
	An exploratory numerical model of rocky shore profile evolution.
	Geomorphology http://doi.org/10.1016/j.geomorph.2016.05.017

	Matsumoto, H., Dickson, M.E., and Kench, P.S. (2016b)
	Modelling the Development of Varied Shore Profile Geometry on Rocky Coasts.
	Journal of Coastal Research http://dx.doi.org/10.2112/SI75-120.1

	Hurst, M.D., Rood, D.H., Ellis, M.A., Anderson, R.S., and Dornbusch, U. (2016)
	Recent acceleration in coastal cliff retreat rates on the south coast of Great Britain.
	Proceedings of the National Academy of Sciences, http://dx.doi.org/10.1073/PNAS.1613044113

	Hurst, M.D., Rood, D.H., and Ellis, M.A. (2017)
	Controls on the distribution of cosmogenic 10 Be across shore platforms
	Earth Surface Dynamics http://dx.doi.org/10.5194/esurf-5-67-2017

	Martin D. Hurst, University of Glasgow
	Hironori Matsumoto, University of Auckland

	March 2017

	Copyright (C) 2017, Martin Hurst

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

#include <fenv.h>
#include <signal.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <unistd.h>
#include "RockyCoastCRN.hpp"
#include "Hiro.hpp"

using namespace std;

void handler(int sig)
{
    printf("Floating Point Exception\n");
    sig += 0;
    exit(0);
}

int main()
{
	//feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
    signal(SIGFPE, handler);

	//initialisation parameters
	double dZ = 0.1;
	double dX = 0.1;
    //double PlatformGradient = 1./10.;
    double Gradient = 1.;
    //double CliffPositionX = 0.;
	double CliffHeight = 10.;

	//Time control parameters
	double EndTime = 1000.;
	double Time = 0.;
	double TimeInterval = 1;

	//Print Control
	double PrintInterval = 100.;
	double PrintTime = Time;
	string OutputMorphologyFileName = "ShoreProfile.xz";
	string OutputConcentrationFileName = "Concentrations.xn";

	//initialise Hiro Model
	Hiro PlatformModel = Hiro(dZ, dX, Gradient, CliffHeight);

	//Which Nuclides to track 10Be, 14C, 26Al, 36Cl?
	vector<int> Nuclides;
	Nuclides.push_back(10);
	//Nuclides.push_back(14);

	//initialise RockyCoastCRN friend object
	RockyCoastCRN PlatformCRN = RockyCoastCRN(PlatformModel, Nuclides);

	//Initialise Tides
	double TidalRange = 2;
	double TidalPeriod = 12.42;
	PlatformModel.InitialiseTides(TidalRange);
	PlatformCRN.InitialiseTides(TidalRange/2.,TidalPeriod);

	//Set sea level rise rate
	double SLR = 0;
	PlatformModel.InitialiseSeaLevel(SLR);

	//Setup Morphology
	//Populate geometric metrics
	PlatformModel.UpdateMorphology();

	//Initialise Waves
	//Single Wave for now but could use the waveclimate object from COVE!?
	double WaveHeight_Mean = 1.;
	double WaveHeight_StD = 0;
	double WavePeriod_Mean = 6.;
	double WavePeriod_StD = 0;
	PlatformModel.InitialiseWaves(WaveHeight_Mean, WaveHeight_StD, WavePeriod_Mean, WavePeriod_StD);


    // Wave coefficient constant
	double StandingCoefficient = 0.01;
	double BreakingCoefficient = 1.;
	double BrokenCoefficient = 1.;
	PlatformModel.Set_WaveCoefficients(StandingCoefficient, BreakingCoefficient, BrokenCoefficient);

	//reset the geology
	double CliffFailureDepth = 1.;
	double Resistance = 0.01;
	double WeatheringRate = 0.001;
	PlatformModel.InitialiseGeology(CliffHeight, CliffFailureDepth, Resistance, WeatheringRate);




	//Loop through time
	while (Time <= EndTime)
	{
		//Update Sea Level
		PlatformModel.UpdateSeaLevel();

		//Get the wave conditions
		PlatformModel.GetWave();

		//Calculate forces acting on the platform
		PlatformModel.CalculateBackwearing();
		PlatformModel.CalculateDownwearing();

		//Do erosion
		PlatformModel.ErodeBackwearing();
		PlatformModel.ErodeDownwearing();

		//Implement Weathering
		PlatformModel.IntertidalWeathering();

		//Update the Morphology
		PlatformModel.UpdateMorphology();

		//Check for Mass Failure
		PlatformModel.MassFailure();

		//Update the morphology inside RockyCoastCRN
		PlatformCRN.UpdateMorphology(PlatformModel);

		//Update the CRN concentrations
		PlatformCRN.UpdateCRNs();

		//print?
		if (Time >= PrintTime)
		{
			//Create string stream for puting time into filename
//			stringstream ss;
//			ss << PrintTime;
//			string PrintTimeString = ss.str();
//			ss.clear();
//			string ConcentrationsFileName ="Concentrations_"+PrintTimeString+".xzn";
			//PlatformCRN.WriteNuclideArray(ConcentrationsFileName, Time, Nuclides[0]);
			PlatformModel.WriteProfile(OutputMorphologyFileName, Time);
			PlatformCRN.WriteCRNProfile(OutputConcentrationFileName, Time);
			PrintTime += PrintInterval;
		}

		//update time
		Time += TimeInterval;
	}

	string ResistanceFileName = "ResistanceArray.xz";
	string MorphologyFileName = "MorphologyArray.xz";

	PlatformModel.WriteResistanceArray(ResistanceFileName, Time);
	PlatformModel.WriteMorphologyArray(MorphologyFileName, Time);


	//a few blank lines to finish
	cout << endl << endl;

}


