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
// Time: 2014/04/23
// File: enn_mesh_manager.h
//

#pragma once

#include "enn_mesh.h"
#include "enn_resource_manager.h"

namespace enn
{
	
//////////////////////////////////////////////////////////////////////////
struct ENN_AABB_INFO
{
	float vMin[3];
	float vMax[3];
};


struct ENN_HEAD_FILE_INFO
{
	char achr_file_flag[3];
	char achr_version[4];
	char achr_reversed[21];

	ENN_AABB_INFO aabb;
	int sub_mesh_count;
};

struct ENN_MESH_CMD
{
	enum
	{
		C_DIFF = 0,
		C_DIFF2,
		C_LIGHTING,
		C_NORMALMAP,
		C_SPECMAP,
		C_EMISSIVEMAP,
		C_TEXANI,
		C_CULLMODE,
		C_NAME,
		C_ALPHA_TEST,
		C_SKELETON,
		C_TU_END,
		C_END
	};
};

struct ENN_SUB_MESH_HEAD_INFO
{
	ENN_AABB_INFO				aabb;
	int							prim_type_;
	int							vertex_bytes_;
	int							index_type_;
	int							index_bytes_;
	VertexElementsType			vertex_type_;
};

//////////////////////////////////////////////////////////////////////////
// MeshResAutoCreator
class MeshResAutoCreator : public ResAutoCreatorBase
{
public:
	MeshResAutoCreator();
	~MeshResAutoCreator();

	virtual bool buildResource(Resource* res);

protected:
	virtual bool prepareResourceImpl(Resource* res, MemoryBuffer* buff);

protected:
	void onMeshBeginParse(ENN_HEAD_FILE_INFO* ehfi);
	
	void onMeshBeginSubMesh(int sub_idx, const ENN_SUB_MESH_HEAD_INFO* esmhi);
	void onMeshEndSubMesh();

	void loadSkeleton(const int*& data);
	void onSkelGetBone(const char* name, int id,
		const vec3f& pos, const Quaternion& quat, const vec3f& sca,
		const Matrix4& offset_matrix);

	void onSkelGetBoneParent(int i_child, int i_parent);

protected:
	MemoryBuffer*					memory_buff_;
	Mesh*							temp_mesh_;
	SubMesh*						curr_temp_sub_mesh_;
	ENN_SUB_MESH_HEAD_INFO*			curr_temp_sub_mesh_info_;
};

//////////////////////////////////////////////////////////////////////////
class MeshManager : public ResourceManager
{
public:
	MeshManager();
	virtual ~MeshManager();

public:
	virtual void init();

	KHAOS_RESMAN_COMM_OVERRIDE(Mesh)

public:
	static Mesh* createCube(const String& name, float size);
	static Mesh* createSphere(const String& name, float size, int segments);
	static Mesh* createPlane(const String& name, float x_len, float z_len, int x_seg, int z_seg);
	
public:
	Mesh* getPointLitMesh();
	Mesh* getSpotLitMesh();
	Mesh* getScreenQuadMesh();

public:
	void saveMesh(Mesh* mesh, const String& file_name);
	void saveMesh(Mesh* mesh, File* file);

protected:
	void copyMeshInfo(ENN_HEAD_FILE_INFO* ehfi);
	void copyAABB(ENN_AABB_INFO* aabb, const AxisAlignedBox& aabb_src);
	void writeCmd(File* file, int cmd_id);
	void writeAlignedName(File* file, const char* name);
	void writeAlignedBlank(File* file, int bytes);
	void copySubMeshInfo(ENN_SUB_MESH_HEAD_INFO* sub_mesh_info);

public:
	void writeSubMeshInfo(const ENN_SUB_MESH_HEAD_INFO& sub_mesh_info, Serializer& dest);
	void readSubMeshInfo(ENN_SUB_MESH_HEAD_INFO& sub_mesh_info, Deserializer& source);

protected:
	virtual Resource* createImpl();

protected:
	Mesh*			curr_temp_save_mesh_;
	SubMesh*		curr_temp_save_sub_mesh_;

	Mesh*			point_lit_mesh_;
	Mesh*			spot_lit_mesh_;
	Mesh*			full_screen_mesh_;
};

extern MeshManager* g_mesh_mgr_;
}