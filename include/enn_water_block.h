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
// File: enn_water_block.h
//

#pragma once

#include "enn_water_object.h"

namespace enn
{

class WaterBlock : public WaterObject
{
public:
	WaterBlock();
	virtual ~WaterBlock();

	virtual bool init();

	virtual void render();
	virtual const AxisAlignedBox& getObjectAABB() const;

	void setWaterBound(float ww, float wh, float grid_size);

protected:
	virtual void setupVBIB();

protected:
	float						grid_element_size_;
	uint32						width_;
	uint32						height_;
	float						elapsed_time_;

	D3D11ResourceProxy*			vb_;
	D3D11ResourceProxy*			ib_;
	D3D11InputLayout*			render_layout_;

	AxisAlignedBox				object_aabb_;
};

}