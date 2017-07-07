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
// Time: 2014/02/16
// File: enn_mesh.cpp
//

#include "enn_mesh.h"
#include "enn_d3d11_render_device.h"
#include "enn_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
SubMesh::SubMesh(Mesh* owner)
: vb_(0)
, ib_(0)
, use_memory_(0)
, prim_type_(TT_TRIANGLE_LIST)
, vb_type_(0)
, ib_type_(IDX_16)
, vb_count_(0)
, ib_count_(0)
, stride_(0)
, vertex_data_(0)
, index_data_(0)
, owner_(owner)
, render_layout_(0)
{

}

SubMesh::~SubMesh()
{
	ENN_SAFE_DELETE(vb_);
	ENN_SAFE_DELETE(ib_);
	ENN_SAFE_FREE(vertex_data_);
	ENN_SAFE_FREE(index_data_);
}

void SubMesh::setVertexCount(uint32 vertex_count)
{
	/** could not change the vb size. */
	if (vb_)
	{
		ENN_ASSERT(0);
		return;
	}

	ENN_ASSERT(!vb_type_.empty());
	ENN_ASSERT(vertex_count > 0);

	vb_count_ = vertex_count;

	for (size_t i = 0; i < vb_type_.size(); ++i)
	{
		stride_ += vb_type_[i].element_size();
	}
}

void SubMesh::setIndexCount(uint32 idx_count)
{
	if (ib_)
	{
		ENN_ASSERT(0);
		return;
	}

	ENN_ASSERT(idx_count > 0);
	ib_count_ = idx_count;
}

void SubMesh::setVertexData(void* data)
{
	uint32 vertex_buffer_size = stride_ * getVertexCount();

	if (!vertex_data_)
	{
		vertex_data_ = ENN_MALLOC(vertex_buffer_size);
	}

	memcpy(vertex_data_, data, vertex_buffer_size);
}

void SubMesh::setIndexData(void* data)
{
	uint32 index_buffer_size = getIdxSize() * getIndexCount();
	if (!index_data_)
	{
		index_data_ = ENN_MALLOC(index_buffer_size);
	}

	memcpy(index_data_, data, index_buffer_size);
}

void* SubMesh::getVertexData()
{
	if (!vertex_data_)
	{
		uint32 vertex_buffer_size = stride_ * getVertexCount();
		vertex_data_ = ENN_MALLOC(vertex_buffer_size);
	}

	return vertex_data_;
}


void* SubMesh::getIndexData()
{
	if (!index_data_)
	{
		uint32 index_buffer_size = getIdxSize() * getIndexCount();
		index_data_ = ENN_MALLOC(index_buffer_size);
	}

	return index_data_;
}

void SubMesh::makeGPUBuffer()
{
	if (vertex_data_)
	{
		if (!vb_)
		{
			D3D11BufferDesc vb_desc;

			vb_desc.setDefaultVertexBuffer(vb_count_ * stride_, false);
			D3D11_SUBRESOURCE_DATA data;

			data.pSysMem = vertex_data_;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			vb_ = g_render_device_->createVertexBuffer(vb_desc, &data);
		}
	}
	
	if (index_data_)
	{
		if (!ib_)
		{
			D3D11BufferDesc ib_desc;

			ib_desc.setDefaultIndexBuffer(ib_count_ * getIdxSize(), false);
			D3D11_SUBRESOURCE_DATA data;

			data.pSysMem = index_data_;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			ib_ = g_render_device_->createIndexBuffer(ib_desc, &data);
		}
	}
	

	ENN_SAFE_FREE(vertex_data_);
	ENN_SAFE_FREE(index_data_);
}

void SubMesh::setName(const String& name)
{
	name_ = name;
}

void SubMesh::setPrimType(uint32 prim_type)
{
	
	// if we have created vb and ib, we
	// should not change the type.
	ENN_ASSERT(!vb_);
	ENN_ASSERT(!ib_);

	prim_type_ = prim_type;
}

void SubMesh::setVertexType(const VertexElementsType& vb_type)
{
	 
	// if we have created vb and ib, we
	// should not change the type.
	ENN_ASSERT(!vb_);

	vb_type_ = vb_type;
}

void SubMesh::setIdxType(uint32 ib_type)
{
	
	// if we have created vb and ib, we
	// should not change the type.
	ENN_ASSERT(!ib_);

	ib_type_ = ib_type;
}

