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
// Time: 2015/01/07
// File: enn_terrain_section.cpp
//

#include "enn_terrain_section.h"
#include "enn_effect_template_manager.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{

TerrainSection::TerrainSection()
: morph_(0)
, render_layout_(0)
{
	type_flag_.set_flag(NODE_TERRAIN_SECTION);
}

TerrainSection::~TerrainSection()
{

}

void TerrainSection::create(Terrain* terrain, int x, int z)
{
	terrain_ = terrain;
	section_x_ = x;
	section_z_ = z;

	off_x_ = terrain_->getConfig().xSectionSize * x;
	off_z_ = terrain_->getConfig().zSectionSize * z;

	initSection();

	layer_[0] = 0;
	layer_[1] = -1;
	layer_[2] = -1;
	layer_[3] = -1;
}

void TerrainSection::setLayer(int index, int layer)
{
	ENN_ASSERT(index < Terrain::kMaxBlendLayers);
	layer_[index] = layer;
}

int TerrainSection::getLayer(int index)
{
	ENN_ASSERT(index < Terrain::kMaxBlendLayers);
	return layer_[index];
}

void TerrainSection::updateLod()
{
	Camera* cam = g_render_pipe_->getMainCam();

	level_ = 0;

	const AxisAlignedBox& aabb = getWorldAABB();

	float d = (aabb.getMinimum() - cam->getEyePos()).squaredLength();

	bool finished = false;
	int max_level = Terrain::kMaxDetailLevel - 1;

	while (!finished && level_ < max_level)
	{
		float d0 = level_dis_sq_[level_];
		float d1 = level_dis_sq_[level_ + 1];

		if (d > d1)
		{
			level_++;
		}
		else
		{
			finished = true;
		}
	}
}

void TerrainSection::preRender()
{
	TerrainLod::_Key key;
	key.level = level_;
	key.north = level_;
	key.south = level_;
	key.west = level_;
	key.east = level_;

	int x = section_x_;
	int z = section_z_;

	int x_section_count = terrain_->getConfig().xSectionCount;
	int z_section_count = terrain_->getConfig().zSectionCount;

	if (z > 0)
	{
		TerrainSection * section = terrain_->getSection(x, z - 1);
		key.north = section->getLevel();
		if (key.north < level_)
		{
			key.north = level_;
		}
	}

	if (z < z_section_count - 1)
	{
		TerrainSection* section = terrain_->getSection(x, z + 1);
		key.south = section->getLevel();
		if (key.south < level_)
		{
			key.south = level_;
		}
	}

	if (x > 0)
	{
		TerrainSection* section = terrain_->getSection(x - 1, z);
		key.west = section->getLevel();

		if (key.west < level_)
		{
			key.west = level_;
		}
	}

	if (x < x_section_count - 1)
	{
		TerrainSection* section = terrain_->getSection(x + 1, z);
		key.east = section->getLevel();

		if (key.east < level_)
		{
			key.east = level_;
		}
	}

	if (!render_layout_)
	{
		render_layout_ = ENN_NEW D3D11InputLayout();

		render_layout_->setTopologyType(TT_TRIANGLE_STRIP);

		D3D11ResourceProxy* xy_stream = terrain_->getXYStream();

		render_layout_->bindVertexStream(xy_stream->getVertexBuffer(), MASK_P2);
		render_layout_->bindVertexStream(normal_buffer_->getVertexBuffer(), MASK_N3);
		render_layout_->bindVertexStream(height_buffer_->getVertexBuffer(), MASK_P1);
	}

	if (key != key_ || !(render_layout_->getIndexStream()))
	{
		const TerrainLod::_IndexData& data = terrain_->getTerrainLod()->getIndexData(key);

		render_layout_->bindIndexStream(data.index_buffer->getIndexBuffer(), EF_R16UI);

		key_ = key;
	}
}

