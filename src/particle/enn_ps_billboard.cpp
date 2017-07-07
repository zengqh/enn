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
// File: enn_ps_billboard.cpp
//

#include "enn_ps_billboard.h"

namespace enn
{

PSBillboard::PSBillboard()
{
	
}

PSBillboard::~PSBillboard()
{

}

void PSBillboard::getWorldPosition(float3& pos)
{
	if (parent_->isLocalSpace())
	{
		pos = parent_->getParent()->
	}
}

void PSBillboard::getWorldBound(AxisAlignedBox& bound)
{

}

void PSBillboard::getWorldTM(Matrix4& form)
{

}

void PSBillboard::allocBuffer(int quota)
{
	vert_buffer_ = g_render_device_->createVertexBuffer(0, quota * sizeof(PS_Vertex) * 4, true);
	

	uint16* idx = ENN_MALLOC_ARRAY_T(uint16, quota * 6);

	for (int i = 0; i < quota; ++i)
	{
		int index = i * 4;

		*idx++ = index + 0;
		*idx++ = index + 1;
		*idx++ = index + 2;

		*idx++ = index + 2;
		*idx++ = index + 1;
		*idx++ = index + 3;
	}

	idx_buffer_ = g_render_device_->createIndexBuffer(idx, quota * 6);

	ENN_SAFE_FREE(idx);
}

void PSBillboard::updateBuffer(const Quaternion& cam_rot)
{
	common_dir_ = parent_->getCommonDirection();
	common_up_ = parent_->getCommonUpVector();


	cam_rot.toAxes(cam_x_axis_, cam_y_axis_, cam_z_axis_);

	if (parent_->getBillboardType() == PS_BillboardType::PERPENDICULAR ||
		parent_->getBillboardType() == PS_BillboardType::PERPENDICULAR_COMMON)
	{
		common_dir_ = parent_->getParent()->getDerivedMatrix().transformAffineNormal(common_dir_);
		common_up_ = parent_->getParent()->getDerivedMatrix().transformAffineNormal(common_up_);
	}

	D3D11_MAPPED_SUBRESOURCE resource = g_render_device_->mapResource(vert_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0);

	PS_Vertex* v = (PS_Vertex*)resource.pData;

	for (int i = 0; i < count; ++i)
	{
		const Particle * p = parent_->getTechnique()->getParticle(i);

		float width = Math::maxVal(0.0f, p->size.x);
		float height = Math::maxVal(0.0f, p->size.y);

		float3 xAxis, yAxis;
		getBillboardXYAxis(xAxis, yAxis, p);

		xAxis *= width, yAxis *= height;
		float3 position = p->position;

		v->position = position - xAxis * center.x + yAxis * center.y;
		v->color = p->Color;
		v->uv.x = p->uvrect.x1, v->uv.y = p->uvrect.y1;
		++v;

		v->position = position + xAxis * (1 - center.x) + yAxis * center.y;
		v->color = p->Color;
		v->uv.x = p->uvrect.x2, v->uv.y = p->uvrect.y1;
		++v;

		v->position = position - xAxis * center.x - yAxis * (1 - center.y);
		v->color = p->Color;
		v->uv.x = p->uvrect.x1, v->uv.y = p->uvrect.y2;
		++v;

		v->position = position + xAxis * (1 - center.x) - yAxis * (1 - center.y);
		v->color = p->Color;
		v->uv.x = p->uvrect.x2, v->uv.y = p->uvrect.y2;
		++v;
	}

	g_render_device_->unMapResource(vert_buffer_, 0);
}

void PSBillboard::getBillboardXYAxis(float3& xAxis, float3& yAxis, const Particle* p)
{
	int billboard_type = parent_->getBillboardType();

	float3 dir = p->direction;

	switch (billboard_type)
	{

	case PS_BillboardType::POINT:

		// Standard point billboard (default), always faces the camera completely and is always upright
		xAxis = cam_x_axis_;
		yAxis = cam_y_axis_;
		break;

	case PS_BillboardType::ORIENTED:
		// Billboards are oriented around their own direction vector (their own Y axis) and only rotate around this to face the camera
		yAxis = dir;
		xAxis = cam_z_axis_.crossProduct(yAxis);
		break;

	case PS_BillboardType::ORIENTED_COMMON:

		// Billboards are oriented around a shared direction vector(used as Y axis) and only rotate around this to face the camera
		yAxis = common_dir_;
		xAxis = cam_z_axis_.crossProduct(yAxis);

		break;

	case PS_BillboardType::PERPENDICULAR:

		// Billboards are perpendicular to their own direction vector (their own Z axis, the facing direction) and X, Y axis are determined by a shared up-vertor
		xAxis = common_up_.crossProduct(dir);
		xAxis.normalise();

		yAxis = dir.crossProduct(xAxis);
		break;

	case PS_BillboardType::PERPENDICULAR_COMMON:

		// Billboards are perpendicular to a shared direction vector (used as Z axis, the facing direction) and X, Y axis are determined by a shared up-vertor
		xAxis = common_up_.crossProduct(common_dir_);
		yAxis = common_dir_;
		break;

	default:
		break;
	}
}

void PSBillboard::update()
{

}

}