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
// Time: 2015/10/29
// File: enn_water_plane.cpp
//

#include "enn_water_plane.h"
#include "enn_render_pipe.h"
#include "enn_ocean_eff_template.h"
#include "enn_effect_template_manager.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{

WaterPlane::WaterPlane()
{
	water_type_ = Ocean;

	grid_element_size_ = 3;
	grid_size_ = 100;
	need_generate_vb_ = true;
}

WaterPlane::~WaterPlane()
{
	
}

bool WaterPlane::init()
{
	WaterObject::init();

	object_aabb_.setInfinite();

	return true;
}

void WaterPlane::render()
{
	setupVBIB();

	OceanEffectTemplate* ocean_tpl = (OceanEffectTemplate*)(g_eff_template_mgr_->getEffectTemplate(GeneralRender::GR_OCEAN));

	EffectMethod em;

	Camera* cam = g_render_pipe_->getMainCam();

	if (isUnderWater(cam->getEyePos()))
	{
		em.enableUNDERWATER();
	}

	EffectContext* eff_ctx = ocean_tpl->getEffectContext(em);

	OceanEffectParams params;
	params.curr_render_cam_ = cam;
	params.water_obj_ = this;

	ocean_tpl->doSetParams(eff_ctx, 0, &params);

	g_render_device_->setRasterizerState(g_render_device_->cull_ccw_state_);
	g_render_device_->doRender(*render_layout_, eff_ctx->eff_);
	g_render_device_->setSolidState(true);
}

const AxisAlignedBox& WaterPlane::getObjectAABB() const
{
	return object_aabb_;
}

