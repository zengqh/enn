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
// File: enn_mesh.h

#pragma once

#include "enn_resource.h"
#include "enn_d3d11_gpu_buffer.h"
#include "enn_d3d11_resource_proxy.h"
#include "enn_d3d11_input_layout.h"

namespace enn
{

class Mesh;
//////////////////////////////////////////////////////////////////////////
class SubMesh : public AllocatedObject
{
	typedef enn::vector<uint32>::type		IntList;
	typedef enn::vector<IntList>::type		IntListList;

public:
	SubMesh(Mesh* owner);
	~SubMesh();

public:
	void setName(const String& name);
	const String& getName() const;

	void setMaterialName(const String& name);
	const String& getMaterialName() const;

public:
	void setVertexCount(uint32 vertex_count);
	void setIndexCount(uint32 idx_count);
	void setVertexData(void* data);
	void setIndexData(void* data);

	void* getVertexData();
	void* getIndexData();

	void makeGPUBuffer();

	void setPrimType(uint32 prim_type);
	void setVertexType(const VertexElementsType& vb_type);
	void setIdxType(uint32 ib_type);

	uint32 getVertexCount() const;
	uint32 getIndexCount() const;

	uint32 getPrimType() const;
	const VertexElementsType& getVertexType() const;
	uint32 getIdxType() const;

	uint32 getVertexSize() const;
	uint32 getIdxSize() const;

	D3D11VertexBuffer* getVB() const;
	D3D11IndexBuffer* getIB() const;

	const AxisAlignedBox& getAABB() const;
	AxisAlignedBox& getAABB();
	void updateAABB();

public:
	uint32 getPrimCount() const;
	uint32 getPrimVertexCount() const;
	uint32 getPrimVertexIndex(uint32 prim, uint32 vtx) const;

	uint32 getIndex(uint32 i) const;

	const void* getVertex(uint32 i) const;
	void* getVertex(uint32 i);

	const vec3f* getPosition(uint32 i) const;
	vec3f* getPosition(uint32 i);

	const vec3f* getNormal(uint32 i) const;
	vec3f* getNormal(uint32 i);

	const vec4f* getTangent(uint32 i) const;
	vec4f* getTangent(uint32 i);

	const vec2f* getUV1(uint32 i) const;
	vec2f* getUV1(uint32 i);

	const void* getElement(uint32 i, int element) const;
	void* getElement(uint32 i, int element);

	vec3f getNormalByFace( uint32 i ) const;

public:
	D3D11InputLayout* getRenderLayout();

public:
	bool calcNormal();
	bool calcTangent();

public:
	void render(void* context);

protected:
	void getVertexAdjList(IntListList& adjList);

protected:
	String						name_;
	String						mtr_name_;
	AxisAlignedBox				aabb_;
	D3D11ResourceProxy*			vb_;
	D3D11ResourceProxy*			ib_;
	uint32						vb_count_;
	uint32						ib_count_;
	uint32						stride_;
	bool						use_memory_;

	String						mat_name_;

	uint32						prim_type_;
	VertexElementsType			vb_type_;
	uint32						ib_type_;

	void*						vertex_data_;
	void*						index_data_;

	Mesh*						owner_;

	D3D11InputLayout*			render_layout_;
};

//////////////////////////////////////////////////////////////////////////
class Mesh : public Resource
{
	ENN_DECLARE_RTTI(Mesh)

	enum {MODEL_LOAD_PRIORITY = 0};

public:
	typedef unordered_map<int, SubMesh*>::type SubMeshList;

public:
	Mesh();
	virtual ~Mesh();

	uint32 getSubMeshCount() const;

	const SubMesh* getSubMesh(int i) const;
	SubMesh* getSubMesh(int i);

	const SubMesh* getSubMeshByName(const String& name) const;
	SubMesh* getSubMeshByName(const String& name);

	const SubMeshList& getSubMeshList() const;
	SubMeshList& getSubMeshList();

	void makeGPUBuffer();

public:
	SubMesh* createNewSubMesh(int i);
	void destorySubMesh(int i);
	void clear();

public:
	void updateAABB(bool update_sub = false);
	const AxisAlignedBox& getAABB() const;
	AxisAlignedBox& getAABB();

public:
	void setMaterialName(int idx, const String& mtr_name);
	void setMaterialName(const String& mtr_name);
	const String& getMaterialName(int idx) const;

public:
	void calcNormal();
	void calcTangent();

protected:
	SubMeshList			sub_mesh_list_;
	AxisAlignedBox		aabb_;
};

typedef ResPtr<Mesh> MeshPtr;

}