void TerrainSection::notifyUnlockHeight()
{
	const IntRect& rc = terrain_->getLockedHeightRect();
	const Terrain::Config & config = terrain_->getConfig();
	int xtile = Terrain::kSectionVertexSize - 1;
	int ztile = Terrain::kSectionVertexSize - 1;

	IntRect myRect;

	myRect.left = section_x_ * xtile;
	myRect.top = section_z_ * ztile;
	myRect.right = section_x_ * xtile + xtile;
	myRect.bottom = section_z_ * ztile + ztile;

	if (rc.left > myRect.right || rc.right < myRect.left ||
		rc.top > myRect.bottom || rc.bottom < myRect.top)
		return;

	shutdown();
	initSection();
}

void TerrainSection::addRenderQueue()
{
	terrain_->visible_sections_.push_back(this);
}

void TerrainSection::renderDeferred()
{
	if (!render_layout_)
	{
		render_layout_ = ENN_NEW D3D11InputLayout();

		D3D11ResourceProxy* xy_stream = terrain_->getXYStream();

		render_layout_->bindVertexStream(xy_stream->getVertexBuffer(), MASK_P2);
		render_layout_->bindVertexStream(normal_buffer_->getVertexBuffer(), MASK_N3);
		render_layout_->bindVertexStream(height_buffer_->getVertexBuffer(), MASK_P1);
	}


	int layer0 = layer_[0];
	int layer1 = layer_[1];
	int layer2 = layer_[2];
	int layer3 = layer_[3];

	EffectMethod em;

	if (layer3 >= 0)
	{
		em.enableDETAIL_LEVEL4();
	}
	else if (layer2 >= 0)
	{
		em.enableDETAIL_LEVEL3();
	}
	else if (layer1 >= 0)
	{
		em.enableDETAIL_LEVEL2();
	}
	else
	{
		em.enableDETAIL_LEVEL1();
	}

	TerrainSectionEffectTemplate* terr_tpl = (TerrainSectionEffectTemplate*)(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_TERRAIN));
	EffectContext* eff_ctx = terr_tpl->getEffectContextDeferred(em);

	TerrainSectionEffectParams params;
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.is_deferred_ = true;

	eff_ctx->owner_->doSetParams(eff_ctx, 0, &params);
	g_render_device_->doRender(*render_layout_, eff_ctx->eff_);
}

void TerrainSection::render()
{
	preRender();


	int layer0 = layer_[0];
	int layer1 = layer_[1];
	int layer2 = layer_[2];
	int layer3 = layer_[3];

	EffectMethod em;

	if (layer3 >= 0)
	{
		em.enableDETAIL_LEVEL4();
	}
	else if (layer2 >= 0)
	{
		em.enableDETAIL_LEVEL3();
	}
	else if (layer1 >= 0)
	{
		em.enableDETAIL_LEVEL2();
	}
	else if (layer0 >= 0)
	{
		em.enableDETAIL_LEVEL1();
	}
	
	
	TerrainSectionEffectTemplate* terr_tpl = (TerrainSectionEffectTemplate*)(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_TERRAIN));
	EffectContext* eff_ctx = terr_tpl->getEffectContextNormal(em);

	const Terrain::Config& config = terrain_->getConfig();

	float xInvSectionSize = 1 / config.xSectionSize;
	float zInvSectionSize = 1 / config.zSectionSize;
	float xInvSize = 1 / config.xSize;
	float zInvSize = 1 / config.zSize;

	layer0 = Math::maxVal(0, layer0);
	layer1 = Math::maxVal(0, layer1);
	layer2 = Math::maxVal(0, layer2);
	layer3 = Math::maxVal(0, layer3);

	float uvScale0 = terrain_->getLayer(layer0)->scale;
	float uvScale1 = terrain_->getLayer(layer1)->scale;
	float uvScale2 = terrain_->getLayer(layer2)->scale;
	float uvScale3 = terrain_->getLayer(layer3)->scale;

	TerrainSectionEffectParams params;
	params.curr_render_cam_ = g_render_pipe_->getMainCam();
	params.is_deferred_ = false;
	params.em_ = em;
	params.gTransform = Vector4(off_x_, 100, off_z_, 0);
	params.gUVParam = Vector4(xInvSectionSize, zInvSectionSize, xInvSize, zInvSize);
	params.gUVScale = Vector4(uvScale0, uvScale1, uvScale2, uvScale3);
	params.section_ = this;
	params.weight_map_ = terrain_->getWeightMap(section_x_, section_z_);
	memcpy(params.layer, layer_, 4 * sizeof(int));

	eff_ctx->owner_->doSetParams(eff_ctx, 0, &params);

	g_render_device_->setRasterizerState(g_render_device_->cull_ccw_state_);
	g_render_device_->doRender(*render_layout_, eff_ctx->eff_);
}

