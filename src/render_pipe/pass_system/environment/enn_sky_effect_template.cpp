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
// File: enn_sky_effect_template.cpp
//

#include "enn_sky_effect_template.h"
#include "enn_sky.h"

namespace enn
{

SkyEffectTemplate::SkyEffectTemplate(EffectTemplateManager* owner)
	: EffectTemplate(owner)
	, cbuffer_init_(false)
	, sky_cbuff_(0)
	, ps_cbuff_(0)
{

}

SkyEffectTemplate::~SkyEffectTemplate()
{

}

EffectContext* SkyEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "sky.fx", fx_code);
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

EffectContext* SkyEffectTemplate::getEffectContext(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "sky.fx", fx_code);
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

void SkyEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<SkyEffectParams*>(params);

	applyPipelineResources();
}


void SkyEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void SkyEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();
	setMtrParams();
	setSkeletonParams();

	applyConstantBuffers();
}

void SkyEffectTemplate::bindSamplerObjects()
{
	
}

void SkyEffectTemplate::bindShaderResourceViews()
{
	
}

void SkyEffectTemplate::bindUnorderedAccessViews()
{

}

void SkyEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	sky_cbuff_ = g_render_device_->createConstantBuffer(0, sky_params_.size());
	ps_cbuff_ = g_render_device_->createConstantBuffer(0, ps_params_.size());

	cbuffer_init_ = true;
}

void SkyEffectTemplate::setTransformParams()
{
	Camera* cam = curr_eff_params_->curr_render_cam_;

	Matrix4 world_matrix;
	float farClip = cam->getZFar() * 0.9f;
	world_matrix.setTrans(cam->getEyePos());
	world_matrix.setScale(Vector3(farClip));

	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 wvp = (device_proj_matrix * device_view_matrix) * world_matrix;
	Matrix4 wv = device_view_matrix * world_matrix;

	const Sky::Options& option = curr_eff_params_->sky_->getOption();

	sky_params_.matWVP = wvp;
	sky_params_.uLightDir = Vector4(Vector3(0, 1, -16).normalisedCopy(), 0);
	sky_params_.uCameraPos = Vector4(0, option.inner_radius + (option.outer_radius - option.inner_radius) * option.height_position, 0, 0);

	Vector3 invWaveLength = Vector3(1 / Math::pow(option.wave_length.x, 4.0f),
		1 / Math::pow(option.wave_length.y, 4.0f),
		1 / Math::pow(option.wave_length.z, 4.0f));

	sky_params_.invWaveLength = Vector4(invWaveLength, option.inner_radius);


	float Kr4PI = option.rayleigh_multiplier * 4.0f * Math::PI;
	float KrESun = option.rayleigh_multiplier * option.sun_intensity;

	float Km4PI = option.mie_multiplier * 4.0f * Math::PI;
	float KmESun = option.mie_multiplier * option.sun_intensity;

	// uKrESun
	sky_params_.kr_params.x = KrESun;
	// uKmESun
	sky_params_.kr_params.y = KmESun;
	// uKr4PI
	sky_params_.kr_params.z = Kr4PI;
	// uKm4PI
	sky_params_.kr_params.w = Km4PI;


	float Scale = 1.0f / (option.outer_radius - option.inner_radius);
	float ScaleDepth = (option.outer_radius - option.inner_radius) / 2.0f;
	float ScaleOverScaleDepth = Scale / ScaleDepth;
	// uScale
	sky_params_.atmosphere_params.x = Scale;
	sky_params_.atmosphere_params.y = ScaleDepth;
	sky_params_.atmosphere_params.z = ScaleOverScaleDepth;
	sky_params_.atmosphere_params.w = 0;


	/// ps params
	// uG
	float uG = option.g;
	float uG2 = option.g * option.g;
	float uExposure = 
	ps_params_.ps_params.x = uG;
	ps_params_.ps_params.y = uG2;
	ps_params_.ps_params.z = option.exposure;
}

void SkyEffectTemplate::setMtrParams()
{
	
}

void SkyEffectTemplate::setSkeletonParams()
{

}

void SkyEffectTemplate::applyConstantBuffers()
{
	vector<D3D11ConstanceBuffer*>::type cbuffers0, cbuffers1;

	uploadConstanceBuffer(&sky_params_, sky_cbuff_);
	cbuffers0.push_back(sky_cbuff_->getCBuffer());
	cbuffers1.push_back(sky_cbuff_->getCBuffer());


	uploadConstanceBuffer(&ps_params_, ps_cbuff_);
	cbuffers1.push_back(ps_cbuff_->getCBuffer());

	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

}