//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2015/05/04
// File: enn_perlin.h
//

#pragma once

#include "enn_math.h"

#define n_bits				5
#define n_size				(1<<(n_bits-1))
#define n_size_m1			(n_size - 1)
#define n_size_sq			(n_size*n_size)
#define n_size_sq_m1		(n_size_sq - 1)

#define n_packsize			4

#define np_bits				(n_bits+n_packsize-1)
#define np_size				(1<<(np_bits-1))
#define np_size_m1			(np_size-1)
#define np_size_sq			(np_size*np_size)
#define np_size_sq_m1		(np_size_sq-1)

#define n_dec_bits			12
#define n_dec_magn			4096
#define n_dec_magn_m1		4095

#define max_octaves			32

#define noise_frames		256
#define noise_frames_m1		(noise_frames-1)

#define noise_decimalbits	15
#define noise_magnitude		(1<<(noise_decimalbits-1))

#define scale_decimalbits	15
#define scale_magnitude		(1<<(scale_decimalbits-1))

namespace enn
{

class Perlin : public AllocatedObject
{
public:
	struct Options
	{
		/// Octaves
		int Octaves;
		/// Scale
		float Scale;
		/// Falloff
		float Falloff;
		/// Animspeed
		float Animspeed;
		/// Timemulti
		float Timemulti;

		/** Default constructor
		*/
		Options()
			: Octaves(8)
			, Scale(0.085f)
			, Falloff(0.49f)
			, Animspeed(1.4f)
			, Timemulti(1.27f)
		{
		}

		/** Constructor
		@param _Octaves Perlin noise octaves
		@param _Scale Noise scale
		@param _Falloff Noise fall off
		@param _Animspeed Animation speed
		@param _Timemulti Timemulti
		*/
		Options(const int   &_Octaves,
			const float &_Scale,
			const float &_Falloff,
			const float &_Animspeed,
			const float &_Timemulti)
			: Octaves(_Octaves)
			, Scale(_Scale)
			, Falloff(_Falloff)
			, Animspeed(_Animspeed)
			, Timemulti(_Timemulti)
		{

		}
	};

	/** Default constructor
	*/
	Perlin();

	/** Constructor
	@param Options Perlin noise options
	*/
	Perlin(const Options &Options);

	/** Destructor
	*/
	~Perlin();

	/** Call it each frame
	@param timeSinceLastFrame Time since last frame(delta)
	*/
	void update(const float &timeSinceLastFrame);

	/** Get the especified x/y noise value
	@param x X Coord
	@param y Y Coord
	@return Noise value
	@remarks range [~-0.2, ~0.2]
	*/
	float getValue(const float &x, const float &y);

	/** Set/Update perlin noise options
	@param Options Perlin noise options
	@remarks If create() have been already called, Octaves option doesn't be updated.
	*/
	void setOptions(const Options &Options);

	/** Get current Perlin noise options
	@return Current perlin noise options
	*/
	inline const Options& getOptions() const
	{
		return mOptions;
	}

private:
	void init();
	void deinit();

	/** Initialize noise
	*/
	void _initNoise();

	/** Calcule noise
	*/
	void _calculeNoise();

	/** Read texel linear dual
	@param u u
	@param v v
	@param o Octave
	@return int
	*/
	int _readTexelLinearDual(const int &u, const int &v, const int &o);

	/** Read texel linear
	@param u u
	@param v v
	@return Heigth
	*/
	float _getHeigthDual(float u, float v);

	/** Map sample
	@param u u
	@param v v
	@param level Level
	@param octave Octave
	@return Map sample
	*/
	int _mapSample(const int &u, const int &v, const int &upsamplepower, const int &octave);

	/// Perlin noise variables
	int noise[n_size_sq*noise_frames];
	int o_noise[n_size_sq*max_octaves];
	int p_noise[np_size_sq*(max_octaves >> (n_packsize - 1))];
	int *r_noise;
	float magnitude;

	/// Elapsed time
	double v_time;

	/// Perlin noise options
	Options mOptions;

	bool mInited;
};

}