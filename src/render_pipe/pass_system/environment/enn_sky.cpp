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
// File: enn_sky.cpp
//

#include "enn_sky.h"
#include "enn_sky_effect_template.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{

Sky::Sky()
{
	init();
}

Sky::~Sky()
{

}

void Sky::render()
{
	beginRender();

	renderSky();

	endRender();
}

void Sky::renderReflection()
{

}

void Sky::beginRender()
{
	g_render_device_->setDepthStencilState(g_render_device_->default_deptph_state_, 1);
	g_render_device_->setRasterizerState(g_render_device_->cull_none_state_);
}

void Sky::endRender()
{
	g_render_device_->setSolidState(true);
}

void Sky::init()
{
	sky_mesh_ = g_mesh_mgr_->createMesh("sky_mesh__");
	SubMesh* sub_mesh = sky_mesh_->createNewSubMesh(0);

	int iRings = 50, iSegments = 50;
	float x, y, z, r;
	float fTileRingAngle = Math::PI / iRings;
	float fTileSegAngle = Math::TWO_PI / iSegments;
	int iVertexCount = (iRings + 1) * (iSegments + 1);
	int iIndexCount = iRings * iSegments * 6;

	vector<Vector3>::type vb;
	vector<uint16>::type ib;

	// vb
	for (int i = 0; i <= iRings; ++i)
	{
		r = Math::sin(i * fTileRingAngle);
		y = Math::cos(i * fTileRingAngle);

		for (int j = 0; j <= iSegments; ++j)
		{
			x = r * Math::cos(j * fTileSegAngle);
			z = r * Math::sin(j * fTileSegAngle);

			vb.push_back(Vector3(x, y, z));
		}
	}

	// ib
	uint16 row = 0, row_n = 0;
	ib.resize(iIndexCount);


	uint16* ib_buff = &ib[0];

	for (int i = 0; i < iRings; ++i)
	{
		row_n = row + iSegments + 1;

		for (int j = 0; j < iSegments; ++j)
		{
			*ib_buff++ = row + j;
			*ib_buff++ = row + j + 1;
			*ib_buff++ = row_n + j;

			*ib_buff++ = row_n + j;
			*ib_buff++ = row + j + 1;
			*ib_buff++ = row_n + j + 1;
		}

		row += iSegments + 1;
	}
	
	sub_mesh->setPrimType(TT_TRIANGLE_LIST);
	sub_mesh->setVertexType(MASK_P3);
	sub_mesh->setVertexCount(iVertexCount);

	sub_mesh->setIdxType(IDX_16);
	sub_mesh->setIndexCount(iIndexCount);

	sub_mesh->setVertexData(&vb[0]);
	sub_mesh->setIndexData(&ib[0]);

	sky_mesh_->updateAABB(true);
	sub_mesh->makeGPUBuffer();
}

void Sky::update()
{

}

void Sky::renderSky()
{
	SkyEffectTemplate* eff_template = static_cast<SkyEffectTemplate*>(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_SKY));
	SubMesh* ra = sky_mesh_->getSubMesh(0);

	EffectMethod em;
	EffectContext* eff_ctx = eff_template->getEffectContext(em);

	SkyEffectParams params;
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.sky_ = this;

	eff_template->doSetParams(eff_ctx, 0, &params);
	ra->render(eff_ctx);

	//Vector3 color = getColorAt(Vector3(0, 1, 0));
}

