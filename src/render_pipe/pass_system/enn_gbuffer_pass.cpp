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
// Time: 2014/06/22
// File: enn_gbuffer_pass.cpp

#include "enn_gbuffer_pass.h"
#include "enn_render_pipe.h"
#include "enn_general_render.h"
#include "enn_root.h"

namespace enn
{

GBufferPass::GBufferPass() : rtt_(0)
{

}

GBufferPass::~GBufferPass()
{

}

void GBufferPass::prepare()
{

}

void GBufferPass::render()
{
	checkRTT();

	generateGBuffers();

#if ENN_DEBUG
	//renderDebug();
#endif
}

void GBufferPass::generateGBuffers()
{
	g_general_render_->beginRender(GeneralRender::GR_GBUFFER);

	const RenderableNodeList& ra_list = g_render_pipe_->getMainRAList();
	ENN_FOR_EACH_CONST(RenderableNodeList, ra_list, it)
	{
		RenderableNode* ra_node = *it;

		ra_node->addToRender();
	}

	g_general_render_->endAdd();

	ForwardEffectParams eff_params;
	eff_params.render_cam_ = g_render_pipe_->getMainCam();

	D3D11RenderTarget* rtt = rtt_->getRTT();
	rtt->beginRender();
	rtt->clear(Color::WHITE);
	g_general_render_->renderSolidWS(eff_params);
	rtt->endRender();
}

void GBufferPass::checkRTT()
{
	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	if (rtt_)
	{
		uint32 w, h;
		rtt_->getDimension(w, h);

		if (uint32(vp.getWidth() + 0.5f) != w || uint32(vp.getHeight() + 0.5) != h)
		{
			ENN_SAFE_DELETE(rtt_);
		}
	}
	
	if (!rtt_)
	{
		rtt_ = ENN_NEW D3D11RTT2DHelper();
		
		rtt_->setDimension(static_cast<uint32>(vp.getWidth() + 0.5f), static_cast<uint32>(vp.getHeight() + 0.5f));
		rtt_->createColorBuffer(0, EF_ARGB8);
		rtt_->attachRawDepthBuffer(g_render_wnd_->getDepthStencilRTV());
	}
}

void GBufferPass::renderDebug()
{
	D3D11TextureObj* color_buffer = rtt_->getColorBuffer(0);
	D3D11TextureObj* depth_buffer = rtt_->getDepthBuffer();

	SpriteRenderer::InstanceData inst_data;
	inst_data.draw_rect.X = 10;
	inst_data.draw_rect.Y = 560;
	inst_data.draw_rect.width = 200;
	inst_data.draw_rect.height = 200;

	g_sprite_renderer_->render(color_buffer->getResourceProxy(), &inst_data, 1);

	inst_data.draw_rect.X = 220;
	//g_sprite_renderer_->render(depth_buffer->getResourceProxy(), &inst_data, 1);
}

}