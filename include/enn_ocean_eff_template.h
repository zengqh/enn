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
// Time: 2015/10/19
// File: enn_ocean_eff_template.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class WaterObject;
class OceanEffectParams : public EffectParams
{
public:
	Camera*				curr_render_cam_;
	WaterObject*		water_obj_;
};

//////////////////////////////////////////////////////////////////////////
class OceanEffectTemplate : public EffectTemplate
{

	struct WaterVSParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		float4x4 modelMat;
		float4x4 matWV;
		float4x4 modelviewProj;
		float4x4 matProj;
		float4   rippleMat[3];
		float4   eyePosWVS;
		float4   waveDir[3];
		float4   waveData[3];
		float4   rippleDir[3];
		float4   rippleTexScale[3];
		float4   rippleSpeed;
		float4   foamDir;
		float4   foamTexScale;
		float4   foamSpeed;
		float4   elapsedTime;
	};

	struct WaterPSParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		float4       specularParams;
		float4       baseColor;
		float4       miscParams;
		float4       farPlaneDist;
		float4       distortionParams;
		float4       ambientColor;
		float4       eyePosW;
		float4       rippleMagnitude;
		float4       depthGradMax;
		float4       inLightVec;
		float4	     sunColor;
		float4       reflectivity;
		float4       foamParams;
		float4       fogParams;
	};

public:
	OceanEffectTemplate(EffectTemplateManager* owner);
	~OceanEffectTemplate();

public:
	virtual EffectContext* getEffectContext(Renderable* ra);
	EffectContext* getEffectContext(EffectMethod em);
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);

protected:
	virtual void bindShaderPrograms();
	virtual void bindConstantBuffers();
	virtual void bindSamplerObjects();
	virtual void bindShaderResourceViews();
	virtual void bindUnorderedAccessViews();

protected:
	void createConstantBuffers();
	void setWaterParams();
	void applyConstantBuffers();

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	OceanEffectParams*					curr_eff_params_;

	bool								cbuffer_init_;

	WaterVSParams						water_vs_params_;
	WaterPSParams						water_ps_params_;

	D3D11ResourceProxy*					water_vs_cbuff_;
	D3D11ResourceProxy*					water_ps_cbuff_;
};

}