uint32 SubMesh::getVertexCount() const
{
	return vb_count_;
}

uint32 SubMesh::getIndexCount() const
{
	return ib_count_;
}

const String& SubMesh::getName() const
{
	return name_;
}

void SubMesh::setMaterialName(const String& name)
{
	mtr_name_ = name;
}

const String& SubMesh::getMaterialName() const
{
	return mtr_name_;
}

uint32 SubMesh::getPrimType() const
{
	return prim_type_;
}

const VertexElementsType& SubMesh::getVertexType() const
{
	return vb_type_;
}

uint32 SubMesh::getIdxType() const
{
	return ib_type_;
}

uint32 SubMesh::getVertexSize() const
{
	return stride_;
}

uint32 SubMesh::getIdxSize() const
{
	if (ib_type_ == IDX_16)
	{
		return sizeof(uint16);
	}
	else if (ib_type_ == IDX_32)
	{
		return sizeof(int);
	}
	else
	{
		ENN_ASSERT(0);
		return 0;
	}
}

D3D11VertexBuffer* SubMesh::getVB() const
{
	return static_cast<D3D11VertexBuffer*>(vb_->resource_);

}

D3D11IndexBuffer* SubMesh::getIB() const
{
	return static_cast<D3D11IndexBuffer*>(ib_->resource_);
}

const AxisAlignedBox& SubMesh::getAABB() const
{
	return aabb_;
}

AxisAlignedBox& SubMesh::getAABB()
{
	return aabb_;
}

void SubMesh::updateAABB()
{
	ENN_ASSERT(vb_count_ > 0);

	if (!vb_count_)
	{
		return;
	}
	
	void* data = 0;

	if (!vertex_data_)
	{
		ENN_ASSERT(0);
		return;
	}
	else
	{
		data = vertex_data_;
	}

	aabb_.setNull();

	aabb_.merge((vec3f*)data, vb_count_, stride_);
}

uint32 SubMesh::getPrimCount() const
{
	uint32 idx_count = getIndexCount();
	return idx_count / getPrimVertexCount();
}

