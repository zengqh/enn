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
// Time: 2014/07/18
// File: enn_light_buffer_effect_template.h

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class LightBufferEffectParams : public EffectParams
{
public:
	Camera*							curr_render_cam_;
	D3D11ResourceProxy*				normal_tex_;
	D3D11ShaderResourceView*		depth_tex_;
	Light*							light_;
};

//////////////////////////////////////////////////////////////////////////
class LightBufferEffectTemplate : public EffectTemplate
{
	struct TransformParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4		proj_matrix;
		Matrix4		matWVP;
		Matrix4		matWV;
	};

	struct PointLitParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 position;
		Vector4 diffuse;
	};

	struct SpotLitParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 position;
		Vector4 direction;
		Vector4 diffuse;
	};

public:
	LightBufferEffectTemplate(EffectTemplateManager* owner);
	~LightBufferEffectTemplate();

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
	void setPointLitParams();
	void setSpotLitParams();
	void applyConstantBuffers();

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	LightBufferEffectParams*			curr_eff_params_;

	bool								cbuffer_init_;

	TransformParams						transform_params_;
	PointLitParams						point_params_;
	SpotLitParams						spot_params_;

	D3D11ResourceProxy*					transform_cbuff_;
	D3D11ResourceProxy*					point_cbuff_;
	D3D11ResourceProxy*					spot_cbuff_;
};

}