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
// File: enn_ps_billboard_render.h
//

#pragma once

#include "enn_ps_render.h"

namespace enn
{

class PSRenderBillboard : public PSRender
{
public:
	PSRenderBillboard();
	virtual ~PSRenderBillboard();

	virtual void allocQuota(int quota);
	virtual int update(float elapsedTime);

	void setTexture(const String& str);
	const String& getTexture() const;

	void setBillboardType(int type);
	int getBillboardType() const;

	void setBillboardCenter(const float2& center);
	const float2& getBillboardCenter() const;

	void setCommonDirection(const float3& dir);
	const float3& getCommonDirection() const;

	void setCommonUpVector(const float3& up);
	const float3& getCommonUpVector() const;

	void setKeepAspect(bool b);
	bool isKeepAspect() const;

protected:
	PSBillboard				billboard_;
	D3D11TextureObj*		texture_;
};

}