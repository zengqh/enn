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
// Time: 2014/06/29
// File: enn_gbuffer_effect_template.h

#pragma once

#include "enn_effect_template.h"

namespace enn
{

class GBufferEffectParams : public EffectParams
{
public:
	Camera*		curr_render_cam_;
};

//////////////////////////////////////////////////////////////////////////
class GBufferEffectTemplate : public EffectTemplate
{

	struct TransformParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4		matWV;
		Matrix4		matWVP;
	};

	struct SkeletonParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4 matBones[MAX_BONES_NUM];
	};

	struct MtrParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4			mtrShiness_;
	};

public:
	GBufferEffectTemplate(EffectTemplateManager* owner);
	~GBufferEffectTemplate();

public:
	virtual EffectContext* getEffectContext(Renderable* ra);
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
	GBufferEffectParams*				curr_eff_params_;

	bool								cbuffer_init_;

	TransformParams						transform_params_;
	SkeletonParams						skel_params_;
	MtrParams							mtr_params_;

	D3D11ResourceProxy*					transform_cbuff_;
	D3D11ResourceProxy*					skel_cbuff_;
	D3D11ResourceProxy*					mtr_cbuff_;

	TextureObjUnit*						normal_fit_tex_;
};

}