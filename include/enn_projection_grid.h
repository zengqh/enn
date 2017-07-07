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
// Time: 2015/04/30
// File: enn_projection_grid.h
//

#pragma once

#include "enn_math.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

class Perlin;
class Camera;
class ProjectedGrid : public AllocatedObject
{
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
	};

public:
	struct Options
	{
		// Projected grid complexity (N*N)
		int ComplexityU, ComplexityV;

		float Strength;

		float Elevation;

		bool Smooth;

		// Force recalculate mesh geometry each frame
		bool ForceRecalculateGeometry;

		bool ChoppyWaves;

		// Choppy waves strength
		float ChoppyStrength;

		Options()
			: ComplexityU(64)
			, ComplexityV(128)
			, Strength(5.0f)
			, Elevation(5.0f)
			, Smooth(false)
			, ForceRecalculateGeometry(false)
			, ChoppyWaves(true)
			, ChoppyStrength(3.75f)
		{

		}
	};

public:
	ProjectedGrid();
	ProjectedGrid(const Options &options);

	~ProjectedGrid();

	void setHeight(float height);
	void update(float elapsedTime);

protected:
	void init();
	void initGeo();
	void destroy();

	bool getMinMax(Matrix4& range);
	void setDisplacementAmplitude(float ampl);
	Vector4 calculeWorldPosition(const Vector2 &uv, const Matrix4& m);

	void renderGeometry(const Matrix4& m, const Vector3& worldPos);
	void calcNormals();

protected:
	Options		options_;
	Vertex*		vertices_;
	Vector3		pos_, normal_;
	Vector4		t_corners0, t_corners1, t_corners2, t_corners3;
	Plane		plane_, upper_bound_, lower_bound_;
	Camera*		render_cam_;
	Perlin*		noise_;
	float		water_height_;

	D3D11ResourceProxy*		vb_;
	D3D11ResourceProxy*		ib_;
};

}