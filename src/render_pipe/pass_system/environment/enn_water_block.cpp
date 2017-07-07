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
// Time: 2015/09/24
// File: enn_water_block.cpp
//

#include "enn_water_block.h"
#include "enn_ocean_eff_template.h"
#include "enn_effect_template_manager.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{


WaterBlock::WaterBlock()
{
	water_type_ = Lake;

	grid_element_size_ = 5;
}

WaterBlock::~WaterBlock()
{

}

bool WaterBlock::init()
{
	WaterObject::init();
	setupVBIB();

	return true;
}

void WaterBlock::setupVBIB()
{
	// create vertex buffer
	int iVertexCount = width_ * height_;

	float block_width = grid_element_size_ * (width_ - 1);
	float block_height = grid_element_size_ * (height_ - 1);
	float xleft = -block_width * 0.5f;
	float zleft = -block_height * 0.5f;
	Vector3 left_pos = Vector3(xleft, 0, zleft);
	Color2 water_color(31, 56, 64, 127);

	Array<WaterVertex> wv_buffer;
	wv_buffer.reserve(width_ * height_);

	for (int j = 0; j < height_; ++j)
	{
		for (int i = 0; i < width_; ++i)
		{
			WaterVertex wv;
			wv.vertex = Vector3(i * grid_element_size_, 0, j * grid_element_size_) + left_pos;
			wv_buffer.push_back(wv);
		}
	}

	vb_ = g_render_device_->createVertexBuffer(&wv_buffer[0], iVertexCount * sizeof(Vector3), false);

	Array<uint16> indices_buffer;
	int indices_num = (width_ - 1) * (height_ - 1) * 6;

	indices_buffer.resize(indices_num);
	int index = 0;

	//////////////////////////
	//   0----2----+ ...
	//   |  / |    |
	//   |/   |    |
	//   1----3----+ ...
	//   |    |    |
	//   |    |    |
	//   +----+----+ ...
	for (int j = 0; j < height_ - 1; ++j)
	{
		for (int i = 0; i < width_ - 1; ++i, index += 6)
		{
			indices_buffer[index + 0] = (j)* width_ + i;			// 0
			indices_buffer[index + 1] = (j + 1) * width_ + i;       // 1
			indices_buffer[index + 2] = j * width_ + i + 1;			// 2
			indices_buffer[index + 3] = (j + 1) * width_ + i;       // 1
			indices_buffer[index + 4] = (j + 1) * width_ + i + 1;   // 3
			indices_buffer[index + 5] = j * width_ + i + 1;			// 2
		}
	}

	ib_ = g_render_device_->createIndexBuffer(&indices_buffer[0], indices_num * sizeof(uint16), false);

	// create render layout
	render_layout_ = ENN_NEW D3D11InputLayout();

	render_layout_->setTopologyType(TT_TRIANGLE_LIST);

	render_layout_->bindVertexStream(vb_->getVertexBuffer(), MASK_P3);
	render_layout_->bindIndexStream(ib_->getIndexBuffer(), EF_R16UI);

	object_aabb_.setExtents(xleft, 0, zleft, -xleft, 0, -zleft);
}

void WaterBlock::render()
{
	OceanEffectTemplate* ocean_tpl = (OceanEffectTemplate*)(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_OCEAN));

	EffectMethod em;

	Camera* cam = g_render_pipe_->getMainCam();

	if (isUnderWater(cam->getEyePos()))
	{
		em.enableUNDERWATER();
	}

	EffectContext* eff_ctx = ocean_tpl->getEffectContext(em);

	OceanEffectParams params;
	params.curr_render_cam_ = cam;
	params.water_obj_ = this;

	ocean_tpl->doSetParams(eff_ctx, 0, &params);
	g_render_device_->doRender(*render_layout_, eff_ctx->eff_);
}

const AxisAlignedBox& WaterBlock::getObjectAABB() const
{
	return object_aabb_;
}

void WaterBlock::setWaterBound(float ww, float wh, float grid_size)
{
	width_ = ww / grid_size;
	height_ = wh / grid_size;

	grid_element_size_ = ww / width_;

	width_ += 1;
	height_ += 1;
}

}