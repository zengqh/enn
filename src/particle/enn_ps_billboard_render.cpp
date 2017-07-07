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
// Time: 2015/12/17
// File: enn_ps_billboard_render.cpp
//

#include "enn_ps_billboard_render.h"

namespace enn
{

PSRenderBillboard::PSRenderBillboard()
{

}

PSRenderBillboard::~PSRenderBillboard()
{

}

void PSRenderBillboard::allocQuota(int quota)
{
	billboard_.allocBuffer(quota);
}

int PSRenderBillboard::update(float elapsedTime)
{
	PSRender::update(elapsedTime);

	if (particles_.size() > 0)
	{
		AxisAlignedBox bound = AxisAlignedBox::Invalid;
		for (int i = 0; i < particles_.size; ++i)
		{
			const Particle* p = particles_[i];

			float3 vMin = p->position - p->size * 0.5f;
			float3 vMax = p->position + p->size * 0.5f;

			bound.merge(vMin);
			bound.merge(vMax);


		}
	}
}

void PSRenderBillboard::setTexture(const String& str)
{

}

const String& PSRenderBillboard::getTexture() const
{

}

void PSRenderBillboard::setBillboardType(int type)
{

}

int PSRenderBillboard::getBillboardType() const
{

}

void PSRenderBillboard::setBillboardCenter(const float2& center)
{

}

const float2& PSRenderBillboard::getBillboardCenter() const
{

}

void PSRenderBillboard::setCommonDirection(const float3& dir)
{

}

const float3& PSRenderBillboard::getCommonDirection() const
{

}

void PSRenderBillboard::setCommonUpVector(const float3& up)
{

}

const float3& PSRenderBillboard::getCommonUpVector() const
{

}

void PSRenderBillboard::setKeepAspect(bool b)
{

}

bool PSRenderBillboard::isKeepAspect() const
{

}

}