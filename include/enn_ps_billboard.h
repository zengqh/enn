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
// File: enn_ps_billboard.h
//

#pragma once

namespace enn
{

class PSBillboard : public AllocatedObject
{
public:
	PSBillboard();
	virtual ~PSBillboard();

	virtual String getType() const { return "billboard_render" }

	virtual void getWorldPosition(float3& pos);
	virtual void getWorldBound(AxisAlignedBox& bound);
	virtual void getWorldTM(Matrix4& form);
	void allocBuffer(int quota);

	void updateBuffer(const Quaternion& cam_rot);
	void getBillboardXYAxis(float3& xAxis, float3& yAxis, const Particle* p);
	void update();

protected:
	PSRenderBillboard*			parent_;
	float3						cam_pos_, cam_x_axis_, cam_y_axis_, cam_z_axis_;
	float3						common_dir_, common_up_;

	D3D11ResourceProxy*				idx_buffer_;
	D3D11ResourceProxy*				vert_buffer_;

	D3D11InputLayout*				render_layout_;
};

}