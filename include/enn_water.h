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
// Time: 2015/01/18
// File: enn_water.h
//

#pragma once

#include "enn_terrain.h"

namespace enn
{

class WaterBlockIn : public AllocatedObject
{
	friend class Water;

public:
	WaterBlockIn();
	virtual ~WaterBlockIn();

	void init(Water* water, int x, int z);
	void shutdown();

protected:
	AxisAlignedBox						bound_;
	int									index_x_;
	int									index_z_;
	Water*								water_;

	D3D11ResourceProxy*					vb_;
	D3D11ResourceProxy*					ib_;
};

class Water : public AllocatedObject
{
	static const int K_Magic = 'wtr0';

public:
	Water();
	~Water();

	void setHeight(float height);
	float getHeight() const { return height_; }

	void init();
	void shutdown();

	void resize(int w, int h);

	void load(const char* source);
	void save(const char* source);

	char getData(int i, int j);
	void setData(int i, int j, char v);

	void mapCoord(int& _x, int& _z, float x, float z);
	bool hasWater(float x, float z);

	void notifyChange(const IntRect& rc);

	int GetSizeX() { return size_x_; }
	int GetSizeZ() { return size_z_; }
	int GetBlockCountX() { return block_count_x_; }
	int GetBlockCountZ() { return block_count_z_; }
	float GetGridSize() { return grid_size_; }

	void render();

protected:
	void allocBlock();
	void initBlock();
	void initRT();

	void preRender();
	void renderUnderWater();
	void renderUpWater();
	void renderReflection();

protected:
	float				height_;
	int					size_x_;
	int					size_z_;
	char*				data_;
	float				grid_size_;
	int					block_count_x_;
	int					block_count_z_;
	WaterBlockIn*		blocks_;
	D3D11TextureObj*	tex_wave_;
	D3D11TextureObj*	tex_fresnel_;
	D3D11TextureObj*	tex_normal0_;
	D3D11TextureObj*	tex_normal1_;


};

}
