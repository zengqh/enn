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
// File: enn_gbuffer_effect_template.cpp

#include "enn_gbuffer_effect_template.h"
#include "enn_texture_manager.h"

namespace enn
{

GBufferEffectTemplate::GBufferEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, cbuffer_init_(false)
, transform_cbuff_(0)
, skel_cbuff_(0)
, mtr_cbuff_(0)
, normal_fit_tex_(0)
{

}

GBufferEffectTemplate::~GBufferEffectTemplate()
{

}

EffectContext* GBufferEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "gbuffer.fx", fx_code);
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

void GBufferEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<GBufferEffectParams*>(params);

	applyPipelineResources();
}


void GBufferEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void GBufferEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();
	setMtrParams();
	setSkeletonParams();

	applyConstantBuffers();
}

void GBufferEffectTemplate::bindSamplerObjects()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	Texture* tex = g_tex_manager_->getNormalFitTex();
	setTextureSampler(0, tex);

	if (em.hasNormalMap())
	{
		Texture* tex_normal = mtr->useTexNormal()->getTexture();
		setTextureSampler(1, tex_normal);
	}
}

void GBufferEffectTemplate::bindShaderResourceViews()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	Texture* tex = g_tex_manager_->getNormalFitTex();
	setTextureShaderResourceView(0, tex);

	if (em.hasNormalMap())
	{
		Texture* tex_normal = mtr->useTexNormal()->getTexture();
		setTextureShaderResourceView(1, tex_normal);
	}
}

void GBufferEffectTemplate::bindUnorderedAccessViews()
{

}

void GBufferEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	transform_cbuff_ = g_render_device_->createConstantBuffer(0, transform_params_.size());
	skel_cbuff_ = g_render_device_->createConstantBuffer(0, skel_params_.size());
	mtr_cbuff_ = g_render_device_->createConstantBuffer(0, mtr_params_.size());

	cbuffer_init_ = true;
}

void GBufferEffectTemplate::setTransformParams()
{
	Camera* cam = curr_eff_params_->curr_render_cam_;

	const Matrix4& world_matrix = curr_ra_->getWorldMatrix();
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 wvp = (device_proj_matrix * device_view_matrix) * world_matrix;
	Matrix4 wv = device_view_matrix * world_matrix;

	transform_params_.matWV = wv;
	transform_params_.matWVP = wvp;
}

void GBufferEffectTemplate::setMtrParams()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	if (em.hasLighting())
	{
		Vector4 spec_shiness = mtr->getSpecularShiness().getAsVector4();

		spec_shiness.w /= 255.0f;

		mtr_params_.mtrShiness_ = spec_shiness;
	}
}

void GBufferEffectTemplate::setSkeletonParams()
{

}

void GBufferEffectTemplate::applyConstantBuffers()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	vector<D3D11ConstanceBuffer*>::type cbuffers0, cbuffers1;

	uploadConstanceBuffer(&transform_params_, transform_cbuff_);
	cbuffers0.push_back(transform_cbuff_->getCBuffer());

	if (em.hasSkeleton())
	{
		uploadConstanceBuffer(&skel_params_, skel_cbuff_);
		cbuffers0.push_back(skel_cbuff_->getCBuffer());
	}

	uploadConstanceBuffer(&mtr_params_, mtr_cbuff_);
	cbuffers1.push_back(mtr_cbuff_->getCBuffer());

	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

}