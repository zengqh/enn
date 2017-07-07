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

#include "enn_ssao_effect_template.h"
#include "enn_root.h"

namespace enn
{


SSAOEffectTemplate::SSAOEffectTemplate(EffectTemplateManager* owner)
	: EffectTemplate(owner)
	, cbuffer_init_(false)
	, transform_cbuff_(0)
	, ssao_params_cbuff_(0)
{

}

SSAOEffectTemplate::~SSAOEffectTemplate()
{

}

EffectContext* SSAOEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "ssao.fx", fx_code);
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

EffectContext* SSAOEffectTemplate::getEffectContext(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "ssao.fx", fx_code);
	shader_getFileData(base_path + "common.fxh", comm_code);

	String fx_macro_code = makeMacro(em) + comm_code + fx_code;

	D3D11Shader* vs = g_render_device_->loadShader(VERTEX_SHADER, fx_macro_code, "SSAO_VS", "vs_5_0", 0);
	D3D11Shader* ps = g_render_device_->loadShader(PIXEL_SHADER, fx_macro_code, "SSAO_PS", "ps_5_0", 0);

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


void SSAOEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<SSAOEffectParams*>(params);

	applyPipelineResources();
}

void SSAOEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void SSAOEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();

	applyConstantBuffers();
}

void SSAOEffectTemplate::bindSamplerObjects()
{
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, 0, g_render_device_->linear_wrap_state_);
}

void SSAOEffectTemplate::bindShaderResourceViews()
{
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, curr_eff_params_->normal_tex_);
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, curr_eff_params_->depth_tex_);

	Texture* tex = g_tex_manager_->getRandomTexture();
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 2, tex->getTextureObj()->getResourceProxy());
}

void SSAOEffectTemplate::bindUnorderedAccessViews()
{

}

void SSAOEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	transform_cbuff_ = g_render_device_->createConstantBuffer(0, transform_params_.size());
	ssao_params_cbuff_ = g_render_device_->createConstantBuffer(0, ssao_params_.size());

	cbuffer_init_ = true;
}

void SSAOEffectTemplate::setTransformParams()
{
	const Matrix4& proj_matrix = curr_eff_params_->curr_render_cam_->getProjMatrix();
	const Matrix4& view_matrix = curr_eff_params_->curr_render_cam_->getViewMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 pj = device_proj_matrix;
	Matrix4 iv_pj = device_proj_matrix.inverse();

	transform_params_.invMatProj = iv_pj;
	transform_params_.matProj = pj;
	transform_params_.invMatView = device_view_matrix.inverse();

	// ssao params
	// disk radius
	ssao_params_.paramA.x = 1.5f;

	// disk radius sqr
	ssao_params_.paramA.y = enn::Math::sqr(ssao_params_.paramA.x);

	// intensity
	ssao_params_.paramA.z = 1.0f;

	// power
	ssao_params_.paramA.w = 1.0f;
	
	// cEpsilon
	ssao_params_.paramB.x = 0.0001f;

	// cSelfOccBeta
	ssao_params_.paramB.y = 0.002f;
}


void SSAOEffectTemplate::applyConstantBuffers()
{
	EffectMethod& em = curr_eff_context_->eff_method_;

	vector<D3D11ConstanceBuffer*>::type cbuffers0, cbuffers1;

	uploadConstanceBuffer(&transform_params_, transform_cbuff_);
	cbuffers0.push_back(transform_cbuff_->getCBuffer());
	cbuffers1.push_back(transform_cbuff_->getCBuffer());

	uploadConstanceBuffer(&ssao_params_, ssao_params_cbuff_);
	cbuffers1.push_back(ssao_params_cbuff_->getCBuffer());

	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

}