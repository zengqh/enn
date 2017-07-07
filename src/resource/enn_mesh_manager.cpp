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
// File: enn_mesh_manager.cpp
//

#include "enn_mesh_manager.h"
#include "enn_file_system.h"
#include "enn_mesh_manual_creator.h"
#include "enn_mesh_fullscreen_creator.h"

namespace enn
{

#define CURR_MESH_MAJOR_VERSION		(1)
#define CURR_MESH_VICE_VERSION		(0)

MeshManager* g_mesh_mgr_ = 0;
//////////////////////////////////////////////////////////////////////////
static inline const char* _loadAlignedString(const int*& nextdata)
{
	int len = *nextdata;

	ENN_ASSERT((len % 4) == 0);

	++nextdata;
	const char* str = (const char*)nextdata;
	nextdata = (const int*)((char*)nextdata + len);
	return str;
}

static inline bool assert_mesh_ptr_align(const void* ptr)
{
	return ((uint32)ptr % 4 == 0) ? true : false;
}

// MeshResAutoCreator
MeshResAutoCreator::MeshResAutoCreator()
: temp_mesh_(0)
, curr_temp_sub_mesh_(0)
, curr_temp_sub_mesh_info_(0)
, memory_buff_(0)
{
	m_basePath = "mesh";
}

MeshResAutoCreator::~MeshResAutoCreator()
{

}

bool MeshResAutoCreator::buildResource(Resource* res)
{
	temp_mesh_ = static_cast<Mesh*>(res);

	temp_mesh_->makeGPUBuffer();

	temp_mesh_ = 0;

	return true;
}

bool MeshResAutoCreator::prepareResourceImpl(Resource* res, MemoryBuffer* buff)
{
	ENN_ASSERT(buff);

	if (!buff)
	{
		return false;
	}

	temp_mesh_ = static_cast<Mesh*>(res);
	memory_buff_ = buff;

	if (!memory_buff_)
	{
		return false;
	}

	void* data = memory_buff_->GetHandle();

	ENN_HEAD_FILE_INFO* ehfi = static_cast<ENN_HEAD_FILE_INFO*>(data);

	if (strncmp(ehfi->achr_file_flag, "enn", 3) != 0)
	{
		return false;
	}

	uchar ver_first = ehfi->achr_version[0];
	uchar ver_second = ehfi->achr_version[1];

	int total_ver = ver_first * 256 + ver_second;

	onMeshBeginParse(ehfi);

	const int* next_data = (const int*)(ehfi + 1);

	// parse sub mesh
	int sub_mesh_count = ehfi->sub_mesh_count;
	ENN_ASSERT(sub_mesh_count > 0);

	for (int i = 0; i < sub_mesh_count; ++i)
	{
		ENN_SUB_MESH_HEAD_INFO esmhi;

		buff->setPosition((uchar*)next_data - (uchar*)buff->GetHandle());
		g_mesh_mgr_->readSubMeshInfo(esmhi, *buff);
		onMeshBeginSubMesh(i, &esmhi);
		next_data = (const int*)((uchar*)buff->GetHandle() + buff->GetPosition());

		ENN_ASSERT(assert_mesh_ptr_align(next_data));

		/** vertex data */
		int vertex_count = *next_data;
		next_data++;

		curr_temp_sub_mesh_->setVertexCount(vertex_count);
		curr_temp_sub_mesh_->setVertexData((void*)next_data);

		next_data = (const int*)((const char*)next_data + vertex_count * esmhi.vertex_bytes_);

		/** index data */
		ENN_ASSERT(assert_mesh_ptr_align(next_data));
		int index_count = *next_data;
		++next_data;

		curr_temp_sub_mesh_->setIndexCount(index_count);
		curr_temp_sub_mesh_->setIndexData((void*)next_data);
		int indices_bytes = index_count * esmhi.index_bytes_;

		indices_bytes = ENN_ALIGN_4_BYTES(indices_bytes);

		next_data = (const int*)((const char*)next_data + indices_bytes);

		ENN_ASSERT(assert_mesh_ptr_align(next_data));

		/** sub mesh name */
		const char* name = _loadAlignedString(next_data);
		curr_temp_sub_mesh_->setName(name);

		/** sub mesh mtr name */
		const char* mtr_name = _loadAlignedString(next_data);
		curr_temp_sub_mesh_->setMaterialName(mtr_name);

		onMeshEndSubMesh();
	}

	g_file_system_->freeFileData(memory_buff_);

	memory_buff_ = 0;
	temp_mesh_ = 0;
	curr_temp_sub_mesh_ = 0;
	curr_temp_sub_mesh_info_ = 0;

	return true;
}

void MeshResAutoCreator::onMeshBeginParse(ENN_HEAD_FILE_INFO* ehfi)
{
	/** aabb */
	AxisAlignedBox& mesh_aabb = temp_mesh_->getAABB();
	mesh_aabb.setExtents(ehfi->aabb.vMin, ehfi->aabb.vMax);
}

void MeshResAutoCreator::onMeshBeginSubMesh(int sub_idx,
	const ENN_SUB_MESH_HEAD_INFO* esmhi)
{
	curr_temp_sub_mesh_info_ = const_cast<ENN_SUB_MESH_HEAD_INFO*>(esmhi);
	curr_temp_sub_mesh_ = temp_mesh_->createNewSubMesh(sub_idx);

	AxisAlignedBox& mesh_aabb = curr_temp_sub_mesh_->getAABB();
	mesh_aabb.setExtents(curr_temp_sub_mesh_info_->aabb.vMin, curr_temp_sub_mesh_info_->aabb.vMax);
	curr_temp_sub_mesh_->setPrimType(curr_temp_sub_mesh_info_->prim_type_);
	curr_temp_sub_mesh_->setVertexType(curr_temp_sub_mesh_info_->vertex_type_);
	curr_temp_sub_mesh_->setIdxType(curr_temp_sub_mesh_info_->index_type_);
}

void MeshResAutoCreator::onMeshEndSubMesh()
{

}


//////////////////////////////////////////////////////////////////////////
// MeshManager
MeshManager::MeshManager()
: curr_temp_save_mesh_(0)
, curr_temp_save_sub_mesh_(0)
, point_lit_mesh_(0)
, spot_lit_mesh_(0)
{
	g_mesh_mgr_ = this;
}

MeshManager::~MeshManager()
{

}

void MeshManager::init()
{
	ResourceManager::init();

	setAutoCreator(ENN_NEW MeshResAutoCreator());
}

Mesh* MeshManager::createCube(const String& name, float size)
{
	Mesh* mesh = static_cast<Mesh*>(g_mesh_mgr_->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshCubeCreatorImpl(size));
	}