void TerrainSection::initSection()
{
	calcuMorphBuffer();
	calcuErrorMetrics();
	calcuLevelDistance();

	int xSectionVertSize = Terrain::kSectionVertexSize;
	int zSectionVertSize = Terrain::kSectionVertexSize;
	int xVertSize = terrain_->getConfig().xVertexCount;
	int xtile = xSectionVertSize - 1;
	int ztile = zSectionVertSize - 1;

	const float* pHeights = terrain_->getHeights();
	const Vector3* pNormals = (const Vector3*)terrain_->getNormals();

	int iVertexCount = xSectionVertSize * xSectionVertSize;

	// create height vertex buffer.
	height_buffer_ = g_render_device_->createVertexBuffer(0, iVertexCount * sizeof(float), true);

	// create normal buffer
	normal_buffer_ = g_render_device_->createIndexBuffer(0, iVertexCount * sizeof(Vector3), true);

	int iSrcOffset = section_z_ * ztile * xVertSize + section_x_ * xtile;
	float* heights = (float*)g_render_device_->lock(height_buffer_, D3D11_MAP_WRITE_DISCARD);
	Vector3* normals = (Vector3*)g_render_device_->lock(normal_buffer_, D3D11_MAP_WRITE_DISCARD);
	{
		pHeights += iSrcOffset;
		pNormals += iSrcOffset;

		for (int i = 0; i < zSectionVertSize; ++i)
		{
			for (int j = 0; j < xSectionVertSize; ++j)
			{
				heights[j] = pHeights[j];
				normals[j] = pNormals[j];
			}

			heights += xSectionVertSize;
			normals += xSectionVertSize;

			pHeights += xVertSize;
			pNormals += xVertSize;
		}

		g_render_device_->unLock(height_buffer_);
		g_render_device_->unLock(normal_buffer_);
	}

	level_ = 0;
	key_.level = 0;
	key_.north = 0;
	key_.south = 0;
	key_.west = 0;
	key_.east = 0;

	// calculate bound
	pHeights = terrain_->getHeights() + iSrcOffset;

	object_aabb_.setFinite();

	object_aabb_.getMinimum().x = 0;
	object_aabb_.getMinimum().z = 0;

	object_aabb_.getMaximum().x = terrain_->getConfig().xSectionSize;
	object_aabb_.getMaximum().z = terrain_->getConfig().zSectionSize;

	for (int j = 0; j < zSectionVertSize; ++j)
	{
		for (int i = 0; i < xSectionVertSize; ++i)
		{
			object_aabb_.getMinimum().y = Math::minVal(object_aabb_.getMinimum().y, pHeights[i]);
			object_aabb_.getMaximum().y = Math::maxVal(object_aabb_.getMaximum().y, pHeights[i]);
		}

		pHeights += xVertSize;
	}

	onObjectAABBDirty();

	setPosition(off_x_, 0, off_z_);
}

void TerrainSection::shutdown()
{

}

