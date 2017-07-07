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
// Time: 2014/05/19
// File: enn_forward_effect_template.cpp
//

#include "enn_forward_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
ForwardEffectTemplate::ForwardEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, cbuffer_init_(false)
, transform_cbuff_(0)
, skel_cbuff_(0)
, mtr_cbuff_(0)
, dir_lit_cbuff_(0)
, geometry_cbuff_(0)
, curr_eff_context_(0)
, curr_ra_(0)
, curr_eff_params_(0)
{

}

ForwardEffectTemplate::~ForwardEffectTemplate()
{

}

void ForwardEffectTemplate::doSetParams(EffectContext* eff_ctx,
	Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<ForwardEffectParams*>(params);

	applyPipelineResources();
}

EffectContext* ForwardEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "general.fx", fx_code);
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

void ForwardEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	transform_cbuff_ = g_render_device_->createConstantBuffer(0, transform_params_.size());
	skel_cbuff_ = g_render_device_->createConstantBuffer(0, skel_params_.size());
	mtr_cbuff_ = g_render_device_->createConstantBuffer(0, mtr_params_.size());
	dir_lit_cbuff_ = g_render_device_->createConstantBuffer(0, dir_lit_params_.size());
	geometry_cbuff_ = g_render_device_->createConstantBuffer(0, geometry_params_.size());

	cbuffer_init_ = true;
}

void ForwardEffectTemplate::setTransformParams()
{
	Camera* cam = curr_eff_params_->render_cam_;

	const Matrix4& world_matrix = curr_ra_->getWorldMatrix();
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	Matrix4 device_proj_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);

	Matrix4 wvp = device_proj_matrix * view_matrix * world_matrix;

	transform_params_.matWorld = world_matrix;
	transform_params_.matWVP = wvp;

	geometry_params_.camPos = Vector4(cam->getEyePos(), 0);
	geometry_params_.global_amb = Vector4::ZERO;
}

void ForwardEffectTemplate::setMtrParams()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	if (em.hasColorMap())
	{
		Vector4 tex_clr = mtr->getTexClr()->getAsVector4();
		mtr_params_.tex_clr = (tex_clr);
	}

	if (em.hasLighting())
	{
		Vector4 diffuse = mtr->getDiffuse().getAsVector4();
		mtr_params_.mtr_diffuse = (diffuse);

		Vector4 spec_shiness = mtr->getSpecularShiness().getAsVector4();
		mtr_params_.mtr_spec = (spec_shiness);
	}

	if (em.hasEmissive())
	{
		Vector4 emissive = mtr->getEmissive().getAsVector4();
		mtr_params_.mtr_emissive = (emissive);
	}

	Vector4 fog_param_alpha1_alpha2;

	fog_param_alpha1_alpha2.z = mtr->getAlpha();

	if (mtr->hasAlphaTest())
	{
		fog_param_alpha1_alpha2.w = mtr->getAlphaTest();
	}

	if (em.hasFog())
	{
		
	}

	mtr_params_.fog_param_mtr_alpha_alpha_test_ref = (fog_param_alpha1_alpha2);
}

void ForwardEffectTemplate::setSkeletonParams()
{

}

void ForwardEffectTemplate::setLightInfoParams()
{

}

void ForwardEffectTemplate::applyConstantBuffers()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	uploadConstanceBuffer(&transform_params_, transform_cbuff_);
	
	if (em.hasSkeleton())
	{
		uploadConstanceBuffer(&skel_params_, skel_cbuff_);
	}

	uploadConstanceBuffer(&mtr_params_, mtr_cbuff_);

	if (em.hasLighting())
	{
		if (em.hasDirLit())
		{
			uploadConstanceBuffer(&dir_lit_params_, dir_lit_cbuff_);
		}
	}

	uploadConstanceBuffer(&geometry_params_, geometry_cbuff_);
}

void ForwardEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void ForwardEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();
	setMtrParams();
	setSkeletonParams();
	setLightInfoParams();

	applyConstantBuffers();
}

void ForwardEffectTemplate::bindSamplerObjects()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	// texture diffuse sampler
	if (em.hasDiffuseMap())
	{
		Texture* tex = mtr->useTexDiff()->getTexture();
		setTextureSampler(0, tex);
	}

	if (em.hasDiffuse2Map())
	{
		Texture* tex = mtr->useTexDiff2()->getTexture();
		setTextureSampler(1, tex);
	}

	if (em.hasNormalMap())
	{
		Texture* tex = mtr->useTexNormal()->getTexture();
		setTextureSampler(2, tex);
	}

	if (em.hasSpecularMap())
	{
		Texture* tex = mtr->useTexSpecular()->getTexture();
		setTextureSampler(3, tex);
	}

	if (em.hasEmissiveMap())
	{
		Texture* tex = mtr->useTexEmissive()->getTexture();
		setTextureSampler(4, tex);
	}
}

void ForwardEffectTemplate::bindShaderResourceViews()
{
	EffectMethod& em = curr_eff_context_->eff_method_;
	Material* mtr = curr_ra_->getMaterial();

	// texture diffuse sampler
	if (em.hasDiffuseMap())
	{
		Texture* tex = mtr->useTexDiff()->getTexture();
		setTextureShaderResourceView(0, tex);
	}

	if (em.hasDiffuse2Map())
	{
		Texture* tex = mtr->useTexDiff2()->getTexture();
		setTextureShaderResourceView(1, tex);
	}

	if (em.hasNormalMap())
	{
		Texture* tex = mtr->useTexNormal()->getTexture();
		setTextureShaderResourceView(2, tex);
	}

	if (em.hasSpecularMap())
	{
		Texture* tex = mtr->useTexSpecular()->getTexture();
		setTextureShaderResourceView(3, tex);
	}

	if (em.hasEmissiveMap())
	{
		Texture* tex = mtr->useTexEmissive()->getTexture();
		setTextureShaderResourceView(4, tex);
	}
}

void ForwardEffectTemplate::bindUnorderedAccessViews()
{

}

}