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
// File: enn_terrain_section.h
//

#pragma once
#include "enn_terrain.h"
#include "enn_scene_node.h"
#include "enn_terrain_lod.h"

namespace enn
{

class TerrainSection : public SceneNode
{
	ENN_DECLARE_RTTI(TerrainSection);

	friend class Terrain;

public:
	TerrainSection();
	~TerrainSection();

	void create(Terrain* terrain, int x, int z);

	Terrain* getOwner() const { return terrain_; }

	int getLevel() const { return level_; }
	int getSectionX() const { return section_x_; }
	int getSectionZ() const { return section_z_; }
	void setLayer(int index, int layer);
	int getLayer(int index);
	float getOffX() const { return off_x_; }
	float getOffZ() const { return off_z_; }

	void updateLod();
	void preRender();
	void notifyUnlockHeight();

	void addRenderQueue();

	void renderDeferred();
	void render();

protected:
	void initSection();
	void shutdown();

	void calcuMorphBuffer();
	void _calcuMorphBuffer(int level);
	void calcuErrorMetrics();
	float _calcuErrorMetrics(int level);

	void calcuLevelDistance();

protected:
	Terrain*						terrain_;
	int								section_x_, section_z_;
	float							off_x_, off_z_;

	int								layer_[Terrain::kMaxBlendLayers];
	int								level_;
	float							error_metric_[Terrain::kMaxDetailLevel];
	float							level_dis_sq_[Terrain::kMaxDetailLevel];
	D3D11ResourceProxy*				morph_buffer_[Terrain::kMaxDetailLevel];

	float							morph_;
	TerrainLod::_Key				key_;

	D3D11ResourceProxy*				height_buffer_;
	D3D11ResourceProxy*				normal_buffer_;

	D3D11InputLayout*				render_layout_;
};

}