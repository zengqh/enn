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
// File: enn_ssao_effect_template.cpp
//

#include "enn_ssao_filter_effect_template.h"
#include "enn_root.h"

namespace enn
{


SSAOFilterEffectTemplate::SSAOFilterEffectTemplate(EffectTemplateManager* owner)
	: EffectTemplate(owner)
	, cbuffer_init_(false)
	, filter_cbuff_(0)
	, phe_params_cbuff_(0)
	, transform_cbuff_(0)
	, is_offset_filter_calc_(false)
{

}

SSAOFilterEffectTemplate::~SSAOFilterEffectTemplate()
{

}

EffectContext* SSAOFilterEffectTemplate::getEffectContext(Renderable* ra)
{
	EffectMethod em = getMethod(ra);

	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "ssao_filter.fx", fx_code);
	shader_getFileData(base_path + "common.fxh", comm_code);

	String fx_macro_code = makeMacro(em) + comm_code + fx_code;

	D3D11Shader* vs = g_render_device_->loadShader(VERTEX_SHADER, fx_macro_code, "VSMAIN", "vs_5_0", 0);
	D3D11Shader* ps = g_render_device_->loadShader(PIXEL_SHADER, fx_macro_code, "PSMAIN", "ps_5_0", 0);

	ENN_ASSERT(vs && ps);

	EffectContext*& eff_ctx = (eff_ctx_map_[em]);

	ENN_ASSERT(!eff_ctx);
	eff_ctx = ENN_NEW EffectContext();

	eff_ctx->eff_method_ = em;

	D3D11Effect* eff = ENN_NEW D3D11Effect();
	eff->attachShader(VERTEX_SHADER, vs);
	eff->attachShader(PIXEL_SHADER, ps);

	eff_ctx->eff_ = eff;
	eff_ctx->owner_ = this;

	return eff_ctx;
}

EffectContext* SSAOFilterEffectTemplate::getEffectContext(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "ssao_filter.fx", fx_code);
	shader_getFileData(base_path + "common.fxh", comm_code);

	String fx_macro_code = makeMacro(em) + comm_code + fx_code;

	D3D11Shader* vs = g_render_device_->loadShader(VERTEX_SHADER, fx_macro_code, "VSMAIN", "vs_5_0", 0);
	D3D11Shader* ps = g_render_device_->loadShader(PIXEL_SHADER, fx_macro_code, "PSMAIN", "ps_5_0", 0);

	ENN_ASSERT(vs && ps);

	EffectContext*& eff_ctx = (eff_ctx_map_[em]);

	ENN_ASSERT(!eff_ctx);
	eff_ctx = ENN_NEW EffectContext();

	eff_ctx->eff_method_ = em;

	D3D11Effect* eff = ENN_NEW D3D11Effect();
	eff->attachShader(VERTEX_SHADER, vs);
	eff->attachShader(PIXEL_SHADER, ps);

	eff_ctx->eff_ = eff;
	eff_ctx->owner_ = this;

	return eff_ctx;
}


void SSAOFilterEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<SSAOFilterEffectParams*>(params);

	applyPipelineResources();
}

void SSAOFilterEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void SSAOFilterEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();

	applyConstantBuffers();
}

void SSAOFilterEffectTemplate::bindSamplerObjects()
{
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, 0, g_render_device_->linear_wrap_state_);
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, 1, g_render_device_->point_wrap_state_);
}

void SSAOFilterEffectTemplate::bindShaderResourceViews()
{
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, curr_eff_params_->ao_tex_);
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, curr_eff_params_->depth_tex_);
}

void SSAOFilterEffectTemplate::bindUnorderedAccessViews()
{

}

void SSAOFilterEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	filter_cbuff_ = g_render_device_->createConstantBuffer(0, filter_params_.size());
	phe_params_cbuff_ = g_render_device_->createConstantBuffer(0, phe_params_.size());
	transform_cbuff_ = g_render_device_->createConstantBuffer(0, transform_params_.size());

	cbuffer_init_ = true;
}

void SSAOFilterEffectTemplate::setTransformParams()
{
	if (!is_offset_filter_calc_)
	{
		calcFilterOffsetParams(true, filter_params_);
		calcFilterOffsetParams(false, v_filter_params_);
	}

	phe_params_.phe = 10;

	Matrix4 device_proj_matrix;
	g_render_device_->convertProjectionMatrix(curr_eff_params_->curr_render_cam_->getProjMatrix(), device_proj_matrix);

	transform_params_.matProj = device_proj_matrix;
}


void SSAOFilterEffectTemplate::applyConstantBuffers()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	vector<D3D11ConstanceBuffer*>::type cbuffers1;

	if (curr_eff_params_->is_horz)
	{
		uploadConstanceBuffer(&filter_params_, filter_cbuff_);
		cbuffers1.push_back(filter_cbuff_->getCBuffer());
	}
	else
	{
		uploadConstanceBuffer(&v_filter_params_, filter_cbuff_);
		cbuffers1.push_back(filter_cbuff_->getCBuffer());
	}
	

	uploadConstanceBuffer(&phe_params_, phe_params_cbuff_);
	cbuffers1.push_back(phe_params_cbuff_->getCBuffer());

	uploadConstanceBuffer(&transform_params_, transform_cbuff_);
	cbuffers1.push_back(transform_cbuff_->getCBuffer());

	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

void SSAOFilterEffectTemplate::calcFilterOffsetParams(bool is_horz, filterSampleOffsets& filter_params)
{
	int radius = 4;
	float sigma = 3.0f;
	float sigmaScale = -0.5f / (sigma * sigma);

	for (int i = 0; i < 9; ++i)
	{
		int offset = i - radius;

		Vector2& vOff = *(Vector2*)(&(filter_params.filterOffset[i].x));
		vOff = is_horz ? Vector2(offset, 0) : Vector2(0, offset);

		float geometry_weight = ::exp(offset * offset * sigmaScale);

		filter_params.filterOffset[i].z = geometry_weight;
		filter_params.filterOffset[i].w = 0;
	}
}

}