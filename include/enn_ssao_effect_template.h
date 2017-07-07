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
// Time: 2014/10/15
// File: enn_ssao_effect_template.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class SSAOEffectParams : public EffectParams
{
public:
	Camera*							curr_render_cam_;
	D3D11ResourceProxy*				normal_tex_;
	D3D11ResourceProxy*				depth_tex_;
};

//////////////////////////////////////////////////////////////////////////
class SSAOEffectTemplate : public EffectTemplate
{
	struct TransformParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4		invMatProj;
		Matrix4		matProj;
		Matrix4		invMatView;
	};

	struct SSAOParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4	paramA;
		Vector4	paramB;
	};

public:
	SSAOEffectTemplate(EffectTemplateManager* owner);
	~SSAOEffectTemplate();

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
	void applyConstantBuffers();

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	SSAOEffectParams*					curr_eff_params_;

	bool								cbuffer_init_;

	TransformParams						transform_params_;
	SSAOParams							ssao_params_;

	D3D11ResourceProxy*					transform_cbuff_;
	D3D11ResourceProxy*					ssao_params_cbuff_;
};

}