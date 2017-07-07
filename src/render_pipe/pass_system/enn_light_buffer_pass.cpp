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
// Time: 2014/07/16
// File: enn_light_buffer_pass.cpp

#include "enn_light_buffer_pass.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{

LightBufferPass::LightBufferPass() :rtt_(0), point_ec_(0), spot_ec_(0)
{

}

LightBufferPass::~LightBufferPass()
{

}

void LightBufferPass::prepare()
{

}

void LightBufferPass::render()
{
	checkRTT();

	beginRender();

	const LightNodeList& lit_list = g_render_pipe_->getMainLitList();
	ENN_FOR_EACH_CONST(LightNodeList, lit_list, it)
	{
		LightNode* lit_node = *it;
		Light* lit = lit_node->getLight();

		if (lit->getLightType() == LT_POINT)
		{
			renderPointLit(static_cast<PointLight*>(lit));
		}
		else if (lit->getLightType() == LT_SPOT)
		{
			renderSpotLit(static_cast<SpotLight*>(lit));
		}
	}

	endRender();

#if ENN_DEBUG
	//renderDebug();
#endif
}

D3D11ResourceProxy* LightBufferPass::getLitBuffer() const
{
	return rtt_->getColorBuffer(0)->getResourceProxy();
}

void LightBufferPass::beginRender()
{
	D3D11RenderTarget* rtt = rtt_->getRTT();
	rtt->beginRender();
	rtt->clear(Color(0, 0, 0, 0));

	g_render_device_->setBlendState(g_render_device_->trans_add_blend_state_);
	g_render_device_->setDepthStencilState(g_render_device_->depth_always_state_, 1);

	if (!point_ec_ || !spot_ec_)
	{
		EffectMethod em_point, em_spot;
		em_point.enableLighting();
		em_spot.enableLighting();

		em_point.enablePointLits();
		em_spot.enableSpotLits();

		LightBufferEffectTemplate* lit_template = static_cast<LightBufferEffectTemplate*>(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_LITBUFFER));

		point_ec_ = lit_template->getEffectContext(em_point);
		spot_ec_ = lit_template->getEffectContext(em_spot);
	}
}

void LightBufferPass::checkRTT()
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
		
		const D3D11Viewport& vp = g_render_pipe_->getMainVP();

		rtt_->setDimension(static_cast<uint32>(vp.getWidth() + 0.5f), static_cast<uint32>(vp.getHeight() + 0.5f));
		rtt_->createColorBuffer(0, EF_ARGB8);
	}
}

void LightBufferPass::endRender()
{
	D3D11RenderTarget* rtt = rtt_->getRTT();
	rtt->endRender();

	g_render_device_->setBlendState(g_render_device_->solid_blend_state_);
	g_render_device_->setRasterizerState(g_render_device_->cull_cw_state_);
}

void LightBufferPass::renderPointLit(PointLight* lit)
{
	LightBufferEffectParams params;

	D3D11RTT2DHelper* rtt = g_render_pipe_->getGBufferPass()->getRTT();
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.light_ = lit;
	params.normal_tex_ = rtt->getColorBuffer(0)->getResourceProxy();
	params.depth_tex_ = g_render_wnd_->getDepthStencilSRV();

	g_render_device_->setRasterizerState(g_render_device_->cull_ccw_state_);

	point_ec_->owner_->doSetParams(point_ec_, 0, &params);
	lit->getVolume()->getSubMesh(0)->render(point_ec_);
}

void LightBufferPass::renderSpotLit(SpotLight* spot)
{
	LightBufferEffectParams params;

	D3D11RTT2DHelper* rtt = g_render_pipe_->getGBufferPass()->getRTT();
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.light_ = spot;
	params.normal_tex_ = rtt->getColorBuffer(0)->getResourceProxy();
	params.depth_tex_ = g_render_wnd_->getDepthStencilSRV();

	point_ec_->owner_->doSetParams(point_ec_, 0, &params);
	spot->getVolume()->getSubMesh(0)->render(point_ec_);
}

void LightBufferPass::renderDebug()
{
	D3D11TextureObj* color_buffer = rtt_->getColorBuffer(0);

	SpriteRenderer::InstanceData inst_data;
	inst_data.draw_rect.X = 440;
	inst_data.draw_rect.Y = 560;
	inst_data.draw_rect.width = 200;
	inst_data.draw_rect.height = 200;

	g_sprite_renderer_->render(color_buffer->getResourceProxy(), &inst_data, 1);
}

}