	return mesh;
}

Mesh* MeshManager::createSphere(const String& name, float size, int segments)
{
	Mesh* mesh = static_cast<Mesh*>(g_mesh_mgr_->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshSphereCreatorImpl(size, segments));
	}

	return mesh;
}

Mesh* MeshManager::createPlane(const String& name, float x_len, float z_len, int x_seg, int z_seg)
{
	Mesh* mesh = static_cast<Mesh*>(g_mesh_mgr_->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshPlaneCreatorImpl(x_len, z_len, x_seg, z_seg));
	}

	return mesh;
}

Resource* MeshManager::createImpl()
{
	Resource* res = ENN_NEW Mesh;
	res->setManager(this);

	return res;
}

Mesh* MeshManager::getPointLitMesh()
{
	if (point_lit_mesh_)
	{
		return point_lit_mesh_;
	}

	point_lit_mesh_ = static_cast<Mesh*>(createResource("point_lit_mesh"));

	const float minRadius = 1;
	const int   segments = 15;

	float radius = minRadius / Math::cos(Math::PI / (segments - 1));
	float size = radius * 2;

	point_lit_mesh_->setResCreator(ENN_NEW MeshSphereCreatorImpl(size, segments));
	point_lit_mesh_->load(false);

	return point_lit_mesh_;
}