uint32 SubMesh::getPrimVertexCount() const
{
	switch (prim_type_)
	{
	case TT_TRIANGLE_LIST:
		return 3;
	case TT_LINE_LIST:
		return 2;
	case TT_POINT_LIST:
		return 1;
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

uint32 SubMesh::getPrimVertexIndex(uint32 prim, uint32 vtx) const
{
	uint32 idx = prim * getPrimVertexCount() + vtx;
	return getIndex(idx);
}

uint32 SubMesh::getIndex( uint32 i ) const
{
	const void* data = 0;

	data = index_data_;

	ENN_ASSERT(data);

	const uchar* udata = static_cast<const uchar*>(data);

	const uchar* i_curr_data = udata + i * getIdxSize();

	if (ib_type_ == IDX_16)
	{
		return *((const ushort*)i_curr_data);
	}
	else if (ib_type_ == IDX_32)
	{
		return *((const uint32*)i_curr_data);
	}
	else
	{
		ENN_ASSERT(0);
		return 0;
	}
}

const void* SubMesh::getVertex(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getVertex(i);
}

void* SubMesh::getVertex(uint32 i)
{
	uchar* data = static_cast<uchar*>(vertex_data_);

	ENN_ASSERT(data);

	return data + i * getVertexSize();
}

const vec3f* SubMesh::getPosition(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getPosition(i);
}

vec3f* SubMesh::getPosition(uint32 i)
{
	/** off set must be zero */
	return (vec3f*)(getVertex(i));
}

const vec3f* SubMesh::getNormal(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getNormal(i);
}

vec3f* SubMesh::getNormal(uint32 i)
{
	uint32 normal_offset = D3D11RenderDevice::getElementOffset(vb_type_, VE_NORMAL);

	return (vec3f*)((uchar*)getVertex(i) + normal_offset);
}

const vec4f* SubMesh::getTangent(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getTangent(i);
}

vec4f* SubMesh::getTangent(uint32 i)
{
	uint32 tangent_offset = D3D11RenderDevice::getElementOffset(vb_type_, VE_TANGENT);
	return (vec4f*)((uchar*)getVertex(i) + tangent_offset);
}

const vec2f* SubMesh::getUV1(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getUV1(i);
}

vec2f* SubMesh::getUV1(uint32 i)
{
	uint32 uv1_offset = D3D11RenderDevice::getElementOffset(vb_type_, VE_TEXCOORD1);
	return (vec2f*)((uchar*)getVertex(i) + uv1_offset);
}

const void* SubMesh::getElement(uint32 i, int element) const
{
	return const_cast<SubMesh*>(this)->getElement(i, element);
}

void* SubMesh::getElement(uint32 i, int element)
{
	uint32 offset = D3D11RenderDevice::getElementOffset(vb_type_, vb_type_[i]);
	return ((uchar*)getVertex(i) + offset);
}

vec3f SubMesh::getNormalByFace( uint32 i ) const
{
	uint32 i0 = getPrimVertexIndex(i, 0);
	uint32 i1 = getPrimVertexIndex(i, 1);
	uint32 i2 = getPrimVertexIndex(i, 2);

	const vec3f& v0 = *getPosition(i0);
	const vec3f& v1 = *getPosition(i1);
	const vec3f& v2 = *getPosition(i2);

	vec3f va = v1 - v0;
	vec3f vb = v2 - v1;
	vec3f n = va.crossProduct(vb);

	/** 考虑到面积因素，不作单位化 */
	return n;
}

D3D11InputLayout* SubMesh::getRenderLayout()
{
	if (render_layout_) return render_layout_;

	render_layout_ = ENN_NEW D3D11InputLayout();

	render_layout_->bindVertexStream(getVB(), vb_type_);
	render_layout_->bindIndexStream(getIB(), ib_type_ == IDX_16 ? EF_R16UI : EF_R32UI);

	return render_layout_;
}

bool SubMesh::calcNormal()
{
	if (prim_type_ != TT_TRIANGLE_LIST)
	{
		return false;
	}

	if (!D3D11RenderDevice::hasElement(vb_type_, VE_NORMAL))
	{
		return false;
	}

	if (getPrimCount() <= 0)
	{
		return false;
	}

	IntListList adjList;
	getVertexAdjList(adjList);

	uint32 vtx_cnt = getVertexCount();
	for (uint32 i = 0; i < vtx_cnt; ++i)
	{
		vec3f n(0.0f, 0.0f, 0.0f);

		const IntList& adjs = adjList[i];
		for (uint32 j = 0; j < adjs.size(); ++j)
		{
			uint32 face_idx = adjs[j];
			n += getNormalByFace(face_idx);
		}

		vec3f normalized_n = n.normalisedCopy();

		if (Math::isVector3Zero(normalized_n))
		{
			normalized_n = Vector3::UNIT_Y;
		}

		*getNormal(i) = normalized_n;
	}

	return true;
}


// http://www.terathon.com/code/tangent.html
bool SubMesh::calcTangent()
{
	if (prim_type_ != TT_TRIANGLE_LIST)
	{
		return false;
	}

	if (!D3D11RenderDevice::hasElement(vb_type_, VE_TANGENT))
	{
		return false;
	}

	uint32 prim_count = getPrimCount();
	if (prim_count <= 0)
	{
		return false;
	}

	uint32 vtx_cnt = getVertexCount();

	enn::vector<vec3f>::type tangents(vtx_cnt);
	enn::vector<vec3f>::type bitangents(vtx_cnt);

	for (uint32 i = 0; i < prim_count; ++i)
	{
		uint32 i1 = getPrimVertexIndex( i, 0 );
		uint32 i2 = getPrimVertexIndex( i, 1 );
		uint32 i3 = getPrimVertexIndex( i, 2 );

		const vec3f& v1 = *getPosition( i1 );
		const vec3f& v2 = *getPosition( i2 );
		const vec3f& v3 = *getPosition( i3 );

		const vec2f& w1 = *getUV1( i1 );
		const vec2f& w2 = *getUV1( i2 );
		const vec2f& w3 = *getUV1( i3 );

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		Vector3 sDir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		Vector3 tDir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tangents[i1] += sDir;
		tangents[i2] += sDir;
		tangents[i3] += sDir;

		bitangents[i1] += tDir;
		bitangents[i2] += tDir;
		bitangents[i3] += tDir;
	}

	for (uint32 i = 0; i < vtx_cnt; ++i)
	{
		Vector3& n = *getNormal(i);
		Vector3& t = tangents[i];
		Vector3 tangent = ((t - n * n.dotProduct(t))).normalisedCopy();

		float sign = n.crossProduct(t).dotProduct(bitangents[i]) < 0.0f ? -1.0f : 1.0f;

		Vector4& finalTangent = *getTangent(i);
		finalTangent = Vector4(tangent, sign);
	}

	return true;
}


void SubMesh::getVertexAdjList(SubMesh::IntListList& adjList)
{
	adjList.resize(getVertexCount());

	uint32 prim_count = getPrimCount();
	for (uint32 i = 0; i < prim_count; ++i)
	{
		uint32 i0 = getPrimVertexIndex(i, 0);
		uint32 i1 = getPrimVertexIndex(i, 1);
		uint32 i2 = getPrimVertexIndex(i, 2);

		adjList[i0].push_back(i);
		adjList[i1].push_back(i);
		adjList[i2].push_back(i);
	}
}

void SubMesh::render(void* context)
{
	EffectContext* eff_ctx = (EffectContext*)context;
	g_render_device_->doRender(*getRenderLayout(), eff_ctx->eff_);
}

//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
{
	setLoadPriority(MODEL_LOAD_PRIORITY);
}

Mesh::~Mesh()
{
	clear();
}

uint32 Mesh::getSubMeshCount() const
{
	return (uint32)sub_mesh_list_.size();
}

const SubMesh* Mesh::getSubMesh(int i) const
{
	SubMeshList::const_iterator iter = sub_mesh_list_.find(i);

	ENN_ASSERT(iter != sub_mesh_list_.end());

	return iter->second;
}

SubMesh* Mesh::getSubMesh(int i)
{
	SubMeshList::iterator iter = sub_mesh_list_.find(i);

	ENN_ASSERT(iter != sub_mesh_list_.end());

	return iter->second;
}

const SubMesh* Mesh::getSubMeshByName(const String& name) const
{
	ENN_FOR_EACH_CONST(SubMeshList, sub_mesh_list_, it)
	{
		if (it->second->getName() == name)
		{
			return it->second;
		}
	}

	return 0;
}

SubMesh* Mesh::getSubMeshByName(const String& name)
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		if (it->second->getName() == name)
		{
			return it->second;
		}
	}

	return 0;
}

