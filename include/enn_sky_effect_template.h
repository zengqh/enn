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
// Time: 2014/12/21
// File: enn_sky_effect_template.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class Sky;
class SkyEffectParams : public EffectParams
{
public:
	Camera*		curr_render_cam_;
	Sky*		sky_;
};

//////////////////////////////////////////////////////////////////////////
class SkyEffectTemplate : public EffectTemplate
{
	struct SkyParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4 		matWVP;
		Vector4 		uLightDir;
		Vector4			uCameraPos;
		Vector4			invWaveLength;
		Vector4			kr_params;
		Vector4			atmosphere_params;
	};


	struct PsParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 ps_params;
	};

public:
	SkyEffectTemplate(EffectTemplateManager* owner);
	~SkyEffectTemplate();

public:
	virtual EffectContext* getEffectContext(Renderable* ra);
	virtual EffectContext* getEffectContext(EffectMethod em);
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);

protected:
	virtual void bindShaderPrograms();
	virtual void bindConstantBuffers();
	virtual void bindSamplerObjects();
	virtual void bindShaderResourceViews();
	virtual void bindUnorderedAccessViews();

protected:
	void createConstantBuffers();
	void setTransformParams();
	void setMtrParams();
	void setSkeletonParams();
	void applyConstantBuffers();

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	SkyEffectParams*					curr_eff_params_;

	bool								cbuffer_init_;

	SkyParams							sky_params_;
	PsParams							ps_params_;

	D3D11ResourceProxy*					sky_cbuff_;
	D3D11ResourceProxy*					ps_cbuff_;
};

}