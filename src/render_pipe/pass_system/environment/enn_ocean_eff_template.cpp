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
// Time: 2015/10/19
// File: enn_ocean_eff_template.cpp
//

#include "enn_ocean_eff_template.h"
#include "enn_water_object.h"
#include "enn_render_pipe.h"
#include "enn_texture_manager.h"
#include "enn_render_pipe.h"

namespace enn
{

OceanEffectTemplate::OceanEffectTemplate(EffectTemplateManager* owner)
	: EffectTemplate(owner)
	, cbuffer_init_(false)
	, water_vs_cbuff_(0)
	, water_ps_cbuff_(0)
{

}

OceanEffectTemplate::~OceanEffectTemplate()
{

}

EffectContext* OceanEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "water_v2.fx", fx_code);
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

EffectContext* OceanEffectTemplate::getEffectContext(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "water_v2.fx", fx_code);
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

void OceanEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<OceanEffectParams*>(params);

	applyPipelineResources();
}


void OceanEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void OceanEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setWaterParams();

	applyConstantBuffers();
}

void OceanEffectTemplate::bindSamplerObjects()
{
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, 0, g_render_device_->linear_wrap_state_);
}

void OceanEffectTemplate::bindShaderResourceViews()
{
	WaterObject* obj = curr_eff_params_->water_obj_;

	// bump map
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, obj->ripple_tex_obj_->getResourceProxy());

	// refractBuff
	D3D11ResourceProxy* refract_buff = g_render_pipe_->getWaterPass()->getBackRTT();
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, refract_buff);

	// skyMap
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 2, obj->sky_map_obj_->getResourceProxy());
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 3, obj->foam_tex_obj_->getResourceProxy());

	// depth texture
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 4, g_render_pipe_->getWaterPass()->getDepthRTT());
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 5, obj->depth_gradient_tex_obj_->getResourceProxy());
}

void OceanEffectTemplate::bindUnorderedAccessViews()
{

}

void OceanEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	water_vs_cbuff_ = g_render_device_->createConstantBuffer(0, water_vs_params_.size());
	water_ps_cbuff_ = g_render_device_->createConstantBuffer(0, water_ps_params_.size());

	cbuffer_init_ = true;
}