void WaterPlane::setupVBIB()
{
	if (!need_generate_vb_)
	{
		return;
	}

	float block_width = grid_element_size_ * (grid_size_ + 1);
	float block_height = grid_element_size_ * (grid_size_ + 1);
	float xleft = -block_width * 0.5f;
	float zleft = -block_height * 0.5f;
	Vector3 left_pos(xleft, 0, zleft);
	int gridStride = grid_size_ + 1;

	// Number of verts surrounding the grid, projected by the frustum.
	const uint32 borderVertCount = grid_size_ * 4;

	// Number of verts in the front-most row which are raised to the horizon.
	const uint32 horizonVertCount = grid_size_ + 1;

	Array<WaterVertex> wv_buffer;
	wv_buffer.reserve(gridStride * gridStride + borderVertCount + horizonVertCount);

	for (int i = grid_size_; i >=0; --i)
	{
		for (int j = 0; j < gridStride; ++j)
		{
			WaterVertex wv;
			wv.vertex = Vector3(j * grid_element_size_, 0, i * grid_element_size_) + left_pos;
			wv_buffer.push_back(wv);
		}
	}

	// border
	const float2 sBorderTangentVec[4] = {
		float2(1, 0), // Front ( 0 )
		float2(0, 1), // Right ( 1 )
		float2(-1, 0), // Back  ( 2 )
		float2(0, -1)  // Left  ( 3 )
	};

	const float2 sBorderStartPos[4] = {
		float2(-1, -1), // Front ( 0 )
		float2(1, -1), // Right ( 1 )
		float2(1, 1), // Back  ( 2 )
		float2(-1, 1)  // Left  ( 3 )
	};

	// Diagram of Start vert position per Side.
	//
	// Labeling convention for verts is 'As' where A is the first letter of 
	// that side's descriptive name and lower-case s indicates 'start'. 
	// 
	// 
	//
	//          Front
	//          (-1,-1)
	//          Fs------o-----Rs(1,-1)R
	//          |             |      i
	//          |             |      g
	//          o     (0,0)   o      h
	//          |             |      t
	//          |             |     
	//  L(1,1)Ls------o-----Bs
	//  e                     (1,1)   
	//  f                     Back
	//  t

	// Calculate the world-space dimension of the border-vert-ring...

	// Diagram shows overall layout of the WaterPlane with a gridSize of 1,
	// with all 'quads' enumerated.
	// center-grid ( 1 ), border ( 2, 3, 4, 5 ), and horizon ( 6 ).
	//
	//
	//   x------------x
	//    \      6     \    <- horizon quad is really 'above' the front border
	//      x ---------  x     not in front of it
	//      | \    2   / |
	//      |   x---- x  |
	//      |   |     |  |
	//      |  5|  1  |3 |
	//      |   x --- x  |
	//      | /    4    \|
	//      x------------x


	// WaterPlane renders relative to the camera rotation around z and xy position.
	//
	// That is, it rotates around the z-up axis with the camera such that the
	// camera is always facing towards the front border unless looking straight
	// down or up.
	//
	// Also note that the horizon verts are pulled straight up from the front
	// border verts.
	//
	// Therefore...
	//
	// The front border must be as close to the farclip plane as possible 
	// so distant objects clip through the horizon and  farplane at the same time.
	// 
	// The left and right borders must be pulled outward a distance such
	// that water extends horizontally across the entire viewable area while
	// looking straight forward +y or straight down -z.
	//

	// 
	Camera* main_cam = g_render_pipe_->getMainCam();
	const float farDistScale = 0.99f;
	float farDist = main_cam->getZFar() * farDistScale;

	float farWidth = farDist * 3;

	float2 borderExtents(farWidth * 2, farDist * 2);
	float2 borderHalfExtents(farWidth, farDist);

	float2 borderDir;
	float2 borderStart;

	for (int i = 0; i < 4; i++)
	{
		borderDir = sBorderTangentVec[i];
		borderStart = sBorderStartPos[i];

		for (int j = 0; j < grid_size_; j++)
		{
			float frac = (float)j / (float)grid_size_;

			float2 pos(borderStart * borderHalfExtents);
			pos += borderDir * borderExtents * frac;

			WaterVertex wv;
			wv.vertex = Vector3(pos.x, 0, pos.y);

			wv_buffer.push_back(wv);
		}
	}

	// Fill in row of horizion verts.
	// Verts are positioned identical to the front border, but will be
	// manipulated in z within the shader.
	//
	// Z position of 50.0f is unimportant unless you want to disable
	// shader manipulation and render in wireframe for debugging.

	for (int i = 0; i < gridStride; i++)
	{
		float frac = i / (float)grid_size_;

		float2 pos(sBorderStartPos[0] * borderHalfExtents);
		pos += sBorderTangentVec[0] * borderExtents * frac;

		WaterVertex wv;
		wv.vertex.setValue(pos.x, 50.0f, pos.y);
		wv_buffer.push_back(wv);
	}


	vb_ = g_render_device_->createVertexBuffer(&wv_buffer[0], wv_buffer.size() * sizeof(WaterVertex), false);

	// setup ib
	// Fill in the PrimitiveBuffer...

	// 2 triangles per cell/quad   
	const uint32 gridTriCount = grid_size_ * grid_size_ * 2;

	// 4 sides, mGridSize quads per side, 2 triangles per quad
	const uint32 borderTriCount = 4 * grid_size_ * 2;

	// 1 quad per gridSize, 2 triangles per quad
	// i.e. an extra row of 'cells' leading the front side of the grid
	const uint32 horizonTriCount = grid_size_ * 2;

	uint32 PrimCount = gridTriCount + borderTriCount + horizonTriCount;

	uint32 IndxCount = PrimCount * 3;

	Array<uint16> indices_buffer;
	indices_buffer.resize(IndxCount);
	uint16* idxPtr = &indices_buffer[0];

	// Temporaries to hold indices for the corner points of a quad.
	uint32 p00, p01, p11, p10;
	uint32 offset = 0;

	// Given a single cell of the grid diagramed below,
	// quad indice variables are in this orientation.
	//
	// p01 --- p11
	//  |       |
	//  |       |
	// p00 --- p10
	//
	//   Positive Y points UP the diagram ( p00, p01 ).
	//   Positive X points RIGHT across the diagram ( p00, p10 )
	//

	// i iterates bottom to top "column-wise"
	for (int i = 0; i < grid_size_; i++)
	{
		// j iterates left to right "row-wise"
		for (int j = 0; j < grid_size_; j++)
		{
			// where (j,i) is a particular cell.
			p00 = offset;
			p10 = offset + 1;
			p01 = offset + gridStride;
			p11 = offset + 1 + gridStride;

			// Top Left Triangle

			*idxPtr = p00;
			idxPtr++;
			*idxPtr = p01;
			idxPtr++;
			*idxPtr = p11;
			idxPtr++;

			// Bottom Right Triangle

			*idxPtr = p00;
			idxPtr++;
			*idxPtr = p11;
			idxPtr++;
			*idxPtr = p10;
			idxPtr++;

			offset += 1;
		}

		offset += 1;
	}

	// Fill border indices...

	// Given a grid size of 1, 
	// the grid / border verts are in the vertex buffer in this order.
	//
	//
	// 4           5  
	//    2 --- 3
	//    |     |
	//    |     |
	//    0 --- 1
	// 7           6
	//
	//   Positive Y points UP the diagram ( p00, p01 ).
	//   Positive X points RIGHT across the diagram ( p00, p10 )
	//
	//  Note we duplicate the first border vert ( 4 ) since it is also the last
	//  and this makes our loop easier.

	const uint32 sBorderStartVert[4] = {
		gridStride * grid_size_,              // Index to the Top-Left grid vert.
		gridStride * grid_size_ + grid_size_,   // Index to the Top-Right grid vert.
		grid_size_,                           // Index to the Bottom-Right grid vert.
		0,                                  // Index to the Bottom-Left grid vert.
	};

	const int sBorderStepSize[4] = {
		// Step size to the next grid vert along the specified side....
		1,             // Top
		-(int)gridStride,   // Right
		-1,             // Bottom
		gridStride,    // Left
	};

	const uint32 firstBorderVert = gridStride * grid_size_ + gridStride;
	const uint32 lastBorderVert = firstBorderVert + (borderVertCount - 1);
	uint32 startBorderVert = firstBorderVert;
	uint32 startGridVert;
	uint32 curStepSize;

	for (uint32 i = 0; i < 4; i++)
	{
		startGridVert = sBorderStartVert[i];
		curStepSize = sBorderStepSize[i];

		for (uint32 j = 0; j < grid_size_; j++)
		{
			// Each border cell is 1 quad, 2 triangles.

			p00 = startGridVert;
			p10 = startGridVert + curStepSize;
			p01 = startBorderVert;
			p11 = startBorderVert + 1;

			if (p11 > lastBorderVert)
				p11 = firstBorderVert;

			// Top Left Triangle

			*idxPtr = p00;
			idxPtr++;
			*idxPtr = p01;
			idxPtr++;
			*idxPtr = p11;
			idxPtr++;

			// Bottom Right Triangle

			*idxPtr = p00;
			idxPtr++;
			*idxPtr = p11;
			idxPtr++;
			*idxPtr = p10;
			idxPtr++;

			startBorderVert++;
			startGridVert += curStepSize;
		}
	}

	// Fill in 'horizon' triangles.

	uint32 curHorizonVert = lastBorderVert + 1;
	uint32 curBorderVert = firstBorderVert;

	for (uint32 i = 0; i < grid_size_; i++)
	{
		p00 = curBorderVert;
		p10 = curBorderVert + 1;
		p01 = curHorizonVert;
		p11 = curHorizonVert + 1;

		// Top Left Triangle

		*idxPtr = p00;
		idxPtr++;
		*idxPtr = p01;
		idxPtr++;
		*idxPtr = p11;
		idxPtr++;

		// Bottom Right Triangle

		*idxPtr = p00;
		idxPtr++;
		*idxPtr = p11;
		idxPtr++;
		*idxPtr = p10;
		idxPtr++;

		curBorderVert++;
		curHorizonVert++;
	}

	ib_ = g_render_device_->createIndexBuffer(&indices_buffer[0], indices_buffer.size() * sizeof(uint16), false);

	// create render layout
	render_layout_ = ENN_NEW D3D11InputLayout();

	render_layout_->setTopologyType(TT_TRIANGLE_LIST);

	render_layout_->bindVertexStream(vb_->getVertexBuffer(), MASK_P3);
	render_layout_->bindIndexStream(ib_->getIndexBuffer(), EF_R16UI);

	need_generate_vb_ = false;
}

}