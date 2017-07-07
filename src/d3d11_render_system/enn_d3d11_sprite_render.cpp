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
// Time: 2014/07/06
// File: enn_d3d11_sprite_render.cpp

#include "enn_d3d11_sprite_render.h"
#include "enn_root.h"

namespace enn
{

SpriteRenderer* g_sprite_renderer_ = 0;
//////////////////////////////////////////////////////////////////////////
SpriteRenderer::SpriteRenderer()
: tex_vp_cbuffer_(0)
, sprite_eff_(0)
{
	g_sprite_renderer_ = this;
}

SpriteRenderer::~SpriteRenderer()
{

}

bool SpriteRenderer::init()
{
	// load shader
	String base_path = g_file_system_->getRootPath();

	base_path += "/effect/";

	String fx_code, common_code;
	shader_getFileData(base_path + "sprite.fx", fx_code);
	shader_getFileData(base_path + "common.fxh", common_code);

	fx_code = common_code + fx_code;

	D3D11Shader* vs = g_render_device_->loadShader(VERTEX_SHADER, fx_code, "SpriteVS", "vs_5_0", 0);
	D3D11Shader* ps = g_render_device_->loadShader(PIXEL_SHADER, fx_code, "SpritePS", "ps_5_0", 0);

	ENN_ASSERT(vs && ps);

	sprite_eff_ = ENN_NEW D3D11Effect();

	sprite_eff_->attachShader(VERTEX_SHADER, vs);
	sprite_eff_->attachShader(PIXEL_SHADER, ps);

	// create sprite geometry
	SpriteRenderer::VertexData verts[] =
	{
		{ Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f) },
		{ Vector2(1.0f, 0.0f), Vector2(1.0f, 0.0f) },
		{ Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f) },
		{ Vector2(0.0f, 1.0f), Vector2(0.0f, 1.0f) }
	};

	uint16 idx[] =
	{
		3, 2, 1, 3, 1, 0
	};

	sprite_geometry_.setVertexType(MASK_P2U2);
	sprite_geometry_.setVertexCount(4);
	sprite_geometry_.setVertexData(&verts[0]);

	sprite_geometry_.setIdxType(IDX_16);
	sprite_geometry_.setIndexCount(6);
	sprite_geometry_.setIndexData(&idx[0]);

	return true;
}

bool SpriteRenderer::shutdown()
{
	ENN_SAFE_DELETE(sprite_eff_);
	return true;
}

void SpriteRenderer::render(D3D11ResourceProxy* tex, const InstanceData* inst_data, uint32 sprite_nums)
{
	sprite_geometry_.resetInst();

	for (uint32 i = 0; i < sprite_nums; ++i)
	{
		sprite_geometry_.addInstance(*inst_data);
	}

	sprite_geometry_.makeGPUBuffer();

	// apply effect
	g_render_device_->setD3D11Effect(sprite_eff_);

	const D3D11_TEXTURE2D_DESC& desc = tex->tex_2d_desc_->getRaw();

	Vector4 texAndViewportSize;
	texAndViewportSize.x = static_cast<float>(desc.Width);
	texAndViewportSize.y = static_cast<float>(desc.Height);

	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	texAndViewportSize.z = static_cast<float>(vp.getWidth());
	texAndViewportSize.w = static_cast<float>(vp.getHeight());

	if (!tex_vp_cbuffer_)
	{
		tex_vp_cbuffer_ = g_render_device_->createConstantBuffer(0, tex_vp_info_.size());
		
	}

	tex_vp_info_.TexAndViewportSize = (texAndViewportSize);

	uploadConstanceBuffer(&tex_vp_info_, tex_vp_cbuffer_);
	setConstantBufferHelper(VERTEX_SHADER, 0, tex_vp_cbuffer_);
	setTextureShaderResourceViewHelper(PIXEL_SHADER, 0, tex);

	g_render_device_->doRender(*sprite_geometry_.getRenderLayout(), sprite_eff_);
}

}