Vector3 Sky::getColorAt(const Vector3& Direction) const
{
	double Scale = 1.0f / (options_.outer_radius - options_.inner_radius),
			   ScaleDepth = (options_.outer_radius - options_.inner_radius) / 2.0f,
		       ScaleOverScaleDepth = Scale / ScaleDepth,
			   Kr4PI  = options_.rayleigh_multiplier * 4.0f * Math::PI,
			   KrESun = options_.rayleigh_multiplier * options_.sun_intensity,
			   Km4PI  = options_.mie_multiplier * 4.0f * Math::PI,
			   KmESun = options_.mie_multiplier * options_.sun_intensity;

		// --- Start vertex program simulation ---
	//Vector3 uLightDir = Vector3(0.96592581f, 0.18301269f, 0.18301269f);
	Vector3 uLightDir = Vector3(0, 1, -1).normalisedCopy();
	Vector3		v3Pos = Direction.normalisedCopy();
	Vector3		uCameraPos = Vector3(0, options_.inner_radius + (options_.outer_radius - options_.inner_radius)*options_.height_position, 0);
		Vector3		uInvWaveLength = Vector3(
			                    1.0f / Math::pow(options_.wave_length.x, 4.0f),
								1.0f / Math::pow(options_.wave_length.y, 4.0f),
								1.0f / Math::pow(options_.wave_length.z, 4.0f));

		// Get the ray from the camera to the vertex, and it's length (far point)
		v3Pos.y += options_.inner_radius;
		Vector3 v3Ray = v3Pos - uCameraPos;
		double fFar = v3Ray.length();
		v3Ray /= fFar;

		// Calculate the ray's starting position, then calculate its scattering offset
		Vector3 v3Start = uCameraPos;
		double fHeight = uCameraPos.y,
		       fStartAngle = v3Ray.dotProduct(v3Start) / fHeight,
		       fDepth = ::exp(ScaleOverScaleDepth * (options_.inner_radius - uCameraPos.y)),
		       fStartOffset = fDepth * _scale(fStartAngle, ScaleDepth);

		// Init loop variables
		double fSampleLength = fFar /(double)4,
		       fScaledLength = fSampleLength * Scale,
			   fHeight_, fDepth_, fLightAngle, fCameraAngle, fScatter;
		Vector3 v3SampleRay = v3Ray * fSampleLength,
		              v3SamplePoint = v3Start + v3SampleRay * 0.5f,
					  color = Vector3(0,0,0), v3Attenuate;

        // Loop the ray
		for (int i = 0; i < 4; i++)
		{
			fHeight_ = v3SamplePoint.length();
			fDepth_ = ::exp(ScaleOverScaleDepth * (options_.inner_radius-fHeight_));

			fLightAngle = uLightDir.dotProduct(v3SamplePoint) / fHeight_;
			fCameraAngle = v3Ray.dotProduct(v3SamplePoint) / fHeight_;

			fScatter = (fStartOffset + fDepth*(_scale(fLightAngle, ScaleDepth) - _scale(fCameraAngle, ScaleDepth)));

			v3Attenuate = Vector3(
				::exp(-fScatter * (uInvWaveLength.x * Kr4PI + Km4PI)),
				::exp(-fScatter * (uInvWaveLength.y * Kr4PI + Km4PI)),
				::exp(-fScatter * (uInvWaveLength.z * Kr4PI + Km4PI)));

			// Accumulate color
			v3Attenuate *= (fDepth_ * fScaledLength);
			color += v3Attenuate;

			// Next sample point
			v3SamplePoint += v3SampleRay;
		}

		// Outputs
		Vector3 oRayleighColor = color * (uInvWaveLength * KrESun),
		              oMieColor      = color * KmESun,
		              oDirection     = uCameraPos - v3Pos;

		// --- End vertex program simulation ---
		// --- Start fragment program simulation ---

		double cos = uLightDir.dotProduct(oDirection) / oDirection.length(),
		       cos2 = cos*cos,
		       rayleighPhase = 0.75 * (1.0 + 0.5*cos2),
			   g2 = options_.g*options_.g,
		       miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * 
			            (1.0f + cos2) / Math::pow(1.0f + g2 - 2.0f * options_.g * cos, 1.5f);

		Vector3 oColor;

		oColor = Vector3(
			1 - ::exp(-options_.exposure * (rayleighPhase * oRayleighColor.x + miePhase * oMieColor.x)),
			1 - ::exp(-options_.exposure * (rayleighPhase * oRayleighColor.y + miePhase * oMieColor.y)),
			1 - ::exp(-options_.exposure * (rayleighPhase * oRayleighColor.z + miePhase * oMieColor.z)));

		// For night rendering
		oColor += Math::clamp<float>(((1 - std::max(oColor.x, std::max(oColor.y, oColor.z)) * 10)), 0, 1)
			* (Vector3(0.05, 0.05, 0.1)
			* (2 - 0.75f*Math::clamp<float>(-uLightDir.y, 0, 1)) * Math::pow(1 - Direction.y, 3));

		// --- End fragment program simulation ---

		// Output color
		return oColor;
}

const float Sky::_scale(const float& cos, const float& uScaleDepth) const
{
	float x = 1 - cos;
	return uScaleDepth * ::exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

}