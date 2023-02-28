/* ------------------------------------------------------------
name: "untitled5"
Code generated with Faust 2.58.8 (https://faust.grame.fr)
Compilation options: -a minimal-fixed-point.cpp -lang cpp -ct 1 -cn fxdsp -es 1 -mcd 16 -fx -ftz 0
------------------------------------------------------------ */

#ifndef  __fxdsp_H__
#define  __fxdsp_H__

/************************************************************************
 IMPORTANT NOTE : this file contains two clearly delimited sections :
 the ARCHITECTURE section (in two parts) and the USER section. Each section
 is governed by its own copyright and license. Please check individually
 each section for license and copyright information.
 *************************************************************************/

/******************* BEGIN minimal-fixed-point.cpp ****************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2019 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 
 ************************************************************************
 ************************************************************************/
 
#include <algorithm>
#include <cmath>

#include "faust/gui/PrintUI.h"
#include "faust/gui/UI.h"
#include "faust/gui/meta.h"
#include "faust/dsp/dsp.h"
#include "faust/audio/dummy-audio.h"
#include "faust/dsp/one-sample-dsp.h"

#if defined(SOUNDFILE)
#include "faust/gui/SoundUI.h"
#endif

#include "ap_fixed.h"

typedef ap_fixed<32, 8, AP_RND_CONV, AP_SAT> fixpoint_t;

// fx version
inline fixpoint_t fabsfx(fixpoint_t x)
{
    return fixpoint_t(std::fabs(float(x)));
}
inline fixpoint_t acosfx(fixpoint_t x)
{
    return fixpoint_t(std::acos(float(x)));
}
inline fixpoint_t asinfx(fixpoint_t x)
{
    return fixpoint_t(std::asin(float(x)));
}
inline fixpoint_t atanfx(fixpoint_t x)
{
    return fixpoint_t(std::atan(float(x)));
}
inline fixpoint_t atan2fx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::atan2(float(x), float(y)));
}
inline fixpoint_t ceilfx(fixpoint_t x)
{
    return fixpoint_t(std::ceil(float(x)));
}
inline fixpoint_t cosfx(fixpoint_t x)
{
    return fixpoint_t(std::cos(float(x)));
}
inline fixpoint_t expfx(fixpoint_t x)
{
    return fixpoint_t(std::exp(float(x)));
}
inline fixpoint_t exp2fx(fixpoint_t x)
{
    return fixpoint_t(std::exp2(float(x)));
}
inline fixpoint_t exp10fx(fixpoint_t x)
{
#ifdef __APPLE__
    return fixpoint_t(__exp10f(float(x)));
#else
    return fixpoint_t(exp10(float(x)));
#endif
}
inline fixpoint_t floorfx(fixpoint_t x)
{
    return fixpoint_t(std::floor(float(x)));
}
inline fixpoint_t fmodfx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::fmod(float(x), float(y)));
}
inline fixpoint_t logfx(fixpoint_t x)
{
    return fixpoint_t(std::log(float(x)));
}
inline fixpoint_t log2fx(fixpoint_t x)
{
    return fixpoint_t(std::log2(float(x)));
}
inline fixpoint_t log10fx(fixpoint_t x)
{
    return fixpoint_t(std::log10(float(x)));
}
inline fixpoint_t powfx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::pow(float(x), float(y)));
}
inline fixpoint_t remainderfx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::remainder(float(x), float(y)));
}
inline fixpoint_t rintfx(fixpoint_t x)
{
    return fixpoint_t(std::rint(float(x)));
}
inline fixpoint_t roundfx(fixpoint_t x)
{
    return fixpoint_t(std::round(float(x)));
}
inline fixpoint_t sinfx(fixpoint_t x)
{
    return fixpoint_t(std::sin(float(x)));
}
inline fixpoint_t sqrtfx(fixpoint_t x)
{
    return fixpoint_t(std::sqrt(float(x)));
}
inline fixpoint_t tanfx(fixpoint_t x)
{
    return fixpoint_t(std::tan(float(x)));
}
// min/max
inline fixpoint_t fminfx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::min(float(x), float(y)));
}
inline fixpoint_t fmaxfx(fixpoint_t x, fixpoint_t y)
{
    return fixpoint_t(std::max(float(x), float(y)));
}

/******************************************************************************
 *******************************************************************************
 
 VECTOR INTRINSICS
 
 *******************************************************************************
 *******************************************************************************/


/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS fxdsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif


class fxdsp : public dsp {
	
 private:
	
	int fSampleRate;
	fixpoint_t fConst1;
	fixpoint_t fRec0[2];
	fixpoint_t fConst2;
	
 public:
	fxdsp() {}

	void metadata(Meta* m) { 
		m->declare("compile_options", "-a minimal-fixed-point.cpp -lang cpp -ct 1 -cn fxdsp -es 1 -mcd 16 -fx -ftz 0");
		m->declare("filename", "untitled5.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.5");
		m->declare("name", "untitled5");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/saw2ptr:author", "Julius O. Smith III");
		m->declare("oscillators.lib/saw2ptr:license", "STK-4.3");
		m->declare("oscillators.lib/version", "0.3");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.3");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fixpoint_t fConst0 = std::min<fixpoint_t>(fixpoint_t(1.92e+05), std::max<fixpoint_t>(fixpoint_t(1.0), fixpoint_t(fSampleRate)));
		fConst1 = fixpoint_t(4.4e+02) / fConst0;
		fConst2 = fixpoint_t(1.0) - fixpoint_t(0.0022727272) * fConst0;
	}
	
	virtual void instanceResetUserInterface() {
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec0[l0] = 0;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual fxdsp* clone() {
		return new fxdsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("untitled5");
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fixpoint_t fTemp0 = fConst1 + fRec0[1] + fixpoint_t(-1.0);
			int iTemp1 = fTemp0 < fixpoint_t(0.0);
			fixpoint_t fTemp2 = fixpoint_t(fConst1 + fRec0[1]);
			fRec0[0] = ((iTemp1) ? fTemp2 : fixpoint_t(fTemp0));
			fixpoint_t fRec1 = ((iTemp1) ? fTemp2 : fixpoint_t(fConst1 + fRec0[1] + fConst2 * fTemp0));
			output0[i0] = FAUSTFLOAT(fixpoint_t(2.0) * fRec1 + fixpoint_t(-1.0));
			fRec0[1] = fRec0[0];
		}
	}

};

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/******************* END minimal-fixed-point.cpp ****************/

#endif
