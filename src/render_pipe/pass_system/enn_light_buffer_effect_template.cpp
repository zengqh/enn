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
// Time: 2014/07/18
// File: enn_light_buffer_effect_template.cpp

#include "enn_light_buffer_effect_template.h"

namespace enn
{

LightBufferEffectTemplate::LightBufferEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, cbuffer_init_(false)
, transform_cbuff_(0)
, point_cbuff_(0)
, spot_cbuff_(0)
{

}

LightBufferEffectTemplate::~LightBufferEffectTemplate()
{

}

EffectContext* LightBufferEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "light_buffer.fx", fx_code);
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

EffectContext* LightBufferEffectTemplate::getEffectContext(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "light_buffer.fx", fx_code);
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

void LightBufferEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<LightBufferEffectParams*>(params);

	applyPipelineResources();
}

void LightBufferEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void LightBufferEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();
	setPointLitParams();
	setSpotLitParams();

	applyConstantBuffers();
}

void LightBufferEffectTemplate::bindSamplerObjects()
{

}

void LightBufferEffectTemplate::bindShaderResourceViews()
{
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, curr_eff_params_->normal_tex_);
	g_render_device_->setD3D11ShaderResourceView(PIXEL_SHADER, 1, curr_eff_params_->depth_tex_);
}

void LightBufferEffectTemplate::bindUnorderedAccessViews()
{

}

void LightBufferEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	transform_cbuff_ = g_render_device_->createConstantBuffer(0, transform_params_.size());
	point_cbuff_ = g_render_device_->createConstantBuffer(0, point_params_.size());
	spot_cbuff_ = g_render_device_->createConstantBuffer(0, spot_params_.size());

	cbuffer_init_ = true;
}

void LightBufferEffectTemplate::setTransformParams()
{

	const Matrix4& world_matrix = curr_eff_params_->light_->getWorldMatrix();
	const Matrix4& view_matrix = curr_eff_params_->curr_render_cam_->getViewMatrix();
	const Matrix4& proj_matrix = curr_eff_params_->curr_render_cam_->getProjMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 wvp = device_proj_matrix * device_view_matrix * world_matrix;
	Matrix4 wv = device_view_matrix * world_matrix;

	transform_params_.matWV = (wv);
	transform_params_.matWVP = (wvp);
	transform_params_.proj_matrix = (device_proj_matrix);
}

void LightBufferEffectTemplate::setPointLitParams()
{
	Light* lit = curr_eff_params_->light_;

	if (lit->getLightType() == LT_POINT)
	{
		PointLight* pt_lit = static_cast<PointLight*>(lit);

		Vector4 diffuse = pt_lit->getDiffuse().getAsVector4();
		point_params_.diffuse = (diffuse);

		Matrix4 device_view_matrix;
		g_render_device_->convertViewMatrix(curr_eff_params_->curr_render_cam_->getViewMatrix(), device_view_matrix);

		Vector3 pos_view = device_view_matrix.transformAffine(pt_lit->getPosition());
		point_params_.position = (Vector4(pos_view, pt_lit->_getFadeScale()));
	}
}

void LightBufferEffectTemplate::setSpotLitParams()
{
	Light* lit = curr_eff_params_->light_;

	if (lit->getLightType() == LT_SPOT)
	{
		SpotLight* spot_lit = static_cast<SpotLight*>(lit);

		Matrix4 device_view_matrix;
		g_render_device_->convertViewMatrix(curr_eff_params_->curr_render_cam_->getViewMatrix(), device_view_matrix);

		Vector3 pos_view = device_view_matrix.transformAffine(spot_lit->getPosition());
		spot_params_.position = (Vector4(pos_view, spot_lit->getFadeDist()));

		Vector3 dir_view = device_view_matrix.transformAffineNormal(-spot_lit->getDirection());
		Vector4 dir_view_angle_x = Vector4(dir_view, spot_lit->_getSpotAngles().x);

		Vector4 diffuse = spot_lit->getDiffuse().getAsVector4();
		diffuse.w = spot_lit->_getSpotAngles().y;

		spot_params_.diffuse = (diffuse);
	}
}

void LightBufferEffectTemplate::applyConstantBuffers()
{
	EffectMethod& em = curr_eff_context_->eff_method_;

	vector<D3D11ConstanceBuffer*>::type cbuffers0, cbuffers1;

	uploadConstanceBuffer(&transform_params_, transform_cbuff_);
	cbuffers0.push_back(transform_cbuff_->getCBuffer());
	cbuffers1.push_back(transform_cbuff_->getCBuffer());

	Light* lit = curr_eff_params_->light_;
	if (lit->getLightType() == LT_POINT)
	{
		uploadConstanceBuffer(&point_params_, point_cbuff_);
		cbuffers1.push_back(point_cbuff_->getCBuffer());
	}

	if (lit->getLightType() == LT_SPOT)
	{
		uploadConstanceBuffer(&spot_params_, spot_cbuff_);
		cbuffers1.push_back(spot_cbuff_->getCBuffer());
	}

	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
	g_render_device_->setD3D11ConstantBuffers(PIXEL_SHADER, cbuffers1);
}

}