Mesh* MeshManager::getSpotLitMesh()
{
	if (spot_lit_mesh_) return spot_lit_mesh_;

	spot_lit_mesh_ = static_cast<Mesh*>(createResource("spot_lit_mesh"));

	// 要求最小的半径和切分数
	const float minRadius = 1;
	const float height = 1;
	const int   wsegments = 20;
	const int   hsegments = 2;

	float radius = minRadius / Math::cos(Math::PI / (wsegments - 1));
	float width = radius * 2;

	spot_lit_mesh_->setResCreator(ENN_NEW MeshConeCreatorImpl(width, height, wsegments, hsegments, true));
	spot_lit_mesh_->load(false);

	return spot_lit_mesh_;
}

Mesh* MeshManager::getScreenQuadMesh()
{
	if (full_screen_mesh_)
	{
		return full_screen_mesh_;
	}

	full_screen_mesh_ = static_cast<Mesh*>(createResource("full_screen_mesh"));

	if (full_screen_mesh_)
	{
		full_screen_mesh_->setResCreator(ENN_NEW FullScreenMeshCreatorImpl);
	}

	full_screen_mesh_->load(false);

	return full_screen_mesh_;
}

void MeshManager::saveMesh(Mesh* mesh, const String& file_name)
{
	const String& root_path = g_file_system_->getRootPath();
	String full_file_name = root_path + "/mesh/" + file_name;

	File file(full_file_name.c_str(), FILE_WRITE);
	saveMesh(mesh, &file);
}

void MeshManager::saveMesh(Mesh* mesh, File* file)
{
	curr_temp_save_mesh_ = mesh;

	int sub_mesh_count = (int)(mesh->getSubMeshCount());

	ENN_ASSERT(sub_mesh_count > 0);

	/** ENN_HEAD_FILE_INFO */
	ENN_HEAD_FILE_INFO ehfi = { 0 };
	memcpy(ehfi.achr_file_flag, "enn", 3);
	ehfi.achr_version[0] = CURR_MESH_MAJOR_VERSION;
	ehfi.achr_version[1] = CURR_MESH_VICE_VERSION;
	ehfi.sub_mesh_count = sub_mesh_count;
	copyMeshInfo(&ehfi);

	file->Write(&ehfi, sizeof(ehfi));

	/** write sub mesh */
	for (int sub_idx = 0; sub_idx < sub_mesh_count; ++sub_idx)
	{
		curr_temp_save_sub_mesh_ = mesh->getSubMesh(sub_idx);

		ENN_SUB_MESH_HEAD_INFO esmhi = { 0 };

		copySubMeshInfo(&esmhi);

		g_mesh_mgr_->writeSubMeshInfo(esmhi, *file);

		/** write vertex data */
		int vertex_count = 0;
		void* vertex_data = 0;

		vertex_count = curr_temp_save_sub_mesh_->getVertexCount();
		vertex_data = curr_temp_save_sub_mesh_->getVertexData();

		ENN_ASSERT(vertex_data);

		file->WriteBin(vertex_count);
		file->Write(vertex_data, vertex_count * esmhi.vertex_bytes_);

		/** write index data */
		int index_count = 0;
		void* index_data = 0;

		index_count = curr_temp_save_sub_mesh_->getIndexCount();
		index_data = curr_temp_save_sub_mesh_->getIndexData();

		ENN_ASSERT(index_data);

		file->WriteBin(index_count);
		int indices_bytes = index_count * esmhi.index_bytes_;
		file->Write(index_data, indices_bytes);
		writeAlignedBlank(file, indices_bytes);

		// write sub mesh name
		file->WriteLenStringAlign4(curr_temp_save_sub_mesh_->getName());

		// write material name
		file->WriteLenStringAlign4(curr_temp_save_sub_mesh_->getMaterialName());
	}

	writeCmd(file, ENN_MESH_CMD::C_END);

	file->Flush();
}

