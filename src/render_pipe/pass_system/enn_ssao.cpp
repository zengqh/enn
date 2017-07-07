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
// File: enn_ssao.cpp
//

#include "enn_ssao.h"
#include "enn_root.h"
#include "enn_general_render.h"
#include "enn_ssao_effect_template.h"
#include "enn_ssao_filter_effect_template.h"

namespace enn
{

SSAO::SSAO() : rtt_(0), ao_eff_(0), res_size_(RESSIZE_FULL)
, rtt_h_blur_(0)
, rtt_v_blur_(0)
{

}

SSAO::~SSAO()
{

}

void SSAO::prepare()
{
	
}

void SSAO::render()
{
	prepareMesh();
	checkRTT();

	generateAO();

	
	blurH();
	blurV();

	renderDebug();
}

void SSAO::checkRTT()
{
	if (!rtt_)
	{
		rtt_ = ENN_NEW D3D11RTT2DHelper();

		const D3D11Viewport& vp = g_render_pipe_->getMainVP();

		int w = vp.getWidth() + 0.5f;
		int h = vp.getHeight() + 0.5f;

		IntVector2 res_size = getResolutionSize(w, h, res_size_);
		rtt_->setDimension(w, h);
		rtt_->createColorBuffer(0, EF_ARGB8);
	}

	if (!rtt_h_blur_)
	{
		rtt_h_blur_ = ENN_NEW D3D11RTT2DHelper();

		const D3D11Viewport& vp = g_render_pipe_->getMainVP();

		int w = vp.getWidth() + 0.5f;
		int h = vp.getHeight() + 0.5f;

		IntVector2 res_size = getResolutionSize(w, h, res_size_);
		rtt_h_blur_->setDimension(w, h);
		rtt_h_blur_->createColorBuffer(0, EF_ARGB8);
	}

	if (!rtt_v_blur_)
	{
		rtt_v_blur_ = ENN_NEW D3D11RTT2DHelper();

		const D3D11Viewport& vp = g_render_pipe_->getMainVP();

		int w = vp.getWidth() + 0.5f;
		int h = vp.getHeight() + 0.5f;

		IntVector2 res_size = getResolutionSize(w, h, res_size_);
		rtt_v_blur_->setDimension(w, h);
		rtt_v_blur_->createColorBuffer(0, EF_ARGB8);
	}
}

void SSAO::beginRender()
{
	D3D11RenderTarget* rtt = rtt_->getRTT();
	rtt->beginRender();
	rtt->clear(Color(1, 1, 1, 1));

	g_render_device_->setBlendState(g_render_device_->solid_blend_state_);
	g_render_device_->setDepthStencilState(g_render_device_->depth_none_state_, 1);
}

void SSAO::endRender()
{

}

void SSAO::prepareMesh()
{
	if (screen_mesh_)
	{
		return;
	}

	MeshPtr full_screen_mesh(g_mesh_mgr_->getScreenQuadMesh());
	screen_mesh_ = full_screen_mesh;
}

void SSAO::generateAO()
{
	D3D11RenderTarget* rtt = rtt_->getRTT();
	rtt->beginRender();
	rtt->clear(Color::WHITE);

	SSAOEffectTemplate* eff_template = static_cast<SSAOEffectTemplate*>(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_SSAO));
	SubMesh* ra = screen_mesh_->getSubMesh(0);

	EffectMethod em;
	EffectContext* eff_ctx = eff_template->getEffectContext(em);

	SSAOEffectParams params;
	D3D11RTT2DHelper* grtt = g_render_pipe_->getGBufferPass()->getRTT();
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.normal_tex_ = grtt->getColorBuffer(0)->getResourceProxy();
	params.depth_tex_ = grtt->getDepthBuffer()->getResourceProxy();

	eff_template->doSetParams(eff_ctx, 0, &params);
	ra->render(eff_ctx);

	rtt->endRender();
}

void SSAO::blurH()
{
	D3D11RenderTarget* rtt = rtt_h_blur_->getRTT();
	rtt->beginRender();
	rtt->clear(Color::WHITE);

	SSAOFilterEffectTemplate* eff_template = static_cast<SSAOFilterEffectTemplate*>(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_SSAO_FILTER));
	SubMesh* ra = screen_mesh_->getSubMesh(0);

	EffectMethod em;
	EffectContext* eff_ctx = eff_template->getEffectContext(em);

	SSAOFilterEffectParams params;
	D3D11RTT2DHelper* grtt = g_render_pipe_->getGBufferPass()->getRTT();
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.ao_tex_ = grtt->getColorBuffer(0)->getResourceProxy();
	params.depth_tex_ = grtt->getDepthBuffer()->getResourceProxy();
	params.is_horz = true;

	eff_template->doSetParams(eff_ctx, 0, &params);
	ra->render(eff_ctx);

	rtt->endRender();
}

void SSAO::blurV()
{
	D3D11RenderTarget* rtt = rtt_v_blur_->getRTT();
	rtt->beginRender();
	rtt->clear(Color::WHITE);

	SSAOFilterEffectTemplate* eff_template = static_cast<SSAOFilterEffectTemplate*>(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_SSAO_FILTER));
	SubMesh* ra = screen_mesh_->getSubMesh(0);

	EffectMethod em;
	EffectContext* eff_ctx = eff_template->getEffectContext(em);

	SSAOFilterEffectParams params;
	D3D11RTT2DHelper* grtt = g_render_pipe_->getGBufferPass()->getRTT();
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.ao_tex_ = grtt->getColorBuffer(0)->getResourceProxy();
	params.depth_tex_ = grtt->getDepthBuffer()->getResourceProxy();
	params.is_horz = false;

	eff_template->doSetParams(eff_ctx, 0, &params);
	ra->render(eff_ctx);

	rtt->endRender();
}

void SSAO::renderDebug()
{
	D3D11TextureObj* color_buffer = rtt_v_blur_->getColorBuffer(0);

	SpriteRenderer::InstanceData inst_data;
	/*
	inst_data.draw_rect.X = 660;
	inst_data.draw_rect.Y = 560;
	inst_data.draw_rect.width = 200;
	inst_data.draw_rect.height = 200;
	*/

	inst_data.draw_rect.X = 0;
	inst_data.draw_rect.Y = 0;
	inst_data.draw_rect.width = 1024;
	inst_data.draw_rect.height = 768;

	g_sprite_renderer_->render(color_buffer->getResourceProxy(), &inst_data, 1);
}

}