void TerrainSection::calcuMorphBuffer()
{
	if (terrain_->getConfig().morphEnable)
	{
		int size = Terrain::kSectionVertexSize * Terrain::kSectionVertexSize * sizeof(float);
		for (int i = 0; i < Terrain::kMaxDetailLevel - 1; ++i)
		{

		}
	}
}

void TerrainSection::_calcuMorphBuffer(int level)
{

}

void TerrainSection::calcuErrorMetrics()
{
	error_metric_[0] = 0;

	for (int i = 1; i < Terrain::kMaxDetailLevel; ++i)
	{
		error_metric_[i] = _calcuErrorMetrics(i);
	}

	for (int i = 2; i < Terrain::kMaxDetailLevel; ++i)
	{
		error_metric_[i] = Math::maxVal(error_metric_[i], error_metric_[i - 1]);
	}
}

float TerrainSection::_calcuErrorMetrics(int level)
{
	int step = 1 << level;

	int xSectionVertSize = Terrain::kSectionVertexSize;
	int ySectionVertSize = Terrain::kSectionVertexSize;

	int xOffset = section_x_ * (xSectionVertSize - 1);
	int yOffset = section_z_ * (ySectionVertSize - 1);

	int xSides = (xSectionVertSize - 1) >> level;
	int ySides = (ySectionVertSize - 1) >> level;

	float err = 0.0f;

	for (int y = 0; y < ySectionVertSize; y += step)
	{
		for (int x = 0; x < xSides; ++x)
		{
			int x0 = x * step;
			int x1 = x0 + step;
			int xm = (x1 + x0) / 2;

			float h0 = terrain_->getHeight(x0 + xOffset, y + yOffset);
			float h1 = terrain_->getHeight(x1 + xOffset, y + yOffset);
			float hm = terrain_->getHeight(xm + xOffset, y + yOffset);
			float hmi = (h0 + h1) / 2;

			float delta = Math::fabs(hm - hmi);
			err = Math::maxVal(err, delta);
		}
	}

	for (int x = 0; x < xSectionVertSize; x += step)
	{
		for (int y = 0; y < ySides; ++y)
		{
			int y0 = y * step;
			int y1 = y0 + step;

			int ym = (y0 + y1) / 2;

			float h0 = terrain_->getHeight(x + xOffset, y0 + yOffset);
			float h1 = terrain_->getHeight(x + xOffset, y1 + yOffset);
			float hm = terrain_->getHeight(x + xOffset, ym + yOffset);
			float hmi = (h0 + h1) / 2;

			float delta = Math::fabs(hm - hmi);
			err = Math::maxVal(err, delta);
		}
	}

	for (int y = 0; y < ySides; ++y)
	{
		int y0 = y * step;
		int y1 = y0 + step;
		int ym = (y0 + y1) / 2;

		for (int x = 0; x < xSides; ++x)
		{
			int x0 = x * step;
			int x1 = x0 + step;
			int xm = (x0 + x1) / 2;

			float h0 = terrain_->getHeight(x0 + xOffset, y0 + yOffset);
			float h1 = terrain_->getHeight(x1 + xOffset, y1 + yOffset);
			float hm = terrain_->getHeight(xm + xOffset, ym + yOffset);
			float hmi = (h0 + h1) / 2;

			float delta = Math::fabs(hm - hmi);

			err = Math::maxVal(err, delta);
		}
	}

	return err;
}

void TerrainSection::calcuLevelDistance()
{
	float pixelerr = Terrain::kMaxPixelError;
	float resolution = Terrain::kMaxResolution;

	float c = 1.0f / (2 * pixelerr / resolution);

	if (section_x_ == 0 && section_z_ == 15)
	{
		int c = 1;
	}

	for (int i = 0; i < Terrain::kMaxDetailLevel; ++i)
	{
		float e = error_metric_[i];
		float d = e * c;
		level_dis_sq_[i] = d * d;
	}
}


}