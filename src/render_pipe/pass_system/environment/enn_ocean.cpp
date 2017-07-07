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
// Time: 2015/05/22
// File: enn_ocean.cpp
//

#include "enn_ocean.h"
#include "enn_projection_grid.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
Ocean::Ocean()
{
	init();

	position_ = Vector3(0, 10, 0);
	proj_grid_ = ENN_NEW ProjectedGrid();
}

Ocean::~Ocean()
{
	
}

void Ocean::render()
{
	
}

void Ocean::setHeight(float h)
{

}

void Ocean::init()
{
	tex_wave_ = ENN_NEW D3D11TextureObj();
	tex_fresnel_ = ENN_NEW D3D11TextureObj();
	tex_normal0_ = ENN_NEW D3D11TextureObj();
	tex_normal1_ = ENN_NEW D3D11TextureObj();

	tex_wave_->loadFile("Water/waves2.dds");
	tex_fresnel_->loadFile("Water/Fresnel.bmp");
	tex_normal0_->loadFile("Water/WaterNormal1.tga");
	tex_normal1_->loadFile("Water/WaterNormal2.tga");

	initRT();
}

void Ocean::initRT()
{

}

void Ocean::renderUnderWater()
{

}

void Ocean::renderUpWater()
{

}

}