const Mesh::SubMeshList& Mesh::getSubMeshList() const
{
	return sub_mesh_list_;
}

Mesh::SubMeshList& Mesh::getSubMeshList()
{
	return sub_mesh_list_;
}

void Mesh::makeGPUBuffer()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->makeGPUBuffer();
	}
}

SubMesh* Mesh::createNewSubMesh(int i)
{
	SubMesh* sub_mesh = ENN_NEW SubMesh(this);

	sub_mesh_list_.insert(SubMeshList::value_type(i, sub_mesh));

	return sub_mesh;
}

void Mesh::destorySubMesh(int i)
{
	SubMeshList::iterator it = sub_mesh_list_.find(i);
	if (it != sub_mesh_list_.end())
	{
		SubMesh* sub_mesh = it->second;
		ENN_SAFE_DELETE(sub_mesh);

		sub_mesh_list_.erase(it);
	}
}

void Mesh::clear()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		ENN_SAFE_DELETE(sub_mesh);
	}

	sub_mesh_list_.clear();
}

void Mesh::updateAABB(bool update_sub)
{
	int cnt = getSubMeshCount();
	if (cnt <= 0)
	{
		aabb_.setNull();
	}

	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;

		ENN_ASSERT(sub_mesh);

		if (update_sub)
		{
			sub_mesh->updateAABB();
		}

		const AxisAlignedBox& sub_aabb = sub_mesh->getAABB();
		aabb_.merge(sub_aabb);
	}

}

const AxisAlignedBox& Mesh::getAABB() const
{
	return aabb_;
}

AxisAlignedBox& Mesh::getAABB()
{
	return aabb_;
}

void Mesh::setMaterialName(int idx, const String& mtr_name)
{
	getSubMesh(idx)->setMaterialName(mtr_name);
}

void Mesh::setMaterialName(const String& mtr_name)
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->setMaterialName(mtr_name);
	}
}

const String& Mesh::getMaterialName(int idx) const
{
	return getSubMesh(idx)->getMaterialName();
}

void Mesh::calcNormal()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->calcNormal();
	}
}

void Mesh::calcTangent()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->calcTangent();
	}
}

}