void MeshManager::copyMeshInfo(ENN_HEAD_FILE_INFO* ehfi)
{
	const AxisAlignedBox& aabb = curr_temp_save_mesh_->getAABB();
	const vec3f& vmin = aabb.getMinimum();
	const vec3f& vmax = aabb.getMaximum();

	ehfi->aabb.vMin[0] = vmin.x;
	ehfi->aabb.vMin[1] = vmin.y;
	ehfi->aabb.vMin[2] = vmin.z;

	ehfi->aabb.vMax[0] = vmax.x;
	ehfi->aabb.vMax[1] = vmax.y;
	ehfi->aabb.vMax[2] = vmax.z;
}

void MeshManager::copyAABB(ENN_AABB_INFO* aabb, const AxisAlignedBox& aabb_src)
{
	const vec3f& vmin = aabb_src.getMinimum();
	const vec3f& vmax = aabb_src.getMaximum();

	aabb->vMin[0] = vmin.x;
	aabb->vMin[1] = vmin.y;
	aabb->vMin[2] = vmin.z;

	aabb->vMax[0] = vmax.x;
	aabb->vMax[1] = vmax.y;
	aabb->vMax[2] = vmax.z;
}

void MeshManager::writeCmd(File* file, int cmd_id)
{
	file->Write(&cmd_id, sizeof(cmd_id));
}

void MeshManager::writeAlignedBlank(File* file, int bytes)
{
	int align_size = ENN_ALIGN_4_BYTES(bytes);
	int pad = align_size - bytes;

	if (pad)
	{
		char dummy[4] = { 0 };
		file->Write(dummy, pad);
	}
}

void MeshManager::copySubMeshInfo(ENN_SUB_MESH_HEAD_INFO* sub_mesh_info)
{
	copyAABB(&sub_mesh_info->aabb, curr_temp_save_sub_mesh_->getAABB());
	sub_mesh_info->prim_type_ = curr_temp_save_sub_mesh_->getPrimType();
	sub_mesh_info->vertex_type_ = curr_temp_save_sub_mesh_->getVertexType();
	sub_mesh_info->index_type_ = curr_temp_save_sub_mesh_->getIdxType();
	sub_mesh_info->vertex_bytes_ = curr_temp_save_sub_mesh_->getVertexSize();
	sub_mesh_info->index_bytes_ = curr_temp_save_sub_mesh_->getIdxSize();
}

void MeshManager::writeSubMeshInfo(const ENN_SUB_MESH_HEAD_INFO& sub_mesh_info, Serializer& dest)
{
	dest.WriteBin(sub_mesh_info.aabb);
	dest.WriteBin(sub_mesh_info.prim_type_);
	dest.WriteBin(sub_mesh_info.vertex_bytes_);
	dest.WriteBin(sub_mesh_info.index_type_);
	dest.WriteBin(sub_mesh_info.index_bytes_);

	dest.WriteBin((int)sub_mesh_info.vertex_type_.size());
	dest.Write(&sub_mesh_info.vertex_type_[0], sub_mesh_info.vertex_type_.size() * sizeof(VertexElement));
}

void MeshManager::readSubMeshInfo(ENN_SUB_MESH_HEAD_INFO& sub_mesh_info, Deserializer& source)
{
	source.ReadBin(sub_mesh_info.aabb);
	source.ReadBin(sub_mesh_info.prim_type_);
	source.ReadBin(sub_mesh_info.vertex_bytes_);
	source.ReadBin(sub_mesh_info.index_type_);
	source.ReadBin(sub_mesh_info.index_bytes_);

	int vertex_elems_num;
	source.ReadBin(vertex_elems_num);

	sub_mesh_info.vertex_type_.resize(vertex_elems_num);
	source.Read(&sub_mesh_info.vertex_type_[0], vertex_elems_num * sizeof(VertexElement));
}

}

