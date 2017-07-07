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
// Time: 2014/03/14
// File: enn_main_result_effect_template.h
//

#pragma once

#include "enn_effect_template.h"
#include "enn_node_factory.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class MainResultEffectParams : public EffectParams
{
public:
	Camera*						render_cam_;
	D3D11ResourceProxy*			light_buffer_;
};

//////////////////////////////////////////////////////////////////////////
class MainResultEffectTemplate : public EffectTemplate
{
	struct TransformParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4		matWorld;
		Matrix4		matWVP;
		Matrix4		matUV;
	};

	struct SkeletonParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4 matBones[MAX_BONES_NUM];
	};

	struct MtrParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4		mtr_diffuse;
		Vector4		mtr_spec;
		Vector4		mtr_emissive;
		Vector4		tex_clr;
		Vector4		fog_param_mtr_alpha_alpha_test_ref;
		Vector4		fog_color;
	};

	struct DirLightParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4		dir_light_dir;
		Vector4		dir_light_diff;
	};

	struct GeometryParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4		global_amb;
		Vector4		camPos;
	};


public:
	MainResultEffectTemplate(EffectTemplateManager* owner);
	~MainResultEffectTemplate();

public:
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);
	virtual EffectContext* getEffectContext(Renderable* ra);

protected:
	void createConstantBuffers();

protected:
	void setTransformParams();
	void setMtrParams();
	void setSkeletonParams();
	void setLightInfoParams();
	void applyConstantBuffers();

protected:
	virtual void bindShaderPrograms();
	virtual void bindConstantBuffers();
	virtual void bindSamplerObjects();
	virtual void bindShaderResourceViews();
	virtual void bindUnorderedAccessViews();

protected:
	virtual String makeMacro(const EffectMethod& em);

protected:
	TransformParams						transform_params_;
	SkeletonParams						skel_params_;
	MtrParams							mtr_params_;
	DirLightParams						dir_lit_params_;
	GeometryParams						geometry_params_;

	D3D11ResourceProxy*					transform_cbuff_;
	D3D11ResourceProxy*					skel_cbuff_;
	D3D11ResourceProxy*					mtr_cbuff_;
	D3D11ResourceProxy*					dir_lit_cbuff_;
	D3D11ResourceProxy*					geometry_cbuff_;

	bool								cbuffer_init_;

	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	MainResultEffectParams*				curr_eff_params_;
};

}