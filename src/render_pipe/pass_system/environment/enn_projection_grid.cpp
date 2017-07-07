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
// File: enn_projection_grid.cpp
//

#include "enn_projection_grid.h"
#include "enn_perlin.h"
#include "enn_render_pipe.h"

namespace enn
{

ProjectedGrid::ProjectedGrid()
: vertices_(0)
, normal_(0, 1, 0)
{
	init();
	initGeo();

	setHeight(0);
}

ProjectedGrid::ProjectedGrid(const Options &options)
: vertices_(0)
, normal_(0, 1, 0)
{
	init();

	setHeight(0);
}

ProjectedGrid::~ProjectedGrid()
{
	
}

void ProjectedGrid::setHeight(float height)
{
	noise_ = ENN_NEW Perlin();

	vertices_ = ENN_MALLOC_ARRAY_T(Vertex, options_.ComplexityU * options_.ComplexityV);

	for (int i = 0; i < options_.ComplexityU * options_.ComplexityV; i++)
	{
		vertices_[i].normal = Vector3::NEGATIVE_UNIT_Y;
	}

	render_cam_ = g_render_pipe_->getMainCam();
}

void ProjectedGrid::update(float elapsedTime)
{
	noise_->update(elapsedTime);

	const Vector3& RenderCameraPos = render_cam_->getEyePos();

	float far_clip = render_cam_->getZFar();

	Matrix4 range;

	if (getMinMax(range))
	{
		renderGeometry(range, RenderCameraPos);

		Vertex* vx = (Vertex*)g_render_device_->lock(vb_);
		memcpy(vx, vertices_, options_.ComplexityU * options_.ComplexityV * sizeof(Vertex));
		g_render_device_->unLock(vb_);
	}
}

void ProjectedGrid::init()
{
	noise_ = ENN_NEW Perlin;

	vertices_ = ENN_MALLOC_ARRAY_T(Vertex, options_.ComplexityU * options_.ComplexityV);

	for (int i = 0; i < options_.ComplexityU * options_.ComplexityV; i++)
	{
		vertices_[i].normal = Vector3::NEGATIVE_UNIT_Y;
	}

	render_cam_ = g_render_pipe_->getMainCam();
}

void ProjectedGrid::initGeo()
{
	int iVertexCount = options_.ComplexityU * options_.ComplexityV;
	int iIndexCount = 6 * (options_.ComplexityU - 1) * (options_.ComplexityV - 1);

	// create vertex buffer.
	vb_ = g_render_device_->createVertexBuffer(0, iVertexCount * sizeof(Vertex), true);

	short* index_buffer = ENN_MALLOC_ARRAY_T(short, iIndexCount);

	{
		int i = 0;
		for (int v = 0; v < options_.ComplexityV - 1; v++)
		{
			for (int u = 0; u < options_.ComplexityU - 1; u++)
			{
				index_buffer[i++] = v * options_.ComplexityU + u;
				index_buffer[i++] = v * options_.ComplexityU + u + 1;
				index_buffer[i++] = (v + 1) * options_.ComplexityU + u;

				// face 2 /|
				index_buffer[i++] = (v + 1) * options_.ComplexityU + u;
				index_buffer[i++] = v * options_.ComplexityU + u + 1;
				index_buffer[i++] = (v + 1) * options_.ComplexityU + u + 1;
			}
		}
	}

	// create index buffer
	ib_ = g_render_device_->createIndexBuffer(index_buffer, iIndexCount * sizeof(short), false);

	ENN_SAFE_FREE(index_buffer);
}

void ProjectedGrid::destroy()
{
	ENN_SAFE_FREE(vertices_);
}

bool ProjectedGrid::getMinMax(Matrix4& range)
{
	setDisplacementAmplitude(options_.Strength);

	float x_min, y_min, x_max, y_max;
	Vector3 frustum[8], proj_points[24];

	int n_points = 0;

	int cube[] = {
		0, 1, 0, 2, 2, 3, 1, 3,
		0, 4, 2, 6, 3, 7, 1, 5,
		4, 6, 4, 5, 5, 7, 6, 7
	};	// which frustum points are connected together?

	Matrix4 matInvVP = render_cam_->getViewProjMatrix();
	matInvVP.inverse();

	frustum[0] = matInvVP.transformAffine(Vector3(-1, -1, -1));
	frustum[1] = matInvVP.transformAffine(Vector3(+1, -1, -1));
	frustum[2] = matInvVP.transformAffine(Vector3(-1, +1, -1));
	frustum[3] = matInvVP.transformAffine(Vector3(+1, +1, -1));
	frustum[4] = matInvVP.transformAffine(Vector3(-1, -1, 1));
	frustum[5] = matInvVP.transformAffine(Vector3(+1, -1, 1));
	frustum[6] = matInvVP.transformAffine(Vector3(-1, +1, 1));
	frustum[7] = matInvVP.transformAffine(Vector3(+1, +1, 1));

	for (int i = 0; i < 12; i++)
	{
		int src = cube[i * 2], dst = cube[i * 2 + 1];

		Ray ray(frustum[src], frustum[dst] - frustum[src]);

		float dist = (frustum[src] - frustum[dst]).length();

		std::pair<bool, float> info = ray.intersects(upper_bound_);

		if (info.first && info.second < dist + Math::ENN_EPSILON)
		{
			proj_points[n_points++] = frustum[src] + info.second * ray.getDirection();
		}

		info = ray.intersects(lower_bound_);

		if (info.first && info.second < dist + Math::ENN_EPSILON)
		{
			proj_points[n_points++] = frustum[src] + info.second * ray.getDirection();
		}
	}

	// check if any of the frustums vertices lie between the upper_bound and lower_bound planes
	{
		for (int i = 0; i < 8; i++)
		{
			float d0 = upper_bound_.getDistance(frustum[i]);
			float d1 = lower_bound_.getDistance(frustum[i]);

			if (d0 / d1 < 0)
			{
				proj_points[n_points++] = frustum[i];
			}
		}
	}

	{
		for (int i = 0; i < n_points; i++)
		{
			// project the point onto the surface plane
			proj_points[i] = proj_points[i] - normal_ * plane_.getDistance(proj_points[i]);
		}
	}

	{
		for (int i = 0; i < n_points; i++)
		{
			proj_points[i] = render_cam_->getViewProjMatrix().transformAffine(proj_points[i]);
		}
	}

	if (n_points > 0)
	{
		x_min = proj_points[0].x;
		x_max = proj_points[0].x;
		y_min = proj_points[0].y;
		y_max = proj_points[0].y;

		for (int i = 1; i<n_points; i++)
		{
			if (proj_points[i].x > x_max) x_max = proj_points[i].x;
			if (proj_points[i].x < x_min) x_min = proj_points[i].x;
			if (proj_points[i].y > y_max) y_max = proj_points[i].y;
			if (proj_points[i].y < y_min) y_min = proj_points[i].y;
		}

		// build the packing matrix that spreads the grid across the "projection window"
		Matrix4 pack(x_max - x_min, 0, 0, x_min,
			0, y_max - y_min, 0, y_min,
			0, 0, 1, 0,
			0, 0, 0, 1);

		Matrix4 invViewProj = render_cam_->getViewProjMatrix();
		invViewProj.inverse();

		range = invViewProj * pack;

		return true;
	}

	return false;
}

void ProjectedGrid::setDisplacementAmplitude(float ampl)
{
	upper_bound_ = Plane(normal_, pos_ + ampl * plane_.normal);
	lower_bound_ = Plane(normal_, pos_ - ampl * plane_.normal);
}

Vector4 ProjectedGrid::calculeWorldPosition(const Vector2 &uv, const Matrix4& m)
{
	const Matrix4& viewMat = render_cam_->getViewProjMatrix();

	Vector4 origin(uv.x, uv.y, -1, 1);
	Vector4 direction(uv.x, uv.y, 1, 1);

	origin = m * origin;
	direction = m * direction;

	Vector3 _org(origin.x / origin.w, origin.y / origin.w, origin.z / origin.w);
	Vector3 _dir(direction.x / direction.w, direction.y / direction.w, direction.z / direction.w);

	Ray _ray(_org, _dir);
	std::pair<bool, float> _result = _ray.intersects(plane_);
	float l = _result.second;
	Vector3 worldPos = _org + _dir*l;
	Vector4 _tempVec = viewMat*Vector4(worldPos);
	float _temp = -_tempVec.z / _tempVec.w;
	Vector4 retPos(worldPos);
	retPos /= _temp;

	return retPos;
}

void ProjectedGrid::renderGeometry(const Matrix4& m, const Vector3& worldPos)
{
	Vector4 t_corners[4];

	t_corners[0] = calculeWorldPosition(Vector2(0, 0), m);
	t_corners[1] = calculeWorldPosition(Vector2(1, 0), m);
	t_corners[2] = calculeWorldPosition(Vector2(0, 1), m);
	t_corners[3] = calculeWorldPosition(Vector2(1, 1), m);

	float du = 1.0f / (options_.ComplexityU - 1),
		dv = 1.0f / (options_.ComplexityV - 1),
		u, v = 0.0f,
		// _1_u = (1.0f-u)
		_1_u, _1_v = 1.0f,
		divide, noise;

	float x, y, z, w;

	int i = 0, iv, iu;

	Vertex * Vertices = vertices_;

	for (iv = 0; iv < options_.ComplexityV; iv++)
	{
		u = 0.0f;
		_1_u = 1.0f;

		for (iu = 0; iu < options_.ComplexityU; iu++)
		{
			x = _1_v*(_1_u*t_corners[0].x + u*t_corners[1].x) + v*(_1_u*t_corners[2].x + u*t_corners[3].x);
			z = _1_v*(_1_u*t_corners[0].z + u*t_corners[1].z) + v*(_1_u*t_corners[2].z + u*t_corners[3].z);
			w = _1_v*(_1_u*t_corners[0].w + u*t_corners[1].w) + v*(_1_u*t_corners[2].w + u*t_corners[3].w);

			divide = 1 / w;
			x *= divide;
			z *= divide;

			noise = noise_->getValue(x * 5, y * 5);

			Vertices[i].position = Vector3(x, y, z);

			i++;
			u += du;
			_1_u = 1.0f - u;
		}

		v += dv;
		_1_v = 1.0f - v;
	}

	calcNormals();
}

void ProjectedGrid::calcNormals()
{
	int v, u;
	Vector3 vec1, vec2, normal;

	Vertex * Vertices = static_cast<Vertex*>(vertices_);

	for (int i = 0; i < options_.ComplexityU * options_.ComplexityV; ++i)
	{
		Vertices[i].normal = Vector3::ZERO;
	}

	int pr = 0, r = options_.ComplexityU, nr = r + options_.ComplexityU;

	for (v = 1; v < (options_.ComplexityV - 1); v++)
	{
		for (u = 1; u < (options_.ComplexityU - 1); u++)
		{
			vec1 = Vertices[r + u + 1].position - Vertices[r + u - 1].position;
			vec2 = Vertices[nr + u].position - Vertices[pr + u].position;

			normal = vec2.crossProduct(vec1);

			Vertices[r + u].normal += normal;
		}

		r += options_.ComplexityU;
		pr = r - options_.ComplexityU;
		nr = r + options_.ComplexityU;
	}

	int last_r = (options_.ComplexityV - 1) * options_.ComplexityU;
	for (int u = 0; u < options_.ComplexityU; ++u)
	{
		Vertices[0 + u].normal.y = 1;
		Vertices[last_r + u].normal.y = 1;
	}

	r = 0;
	for (int v = 0; v < options_.ComplexityV; ++v)
	{
		Vertices[r + 0].normal.y = 1;
		Vertices[r + (options_.ComplexityU - 1)].normal.y = 1;

		r += options_.ComplexityU;
	}
}

}