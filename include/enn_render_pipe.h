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
// Time: 2014/05/15
// File: enn_render_pipe.h
//

#pragma once

#include "enn_effect_template_manager.h"
#include "enn_main_result_pass.h"
#include "enn_forward_pass.h"
#include "enn_gbuffer_pass.h"
#include "enn_light_buffer_pass.h"
#include "enn_ssao.h"
#include "enn_environment.h"
#include "enn_water_pass.h"

namespace enn
{

class RenderPipe : public AllocatedObject
{
public:
	enum PostDesc
	{
		SRC_POST,
		DST_POST
	};

public:
	RenderPipe();
	virtual ~RenderPipe();

	void init();
	void shutdown();

	void render(CameraNode* cam);

public:
	const RenderableNodeList& getMainRAList() const { return main_ra_list_; }
	const LightNodeList& getMainLitList() const { return main_lit_list_; }
	const EnvProbeNodeList& getMainEnvProbeList() const { return main_probe_list_; }
	const WaterNodeList& getWaterNodeList() const { return water_node_list_; }
	Camera* getMainCam() const { return main_cam_; }
	void queryLightsInfo(const AxisAlignedBox& aabb, LightsInfo& litsInfo, bool checkSM);
	LightNode* getSunLight() const;

	D3D11RTT2DHelper* getPostRTT(int type) const { return posts_[type]; }

protected:
	void calcMainResults();
	void setMainCamera(CameraNode* cam);

protected:
	void createRenderPass();
	void destroyRenderPass();

	void registerEffectTemplate();
	void createSpriteRender();
	void destroySpriteRender();

	void createPostRTT(D3D11RTT2DHelper*& rtt);
	void swapPost();

public:
	void beginRender();
	void endRender();

	const D3D11Viewport& getMainVP() const { return main_vp_; }

public:
	GBufferPass* getGBufferPass() const { return gbuffer_pass_; }
	RenderForwardPass* getForwardPass() const { return forward_pass_; }
	LightBufferPass* getLightBufferPass() const { return lit_buffer_pass_; }
	WaterPass* getWaterPass() const { return water_pass_; }

protected:
	RenderableNodeList				main_ra_list_;
	LightNodeList					main_lit_list_;
	EnvProbeNodeList				main_probe_list_;
	WaterNodeList					water_node_list_;

	Camera*							main_cam_;
	SceneGraph*						main_sg_;

	RenderForwardPass*				forward_pass_;
	GBufferPass*					gbuffer_pass_;
	LightBufferPass*				lit_buffer_pass_;
	SSAO*							ssao_pass_;
	MainResultPass*					main_result_pass_;
	WaterPass*						water_pass_;

	D3D11Viewport					main_vp_;

	D3D11RTT2DHelper*				post0_;
	D3D11RTT2DHelper*				post1_;

	D3D11RTT2DHelper*				posts_[2];

	bool							enable_post_rtt_;
};

extern RenderPipe* g_render_pipe_;
}