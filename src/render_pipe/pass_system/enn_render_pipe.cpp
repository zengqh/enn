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
// Time: 2014/05/22
// File: enn_render_pipe.cpp
//

#include "enn_render_pipe.h"
#include "enn_general_render.h"
#include "enn_root.h"

namespace enn
{

RenderPipe* g_render_pipe_ = 0;

RenderPipe::RenderPipe()
: lit_buffer_pass_(0)
, main_cam_(0)
, main_sg_(0)
, forward_pass_(0)
, gbuffer_pass_(0)
, ssao_pass_(0)
, post0_(0)
, post1_(0)
, water_pass_(0)
, enable_post_rtt_(true)
{
	g_render_pipe_ = this;
}

RenderPipe::~RenderPipe()
{

}

void RenderPipe::init()
{
	ENN_NEW GeneralRender();
	ENN_NEW EffectTemplateManager();

	createRenderPass();
	registerEffectTemplate();
	createSpriteRender();
}

void RenderPipe::shutdown()
{
	destroyRenderPass();

	ENN_SAFE_DELETE(g_general_render_);
	ENN_SAFE_DELETE(g_eff_template_mgr_);
}

void RenderPipe::render(CameraNode* cam)
{
	setMainCamera(cam);
	calcMainResults();

	gbuffer_pass_->render();
	lit_buffer_pass_->render();
	//ssao_pass_->render();

	g_environment_->getSky()->render();

	g_environment_->getTerrain()->render();

	main_result_pass_->render();

	// render water
	water_pass_->render();
}

void RenderPipe::queryLightsInfo(const AxisAlignedBox& aabb, LightsInfo& litsInfo, bool checkSM)
{
	litsInfo.clear();

	Vector3 pos = aabb.getCenter();

	ENN_FOR_EACH_CONST(LightNodeList, main_lit_list_, it)
	{
		LightNode* lit_node = *it;
		if (lit_node->intersects(aabb))
		{
			float dist = lit_node->squareDistance(pos);

			if (checkSM)
			{
				
			}

			litsInfo.addLight(lit_node->getLight(), dist, checkSM);
		}
	}
}

LightNode* RenderPipe::getSunLight() const
{
	ENN_FOR_EACH_CONST(LightNodeList, main_lit_list_, it)
	{
		LightNode* lit_node = *it;
		if (lit_node->getLight()->getLightType() == LT_DIRECTIONAL)
		{
			return lit_node;
		}
	}

	return 0;
}

void RenderPipe::calcMainResults()
{
	g_environment_->getTerrain()->clearVisibleSections();

	FindSceneMainResult finder(&main_ra_list_, &main_lit_list_, &main_probe_list_, &water_node_list_);
	finder.setFinder(main_cam_);

	finder.find(main_sg_);
}

void RenderPipe::setMainCamera(CameraNode* cam)
{
	main_cam_ = cam->getCamera();
	main_sg_ = cam->getSceneGraph();
}

void RenderPipe::createRenderPass()
{
	forward_pass_ = ENN_NEW RenderForwardPass();
	gbuffer_pass_ = ENN_NEW GBufferPass();
	lit_buffer_pass_ = ENN_NEW LightBufferPass();
	ssao_pass_ = ENN_NEW SSAO();
	main_result_pass_ = ENN_NEW MainResultPass();
	ENN_NEW Environment();
	water_pass_ = ENN_NEW WaterPass();
}

void RenderPipe::destroyRenderPass()
{
	ENN_SAFE_DELETE(forward_pass_);
	ENN_SAFE_DELETE(gbuffer_pass_);
	ENN_SAFE_DELETE(lit_buffer_pass_);
	ENN_SAFE_DELETE(ssao_pass_);
	ENN_SAFE_DELETE(main_result_pass_);
	ENN_SAFE_DELETE(water_pass_);
}

void RenderPipe::registerEffectTemplate()
{
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_FORWARD, ENN_NEW ForwardEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_GBUFFER, ENN_NEW GBufferEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_LITBUFFER, ENN_NEW LightBufferEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_MAIN_RESULT, ENN_NEW MainResultEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_SSAO, ENN_NEW SSAOEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_SSAO_FILTER, ENN_NEW SSAOFilterEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_SKY, ENN_NEW SkyEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_TERRAIN, ENN_NEW TerrainSectionEffectTemplate(g_eff_template_mgr_));
	g_eff_template_mgr_->registerEffectTemplate(GeneralRender::GR_OCEAN, ENN_NEW OceanEffectTemplate(g_eff_template_mgr_));
}

void RenderPipe::createSpriteRender()
{
	ENN_NEW SpriteRenderer();
	g_sprite_renderer_->init();
}

void RenderPipe::destroySpriteRender()
{
	ENN_SAFE_DELETE(g_sprite_renderer_);
}

void RenderPipe::createPostRTT(D3D11RTT2DHelper*& rtt)
{
	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	if (rtt)
	{
		uint32 w, h;
		rtt->getDimension(w, h);

		if (uint32(vp.getWidth() + 0.5f) != w || uint32(vp.getHeight() + 0.5) != h)
		{
			ENN_SAFE_DELETE(rtt);
		}
	}

	if (!rtt)
	{
		rtt = ENN_NEW D3D11RTT2DHelper();

		rtt->setDimension(static_cast<uint32>(vp.getWidth() + 0.5f), static_cast<uint32>(vp.getHeight() + 0.5f));
		rtt->createColorBuffer(0, EF_ARGB8);
		rtt->attachRawDepthBuffer(g_render_wnd_->getDepthStencilRTV());
	}
}

void RenderPipe::swapPost()
{
	std::swap(posts_[0], posts_[1]);
}

void RenderPipe::beginRender()
{
	main_vp_.update(0, 0, g_render_wnd_->getWidth(), g_render_wnd_->getHeight());
	g_render_device_->setViewport(main_vp_);

	g_render_wnd_->beginRender();
	g_render_wnd_->clear(g_root_->getRootSetting().clear_clr_);

	createPostRTT(post0_);
	createPostRTT(post1_);

	posts_[SRC_POST] = post0_;
	posts_[DST_POST] = post1_;

	if (enable_post_rtt_)
	{
		post0_->getRTT()->beginRender();
		post0_->getRTT()->clear(g_root_->getRootSetting().clear_clr_);
	}

	g_render_device_->beginFrame();
}

void RenderPipe::endRender()
{
	// render into 

	D3D11RTT2DHelper* src_rtt = getPostRTT(SRC_POST);
	if (enable_post_rtt_)
	{
		src_rtt->getRTT()->endRender();
	}

	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	SpriteRenderer::InstanceData inst_data;
	inst_data.draw_rect.X = 0;
	inst_data.draw_rect.Y = 0;
	inst_data.draw_rect.width = vp.getWidth();
	inst_data.draw_rect.height = vp.getHeight();

	if (enable_post_rtt_) g_sprite_renderer_->render(src_rtt->getColorBuffer(0)->getResourceProxy(), &inst_data, 1);

	// end frame
	g_render_device_->endFrame();
	g_render_wnd_->present();
}

}