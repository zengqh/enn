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
// Time: 2014/12/19
// File: enn_sky.h
//

#pragma once

#include "enn_pass_base.h"
#include "enn_mesh_manager.h"

namespace enn
{

class Sky : public AllocatedObject
{
public:
	struct Options
	{
		// Inner atmosphere radius
		float inner_radius;

		// Outer atmosphere radius
		float outer_radius;

		// Height position, in [0, 1] range, 0=InnerRadius, 1=OuterRadius
		float height_position;

		// Rayleigh multiplier
		float rayleigh_multiplier;

		// Mie multiplier
		float mie_multiplier;

		// Sun intensity
		float sun_intensity;

		// WaveLength for RGB channels
		Vector3 wave_length;

		// Phase function
		float g;

		// Exposure coeficient
		float exposure;

		Options()
			: inner_radius(9.77501f)
			, outer_radius(10.2963f)
			, height_position(0.01f)
			, rayleigh_multiplier(0.0022f)
			, mie_multiplier(0.000675f)
			, sun_intensity(30)
			, wave_length(0.57f, 0.54f, 0.44f)
			, g(-0.991f)
			, exposure(1.5f)
		{

		}
	};

public:
	Sky();
	~Sky();

	void render();
	void renderReflection();
	void beginRender();
	void endRender();

	const Options& getOption() const { return options_; }

protected:
	void init();
	void update();
	void renderSky();
	Vector3 getColorAt(const Vector3& Direction) const;
	const float _scale(const float& cos, const float& uScaleDepth) const;

protected:
	Options				options_;
	Mesh*				sky_mesh_;
};

}