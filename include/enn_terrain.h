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
// Time: 2015/01/05
// File: enn_terrain.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

class TerrainLod;
class Terrain : public AllocatedObject
{
	friend class TerrainSection;

public:
	enum Consts
	{
		kMaxLayers = 32,
		kMaxBlendLayers = 4,
		kSectionVertexSize = 65,
		kSectionVertexSize_2 = kSectionVertexSize * kSectionVertexSize,
		kMaxDetailLevel = 4,
		kWeightMapSize = 256,
		kMaxPixelError = 4,
		kMaxResolution = 768
	};

	struct Layer
	{
		char detail[128];
		char normal[128];
		char specular[128];
		float scale;
		int material;

		Layer() : scale(100), material(-1)
		{
			detail[0] = 0;
			normal[0] = 0;
			specular[0] = 0;
		}
	};

	struct Config
	{
		float xSize, zSize;
		int xVertexCount, zVertexCount;
		int iVertexCount;
		bool morphEnable;
		float morphStart;
		float xSectionSize, zSectionSize;
		int xSectionCount, zSectionCount, iSectionCount;
			
		int xWeightMapSize, zWeightMapSize, iWeightMapSize;

		int bitFlags;

		Config()
		{
			xSize = zSize = 1024;

			xVertexCount = zVertexCount = 1025;

			iVertexCount = 0;

			morphEnable = false;
			morphStart = 0;

			xSectionCount = zSectionCount = 0;
			xSectionSize = zSectionSize = 0;
			iSectionCount = 0;

			xWeightMapSize = 0;
			zWeightMapSize = 0;
		}
	};

public:
	Terrain(const Config& config);
	Terrain(const char* name);
	virtual ~Terrain();

	void save(const char* filename);
	int addLayer(const Layer& layer);
	const Layer* getLayer(int index);
	void setLayer(int index, const Layer& layer);
	void removeLayer(int layer);

	const Config& getConfig() const;
	TerrainLod* getTerrainLod();
	TerrainSection* getSection(int x, int z);
	Vector3 getPosition(int x, int z);
	float getHeight(int x, int z);
	Vector3 getNormal(int x, int z);
	Color2 getWeight(int x, int z);
	D3D11TextureObj* getWeightMap(int x, int z);

	const float* getHeights() const;
	const Vector3* getNormals() const;

	void render();
	void renderDeferred();
	void renderInMirror();

	int getVisibleSectionCount() const;
	TerrainSection* getVisibleSection(int i);

	float getHeight(float x, float z);
	Vector3 getNormal(float x, float z);

	// use in editor
	float* lockHeight(const IntRect& rect);
	void unlockHeight();

	bool isLockedHeight();
	const IntRect& getLockedHeightRect();
	const float* getLockedHeightData();

	float* lockWeightMap(const IntRect& rect);
	void unlockWeightMap(int layer);
	bool isLockedWeightMap();
	const IntRect& getLockedWeightMapRect();
	const float* getLockedWeightMapData();

	D3D11TextureObj* getDetailMap(int layer);
	D3D11TextureObj* getNormalMap(int layer);
	D3D11TextureObj* getSpecularMap(int layer);

	Vector3 _getPosition(int x, int z);
	float _getHeight(int x, int z);
	Vector3 _getNormal(int x, int z);

	D3D11ResourceProxy* getXYStream() const { return xy_stream_;  }

	void clearVisibleSections() { visible_sections_.clear(); }

protected:
	void create(const Config& config);
	void load(const char* filename);
	void init();
	void calcuNormals();
	Vector3 toFloatNormal(const Color2& clr);

protected:
	Config						config_;
	bool						inited_;
	Array<TerrainSection*>		sections_;
	Array<TerrainSection*>		visible_sections_;
	Array<D3D11TextureObj*>		weight_maps_;

	TerrainLod*					lod_;
	float*						heights_;
	Vector3*					normals_;
	Color2*						weights_;

	AxisAlignedBox				bound_;

	D3D11TextureObj*			default_detail_map_;
	D3D11TextureObj*			default_normal_map_;
	D3D11TextureObj*			default_specular_map_;

	Layer						layers_[kMaxLayers];
	D3D11TextureObj*			detail_maps_[kMaxLayers];
	D3D11TextureObj*			normal_maps_[kMaxLayers];
	D3D11TextureObj*			specular_maps_[kMaxLayers];

	// editor
	IntRect						locked_rect_;
	float*						locked_data_;

	IntRect						locked_weight_map_rect_;
	float*						locked_weight_map_data_;

	D3D11ResourceProxy*			xy_stream_;
};

}