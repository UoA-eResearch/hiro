# -*- coding: utf-8 -*-
"""
Created on Mon Feb  8 15:10:34 2016 by Martin Hurst
Last modified by Hiro Matsumoto on 21-09-2017 

Script to plot the results of Hiro and RockyCoastCRN

Martin Hurst,
March 7th 2016

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage.filters import gaussian_filter1d
from matplotlib import cm
from matplotlib import rc

# Customise figure style #
rc('font',size=8)
rc('ytick.major',pad=5)
rc('xtick.major',pad=5)
padding = 5
plt.figure(1,figsize=(6,6))

# COLOR MAP
ColourMap = cm.jet

#First plot the morphology through time
#FileName = "../driver_files/ShoreProfile.xz"
FileName = "./ShoreProfile.xz"
f = open(FileName,'r')
Lines = f.readlines()
NoLines = len(Lines)

ax1 = plt.subplot(211)

#Get header info and setup X coord
Header = Lines[0].strip().split(" ")
Times = []
RetreatRate = []
CliffHeight = int(Header[0])
dz = float(Header[1])
M = np.zeros(NoLines, dtype="float64")

for j in range(1,NoLines):
    Line = (Lines[j].strip().split(" "))
    Time = float(Line[0])
    #Read morphology
    X = np.array(Line[1:],dtype="float64")
    NValues = len(X)
    M[j] = Line[1] 
    Z = np.linspace(CliffHeight,-CliffHeight,NValues)
    X = gaussian_filter1d(X, sigma=6)
    #Z = np.linspace(CliffHeight,-CliffHeight, NValues)
    ax1.plot(X,Z,'k-',lw=1.5,color=ColourMap((j)/(NoLines)))
	
	#colour = Time/StartTime
	#ax1.plot(X,Z,'-',lw=1.5,color=cm.RdBu(colour))
	#PlotTime += PlotInterval
	
	# tweak the plot
	#ax1.set_xticklabels([])
plt.xlabel("Distance (m)")
plt.ylabel("Elevation (m)")
xmin, xmax = ax1.get_xlim()
#plt.xlim(xmin-10,xmax+10)
#plt.ylim(-CliffHeight,CliffHeight)
#plt.ylim(-30,30)
#plt.tight_layout()
#plt.show()
	
#now plot CRN concentration through time
#FileName = "../driver_files/CRNConcentrations.xn"
FileName = "./Concentrations.xn"
f = open(FileName,'r')
Lines = f.readlines()
NoLines = len(Lines)

ax2 = plt.subplot(212)
#Get header info and setup X coord
Header1 = Lines[0].strip().split(" ")
Header2 = Lines[1].strip().split(" ")
m=M/dz

for j in range(2,NoLines,1):
    X = np.array((Lines[j].strip().split(" "))[1:],dtype="float64")
    px = X[1:int(m[j-1])]
    px = gaussian_filter1d(px, sigma=6)
    #N = np.array((Lines[j+1].strip().split(" "))[1:],dtype="float64")
    pn = np.arange(1,int(m[j-1]),1)*dz
    ax2.plot(pn,px,'r-',lw=1.5,color=ColourMap((j)/(NoLines)))
    
plt.xlabel('Distance (m)')
plt.ylabel('Concentration (atoms g$^{-1}$)')

plt.tight_layout()
plt.show()
