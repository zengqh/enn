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
// Time: 2014/07/06
// File: enn_d3d11_sprite_render.h

#pragma once

#include "enn_mesh.h"
#include "enn_render_parameter.h"

namespace enn
{

#define MASK_SPRITE (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_TEXCOORD, 2, EF_ABGR32F))))

template <class TInstance> class SpriteGeometry : public SubMesh
{
	typedef typename vector<TInstance>::type		InstanceList;

public:
	SpriteGeometry();
	virtual ~SpriteGeometry();

	void addInstance(const TInstance& inst);
	uint32 getInstanceCount() const;
	uint32 getInstanceListSize() const { return inst_list_.size() * sizeof(TInstance); }


	void makeGPUBuffer();
	D3D11InputLayout* getRenderLayout();

	void resetInst();

	D3D11VertexBuffer* getInstBuffer() const { return inst_buffer_->getResourceT<D3D11VertexBuffer>(); }
protected:
	InstanceList			inst_list_;

	D3D11ResourceProxy*		inst_buffer_;

	uint32					inst_buffer_size_;
};


//////////////////////////////////////////////////////////////////////////
// IMPL
template <class TInstance> SpriteGeometry<TInstance>::SpriteGeometry() : SubMesh(0), inst_buffer_(0)
{
	inst_buffer_size_ = 0;
	
}

template <class TInstance> SpriteGeometry<TInstance>::~SpriteGeometry()
{
	ENN_SAFE_DELETE(inst_buffer_);
}

template <class TInstance>
void SpriteGeometry<TInstance>::addInstance(const TInstance& inst)
{
	inst_list_.push_back(inst);
}

template <class TInstance>
uint32 SpriteGeometry<TInstance>::getInstanceCount() const
{
	return inst_list_.size();
}

template <class TInstance>
void SpriteGeometry<TInstance>::makeGPUBuffer()
{
	SubMesh::makeGPUBuffer();

	if (inst_buffer_size_ < getInstanceListSize())
	{
		ENN_SAFE_DELETE(inst_buffer_);
	}

	if (!inst_buffer_)
	{
		D3D11BufferDesc desc;

		inst_buffer_size_ = inst_list_.size() * sizeof(TInstance);

		desc.setDefaultVertexBuffer(inst_buffer_size_, true);
		D3D11_SUBRESOURCE_DATA data;

		data.pSysMem = &inst_list_[0];
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		inst_buffer_ = g_render_device_->createVertexBuffer(desc, &data);
		return;
	}

	D3D11_MAPPED_SUBRESOURCE resource = g_render_device_->mapResource(inst_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0);

	memcpy(resource.pData, &inst_list_[0], getInstanceListSize());

	g_render_device_->unMapResource(inst_buffer_, 0);
}

template <class TInstance>
D3D11InputLayout* SpriteGeometry<TInstance>::getRenderLayout()
{
	if (render_layout_)
	{
		render_layout_->setInstanceStream(getInstBuffer());
		return render_layout_;
	}

	render_layout_ = ENN_NEW D3D11InputLayout();

	render_layout_->bindVertexStream(getVB(), vb_type_);
	render_layout_->bindIndexStream(getIB(), ib_type_ == IDX_16 ? EF_R16UI : EF_R32UI);
	render_layout_->bindVertexStream(getInstBuffer(), MASK_SPRITE, ST_INSTANCE, 1);

	return render_layout_;
}

template <class TInstance>
void SpriteGeometry<TInstance>::resetInst()
{
	inst_list_.clear();
}

//////////////////////////////////////////////////////////////////////////
class SpriteRenderer : public AllocatedObject
{
public:
	struct SpriteDrawRect
	{
		float X;
		float Y;
		float width;
		float height;
	};

	struct InstanceData
	{
		SpriteDrawRect draw_rect;
	};

	struct VertexData
	{
		Vector2 position;
		Vector2 texCoord;
	};

private:
	struct TexViewportInfo
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Vector4 TexAndViewportSize;
	};

public:
	SpriteRenderer();
	virtual ~SpriteRenderer();

	bool init();
	bool shutdown();

	void render(D3D11ResourceProxy* tex, const InstanceData* inst_data, uint32 sprite_nums);

protected:
	SpriteGeometry<InstanceData>		sprite_geometry_;
	D3D11Effect*						sprite_eff_;

	D3D11ResourceProxy*					tex_vp_cbuffer_;
	TexViewportInfo						tex_vp_info_;
};

extern SpriteRenderer*		g_sprite_renderer_;

}