void OceanEffectTemplate::setWaterParams()
{
	Camera* cam = curr_eff_params_->curr_render_cam_;
	WaterObject* obj = curr_eff_params_->water_obj_;

	const Matrix4& world_matrix = obj->getSceneNode()->getDerivedMatrix();
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 wvp = (device_proj_matrix * device_view_matrix) * world_matrix;
	Matrix4 wv = device_view_matrix * world_matrix;


	//////////////////////////////////////////////////////////////////////////
	water_vs_params_.modelMat = world_matrix;
	water_vs_params_.modelviewProj = wvp;
	water_vs_params_.matWV = wv;
	water_vs_params_.matProj = device_proj_matrix;
	water_vs_params_.elapsedTime.x = g_time_system_->getCurrSystemTime() * 0.001f;

	for (uint32 i = 0; i < WaterObject::MAX_WAVES; i++)
	{
		water_vs_params_.rippleDir[i].setValue(-obj->ripple_dir_[i].x, -obj->ripple_dir_[i].y, 0, 0);
	}
	
	water_vs_params_.rippleSpeed.setValue(obj->ripple_speed_[0], obj->ripple_speed_[1], obj->ripple_speed_[2], 1);

	for (uint32 i = 0; i < WaterObject::MAX_WAVES; i++)
	{
		float2 texScale = obj->ripple_tex_scale_[i];
		if (texScale.x > 0.0)
			texScale.x = 1.0 / texScale.x;
		if (texScale.y > 0.0)
			texScale.y = 1.0 / texScale.y;

		water_vs_params_.rippleTexScale[i].setValue(texScale.x, texScale.y, 0, 0);
	}


	float angle, cosine, sine;
	for (uint32 i = 0; i < WaterObject::MAX_WAVES; i++)
	{
		angle = Math::atan2(obj->ripple_dir_[i].x, -obj->ripple_dir_[i].y);
		cosine = Math::cos(angle);
		sine = Math::sin(angle);

		water_vs_params_.rippleMat[i].setValue(cosine, sine, -sine, cosine);
	}


	// Waves...
	for (uint32 i = 0; i < WaterObject::MAX_WAVES; i++)
	{
		water_vs_params_.waveDir[i].setValue(-obj->wave_dir_[i].x, -obj->wave_dir_[i].y, 0, 0);
	}
		
	for (uint32 i = 0; i < WaterObject::MAX_WAVES; i++)
	{
		water_vs_params_.waveData[i].setValue(-obj->wave_speed_[i], obj->wave_magnitude_[i] * obj->overall_wave_magnitude_, 0, 0);
	}

	// Foam...
	water_vs_params_.foamDir.setValue(obj->foam_dir_[0].x, obj->foam_dir_[0].y, obj->foam_dir_[1].x, obj->foam_dir_[1].y);

	water_vs_params_.foamSpeed.setValue(obj->foam_speed_[0], obj->foam_speed_[2], 0, 0);

	water_vs_params_.foamTexScale.setValue(obj->foam_tex_scale_[0].x, obj->foam_tex_scale_[0].y, 
		obj->foam_tex_scale_[1].x, obj->foam_tex_scale_[1].y);

	for (uint32 i = 0; i < 4; i++)
	{
		if (water_vs_params_.foamTexScale[i] > 0.0f)
			water_vs_params_.foamTexScale[i] = 1.0 / water_vs_params_.foamTexScale[i];
	}

	//////////////////////////////////////////////////////////////////////////
	water_ps_params_.rippleMagnitude.setValue(
		obj->ripple_magnitude_[0],
		obj->ripple_magnitude_[1],
		obj->ripple_magnitude_[2],
		obj->overall_ripple_magnitude_);

	
	water_ps_params_.baseColor.setValue(29 / 255.0f, 145 / 255.0f, 221 / 255.0f, 1);

	water_ps_params_.eyePosW = curr_eff_params_->curr_render_cam_->getEyePos();

	water_ps_params_.fogParams.setValue(obj->water_fog_data_.density, obj->water_fog_data_.densityOffset,1,1);
	water_ps_params_.farPlaneDist.setValue(cam->getZFar(), 1, 1, 1);
	water_ps_params_.distortionParams.setValue(obj->distort_start_dist_, obj->distort_end_dist_, obj->distort_full_depth_,1);

	DirectLight* sun_light = g_render_pipe_->getSunLight()->getLightAs<DirectLight>();
	water_ps_params_.inLightVec = float4(sun_light->getDirection(), 1);
	water_ps_params_.ambientColor = sun_light->getDiffuse().getAsVector4();
	water_ps_params_.foamParams.setValue(obj->overall_foam_opacity_, obj->foam_max_depth_, obj->foam_ambient_lerp_, obj->foam_ripple_influence_);
	water_ps_params_.miscParams.setValue(obj->fresnel_bias_, obj->fresnel_power_, 1, 1);
	
	float4 specularParams(obj->specular_color_.r, obj->specular_color_.g, 
		obj->specular_color_.b, obj->specular_power_);

	const Color &sunColor = sun_light->getDiffuse();

	float brightness = 1.0f;

	specularParams.x *= sunColor.r * brightness;
	specularParams.y *= sunColor.g * brightness;
	specularParams.z *= sunColor.b * brightness;

	water_ps_params_.specularParams = specularParams;
	water_ps_params_.depthGradMax.x = obj->depth_gradient_max_;
	water_ps_params_.reflectivity = obj->reflectivity_;
}


void OceanEffectTemplate::applyConstantBuffers()
{
	vector<D3D11ConstanceBuffer*>::type cbuffers0, cbuffers1;

	uploadConstanceBuffer(&water_vs_params_, water_vs_cbuff_);
	cbuffers0.push_back(water_vs_cbuff_->getCBuffer());
	cbuffers1.push_back(water_vs_cbuff_->getCBuffer());

	uploadConstanceBuffer(&water_ps_params_, water_ps_cbuff_);
	cbuffers0.push_back(water_ps_cbuff_->getCBuffer());
	cbuffers1.push_back(water_ps_cbuff_->getCBuffer());

	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

}