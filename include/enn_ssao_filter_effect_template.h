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
// File: enn_ssao_filter_effect_template.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class SSAOFilterEffectParams : public EffectParams
{
public:
	Camera*							curr_render_cam_;
	D3D11ResourceProxy*				ao_tex_;
	D3D11ResourceProxy*				depth_tex_;
	bool							is_horz;
};

//////////////////////////////////////////////////////////////////////////
class SSAOFilterEffectTemplate : public EffectTemplate
{
	struct filterSampleOffsets
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 filterOffset[9];
	};

	struct photometricExponent
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 phe;
	};

	struct Transforms
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4 matProj;
	};

public:
	SSAOFilterEffectTemplate(EffectTemplateManager* owner);
	~SSAOFilterEffectTemplate();

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

	void calcFilterOffsetParams(bool is_horz, filterSampleOffsets& filter_params);

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	SSAOFilterEffectParams*				curr_eff_params_;

	bool								cbuffer_init_;
	bool								is_offset_filter_calc_;

	filterSampleOffsets					filter_params_;
	filterSampleOffsets					v_filter_params_;
	photometricExponent					phe_params_;
	Transforms							transform_params_;

	D3D11ResourceProxy*					filter_cbuff_;
	D3D11ResourceProxy*					phe_params_cbuff_;
	D3D11ResourceProxy*					transform_cbuff_;
};

}