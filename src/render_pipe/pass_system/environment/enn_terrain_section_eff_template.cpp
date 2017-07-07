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
// Time: 2015/02/13
// File: enn_terrain_section_eff_template.cpp
//

#include "enn_terrain_section_eff_template.h"

namespace enn
{

TerrainSectionEffectTemplate::TerrainSectionEffectTemplate(EffectTemplateManager* owner)
	: EffectTemplate(owner)
	, cbuffer_init_(false)
	, transform_cbuff_(0)
	, normal_fit_tex_(0)
{

}

TerrainSectionEffectTemplate::~TerrainSectionEffectTemplate()
{

}

EffectContext* TerrainSectionEffectTemplate::getEffectContext(Renderable* ra)
{
	ENN_ASSERT(0);
	return 0;
}

EffectContext* TerrainSectionEffectTemplate::getEffectContextNormal(EffectMethod em)
{
	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "terrain.fx", fx_code);
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

EffectContext* TerrainSectionEffectTemplate::getEffectContextDeferred(EffectMethod em)
{
	EffectContextMap::iterator iter = deferred_eff_ctx_map_.find(em);
	if (iter != deferred_eff_ctx_map_.end())
	{
		return iter->second;
	}

	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, comm_code;
	shader_getFileData(base_path + "terrain_gbuffer.fx", fx_code);
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

void TerrainSectionEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	curr_eff_context_ = eff_ctx;
	curr_ra_ = ra;
	curr_eff_params_ = static_cast<TerrainSectionEffectParams*>(params);

	applyPipelineResources();
}


void TerrainSectionEffectTemplate::bindShaderPrograms()
{
	g_render_device_->setD3D11Effect(curr_eff_context_->eff_);
}

void TerrainSectionEffectTemplate::bindConstantBuffers()
{
	createConstantBuffers();

	setTransformParams();
	setMtrParams();
	setSkeletonParams();

	applyConstantBuffers();
}

void TerrainSectionEffectTemplate::bindSamplerObjects()
{
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, 0, g_render_device_->linear_wrap_state_);
}

void TerrainSectionEffectTemplate::bindShaderResourceViews()
{
	Terrain* terrain = curr_eff_params_->section_->getOwner();

	D3D11TextureObj* weight_map = curr_eff_params_->weight_map_;
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, weight_map->getResourceProxy());

	if (curr_eff_params_->is_deferred_)
	{
		D3D11TextureObj* normal_map0 = terrain->getNormalMap(0);
		D3D11TextureObj* normal_map1 = terrain->getNormalMap(1);
		D3D11TextureObj* normal_map2 = terrain->getNormalMap(2);
		D3D11TextureObj* normal_map3 = terrain->getNormalMap(3);

		EffectMethod em = curr_eff_params_->em_;

		if (em.hasDETAIL_LEVEL1())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, normal_map0->getResourceProxy());
		}

		if (em.hasDETAIL_LEVEL2())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, normal_map1->getResourceProxy());
		}

		if (em.hasDETAIL_LEVEL3())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, normal_map2->getResourceProxy());
		}

		if (em.hasDETAIL_LEVEL4())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, normal_map3->getResourceProxy());
		}
	}
	else
	{
		int layer0 = curr_eff_params_->layer[0];
		int layer1 = curr_eff_params_->layer[1];
		int layer2 = curr_eff_params_->layer[2];
		int layer3 = curr_eff_params_->layer[3];

		if (curr_eff_params_->em_.hasDETAIL_LEVEL4())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, terrain->getDetailMap(layer0)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 2, terrain->getDetailMap(layer1)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 3, terrain->getDetailMap(layer2)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 4, terrain->getDetailMap(layer3)->getResourceProxy());
		}
		else if (curr_eff_params_->em_.hasDETAIL_LEVEL3())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, terrain->getDetailMap(layer0)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 2, terrain->getDetailMap(layer1)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 3, terrain->getDetailMap(layer2)->getResourceProxy());
		}
		else if (curr_eff_params_->em_.hasDETAIL_LEVEL2())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, terrain->getDetailMap(layer0)->getResourceProxy());
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 2, terrain->getDetailMap(layer1)->getResourceProxy());
		}
		else if (curr_eff_params_->em_.hasDETAIL_LEVEL1())
		{
			setTextureShaderResourceViewHelper(PIXEL_SHADER, 1, terrain->getDetailMap(layer0)->getResourceProxy());
		}
	}
}

void TerrainSectionEffectTemplate::bindUnorderedAccessViews()
{

}

void TerrainSectionEffectTemplate::createConstantBuffers()
{
	if (cbuffer_init_)
	{
		return;
	}

	transform_cbuff_ = g_render_device_->createConstantBuffer(0, terrain_params_.size());

	cbuffer_init_ = true;
}

void TerrainSectionEffectTemplate::setTransformParams()
{
	Camera* cam = curr_eff_params_->curr_render_cam_;

	const Matrix4 world_matrix;
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	Matrix4 device_proj_matrix, device_view_matrix;

	g_render_device_->convertProjectionMatrix(proj_matrix, device_proj_matrix);
	g_render_device_->convertViewMatrix(view_matrix, device_view_matrix);

	Matrix4 wvp = (device_proj_matrix * device_view_matrix) * world_matrix;
	Matrix4 wv = device_view_matrix * world_matrix;

	terrain_params_.matWV = wv;
	terrain_params_.matWVP = wvp;
	terrain_params_.gTransform = curr_eff_params_->gTransform;
	terrain_params_.gUVParam = curr_eff_params_->gUVParam;
	terrain_params_.gUVScale = curr_eff_params_->gUVScale;
}

void TerrainSectionEffectTemplate::setMtrParams()
{
	
}

void TerrainSectionEffectTemplate::setSkeletonParams()
{

}

void TerrainSectionEffectTemplate::applyConstantBuffers()
{
	vector<D3D11ConstanceBuffer*>::type cbuffers0;

	uploadConstanceBuffer(&terrain_params_, transform_cbuff_);
	cbuffers0.push_back(transform_cbuff_->getCBuffer());


	g_render_device_->setD3D11ConstantBuffers(VERTEX_SHADER, cbuffers0);
}

}