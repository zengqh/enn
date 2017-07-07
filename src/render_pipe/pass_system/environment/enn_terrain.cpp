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
// Time: 2015/01/05
// File: enn_terrain.cpp
//

#include "enn_terrain.h"
#include "enn_terrain_section.h"
#include "enn_root.h"

namespace enn
{

const int K_Terrain_Version = 0;
const int K_Terrain_Magic = 'TRN0';


//////////////////////////////////////////////////////////////////////////
static inline void Terrain_ReadConfig(Terrain::Config* cfg, MemoryBuffer* file, int version)
{
	file->Read(&cfg->xSize, sizeof(int));
	file->Read(&cfg->zSize, sizeof(int));

	file->Read(&cfg->xVertexCount, sizeof(int));
	file->Read(&cfg->zVertexCount, sizeof(int));

	file->Read(&cfg->iVertexCount, sizeof(int));

	file->Read(&cfg->morphEnable, sizeof(bool));
	file->Read(&cfg->morphStart, sizeof(float));

	file->Read(&cfg->xSectionSize, sizeof(int));
	file->Read(&cfg->zSectionSize, sizeof(int));
	file->Read(&cfg->xSectionCount, sizeof(int));
	file->Read(&cfg->zSectionCount, sizeof(int));
	file->Read(&cfg->iSectionCount, sizeof(int));

	file->Read(&cfg->xWeightMapSize, sizeof(int));
	file->Read(&cfg->zWeightMapSize, sizeof(int));
	file->Read(&cfg->iWeightMapSize, sizeof(int));

	file->Read(&cfg->bitFlags, sizeof(int));
}

static inline void Terrain_WriteConfig(File & file, Terrain::Config * cfg)
{
	file.WriteBin(*cfg);
}

static inline bool LoadCommTex(const String& str, D3D11TextureObj*& tex)
{
	tex = ENN_NEW D3D11TextureObj();
	bool ret = tex->loadFile(str);

	ENN_ASSERT(ret);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Terrain::Terrain(const Config& config)
: inited_(false)
, lod_(0)
, heights_(0)
, normals_(0)
, weights_(0)
, default_detail_map_(0)
, default_normal_map_(0)
, default_specular_map_(0)
, locked_data_(0)
, locked_weight_map_data_(0)
, xy_stream_(0)
{
	
}

Terrain::Terrain(const char* name)
: inited_(false)
, lod_(0)
, heights_(0)
, normals_(0)
, weights_(0)
, default_detail_map_(0)
, default_normal_map_(0)
, default_specular_map_(0)
, locked_data_(0)
, locked_weight_map_data_(0)
, xy_stream_(0)
{
	lod_ = ENN_NEW TerrainLod(kMaxDetailLevel);
	load(name);
}

Terrain::~Terrain()
{

}

void Terrain::save(const char* filename)
{
	File file(filename, FILE_WRITE);

	file.WriteBin(K_Terrain_Magic);
	file.WriteBin(K_Terrain_Version);

	// write config
	Terrain_WriteConfig(file, &config_);

	// write layers
	file.Write(layers_, sizeof(Layer) * kMaxLayers);

	// write bound
	file.WriteBin(bound_);

	// write heights
	file.Write(heights_, sizeof(float) * config_.iVertexCount);
	
	// write normals
	file.Write(normals_, sizeof(float) * 3 * config_.iVertexCount);

	// write weight map
	file.Write(weights_, sizeof(Color2) * config_.iVertexCount);


	// write section layers
	for (int j = 0; j < config_.zSectionCount; ++j)
	{
		for (int i = 0; i < config_.xSectionCount; ++i)
		{
			TerrainSection* section = getSection(i, j);

			for (int l = 0; l < kMaxBlendLayers; ++l)
			{
				int layer = section->getLayer(l);
				file.WriteBin(layer);
			}
		}
	}
}

int Terrain::addLayer(const Layer& layer)
{
	for (int i = 0; i < kMaxLayers; ++i)
	{
		if (String(layers_[i].detail).empty())
		{
			layers_[i] = layer;

			LoadCommTex(layer.detail, detail_maps_[i]);
			LoadCommTex(layer.normal, detail_maps_[i]);
			LoadCommTex(layer.specular, detail_maps_[i]);

			return i;
		}
	}

	return -1;
}

const Terrain::Layer* Terrain::getLayer(int index)
{
	ENN_ASSERT(index < kMaxLayers);
	return &layers_[index];
}

void Terrain::setLayer(int index, const Layer& layer)
{
	ENN_ASSERT(index < kMaxLayers);

	Layer& old_layer = layers_[index];

	if (old_layer.detail != layer.detail)
	{
		ENN_SAFE_DELETE(detail_maps_[index]);
		LoadCommTex(layer.detail, detail_maps_[index]);
	}

	if (!String(layer.normal).empty())
	{
		if (normal_maps_[index] != default_normal_map_)
		{
			ENN_SAFE_DELETE(normal_maps_[index]);
		}
		
		LoadCommTex(layer.normal, normal_maps_[index]);
	}
	else
	{
		normal_maps_[index] = default_normal_map_;
	}

	if (!String(layer.specular).empty())
	{
		if (specular_maps_[index] != default_specular_map_)
		{
			ENN_SAFE_DELETE(specular_maps_[index]);
		}

		LoadCommTex(layer.specular, specular_maps_[index]);
	}
	else
	{
		specular_maps_[index] = default_specular_map_;
	}

	old_layer = layer;
}

void Terrain::removeLayer(int layer)
{
	layers_[layer].detail[0] = 0;
	layers_[layer].normal[0] = 0;
	layers_[layer].specular[0] = 0;
	layers_[layer].material = -1;
	layers_[layer].scale = 1;

	detail_maps_[layer] = default_detail_map_;
	normal_maps_[layer] = default_normal_map_;
	specular_maps_[layer] = default_specular_map_;
}

const Terrain::Config& Terrain::getConfig() const
{
	return config_;
}

TerrainLod* Terrain::getTerrainLod() 
{
	return lod_;
}

TerrainSection* Terrain::getSection(int x, int z)
{
	ENN_ASSERT(x < config_.xSectionCount && z < config_.zSectionCount);
	return sections_[z * config_.xSectionCount + x];
}

Vector3 Terrain::getPosition(int x, int z)
{
	ENN_ASSERT(x < config_.xSectionCount && z < config_.zSectionCount);

	float fx = (float)x / (config_.xVertexCount - 1) * config_.xSize;
	float fz = ((float)z) / (config_.zVertexCount - 1) * config_.zSize;
	float fy = getHeight(fx, fz);

	return Vector3(fx, fy, fz);
}

float Terrain::getHeight(int x, int z)
{
	ENN_ASSERT(x < config_.xVertexCount && z < config_.zVertexCount);
	return heights_[z * config_.zVertexCount + x];
}

Vector3 Terrain::getNormal(int x, int z)
{
	ENN_ASSERT(x < config_.xVertexCount && z < config_.zVertexCount);
	return normals_[z * config_.zVertexCount + x];
}

Color2 Terrain::getWeight(int x, int z)
{
	ENN_ASSERT(x < config_.xSectionCount && z < config_.zSectionCount);
	return weights_[z * config_.zVertexCount + x];
}

D3D11TextureObj* Terrain::getWeightMap(int x, int z)
{
	ENN_ASSERT(x < config_.xWeightMapSize && z < config_.zWeightMapSize);
	return weight_maps_[z * config_.zSectionCount + x];
}

const float* Terrain::getHeights() const 
{
	return heights_;
}

const Vector3* Terrain::getNormals() const
{
	return normals_;
}

void Terrain::render()
{
	g_render_device_->setSolidStateLessAndEqual(true);

	ENN_FOR_EACH(Array<TerrainSection*>, visible_sections_, it)
	{
		TerrainSection* section = *it;

		section->updateLod();
	}

	ENN_FOR_EACH(Array<TerrainSection*>, visible_sections_, it)
	{
		TerrainSection* section = *it;

		section->render();
	}
}

void Terrain::renderDeferred()
{
	ENN_FOR_EACH(Array<TerrainSection*>, visible_sections_, it)
	{
		TerrainSection* section = *it;

		section->updateLod();
	}

	ENN_FOR_EACH(Array<TerrainSection*>, visible_sections_, it)
	{
		TerrainSection* section = *it;

		section->renderDeferred();
	}
}

void Terrain::renderInMirror()
{

}

int Terrain::getVisibleSectionCount() const 
{
	return 0;
}

TerrainSection* Terrain::getVisibleSection(int i) 
{
	return 0;
}

float Terrain::getHeight(float x, float z)
{
	float sx = 0, sz = config_.zSize;
	float ex = config_.zSize, ez = 0;

	float fx = (x - sx) / (ex - sx) * (config_.xVertexCount - 1);
	float fz = (z - sz) / (ez - sz) * (config_.zVertexCount - 1);

	int ix = fx;
	int iz = fz;

	ENN_ASSERT(ix >= 0 && ix < config_.xVertexCount - 1 &&
		iz >= 0 && iz <= config_.zVertexCount);

	float dx = fx - ix;
	float dz = fz - iz;

	int ix1 = ix + 1;
	int iz1 = iz + 1;

	ix1 = Math::maxVal(ix1, config_.xVertexCount - 1);
	iz1 = Math::maxVal(iz1, config_.zVertexCount - 1);

	float a = getHeight(ix, iz);
	float b = getHeight(ix1, iz);
	float c = getHeight(ix, iz1);
	float d = getHeight(ix1, iz1);
	float m = (b + c) * 0.5f;
	float h1, h2, final;

	if (dx + dz <= 1.0f)
	{
		d = m + m - a;
	}
	else
	{
		a = m + m - d;
	}

	h1 = a * (1.0f - dx) + b * dx;
	h2 = c * (1.0f - dx) + d * dx;
	final = h1 * (1.0f - dz) + h2 * dz;

	return final;
}

Vector3 Terrain::getNormal(float x, float z)
{
	float sx = 0, sz = config_.zSize;
	float ex = config_.zSize, ez = 0;

	float fx = (x - sx) / (ex - sx) * (config_.xVertexCount - 1);
	float fz = (z - sz) / (ez - sz) * (config_.zVertexCount - 1);

	int ix = (int)fx;
	int iz = (int)fz;

	ENN_ASSERT(ix >= 0 && ix <= config_.xVertexCount - 1 &&
		iz >= 0 && iz <= config_.zVertexCount - 1);

	float dx = fx - ix;
	float dz = fz - iz;

	int ix1 = ix + 1;
	int iz1 = iz + 1;

	ix1 = Math::minVal(ix1, config_.xVertexCount - 1);
	iz1 = Math::maxVal(iz1, config_.zVertexCount - 1);

	Vector3 a = getNormal(ix, iz);
	Vector3 b = getNormal(ix1, iz);
	Vector3 c = getNormal(ix, iz1);
	Vector3 d = getNormal(ix1, iz1);
	Vector3 m = (b + c) * 0.5f;
	Vector3 h1, h2, final;

	if (dx + dz <= 1.0f)
	{
		d = m + (m - a);
	}
	else
	{
		a = m + (m - d);
	}

	h1 = a * (1.0f - dx) + b * dx;
	h2 = c * (1.0f - dx) + d * dx;
	final = h1 * (1.0f - dz) + h2 * dz;

	return final;
}

// use in editor
float* Terrain::lockHeight(const IntRect& rect)
{
	ENN_ASSERT(!locked_data_);

	int w = rect.getWidth();
	int h = rect.getHeight();

	locked_data_ = ENN_MALLOC_ARRAY_T(float, w * h);
	int index = 0;
	for (int j = rect.top; j <= rect.bottom; ++j)
	{
		for (int i = rect.left; i < rect.right; ++i)
		{
			locked_data_[index++] = getHeight(i, j);
		}
	}

	locked_rect_ = rect;

	return locked_data_;
}

void Terrain::unlockHeight()
{
	ENN_ASSERT(locked_data_);

	locked_rect_.left = Math::maxVal(0, locked_rect_.left);
	locked_rect_.top = Math::maxVal(0, locked_rect_.top);
	locked_rect_.right = Math::minVal(config_.xVertexCount - 1, locked_rect_.right);
	locked_rect_.bottom = Math::minVal(config_.zVertexCount - 1, locked_rect_.bottom);

	int index = 0;
	for (int j = locked_rect_.top; j <= locked_rect_.bottom; ++j)
	{
		for (int i = locked_rect_.left; i <= locked_rect_.right; ++i)
		{
			heights_[j * config_.xVertexCount + i] = locked_data_[index++];
		}
	}

	// need re-calculate normals
	IntRect rcNormal = locked_rect_;
	rcNormal.left -= 2;
	rcNormal.right += 2;
	rcNormal.top -= 2;
	rcNormal.bottom += 2;

	rcNormal.left = Math::maxVal(0, rcNormal.left);
	rcNormal.top = Math::maxVal(0, rcNormal.top);
	rcNormal.right = Math::minVal(config_.xVertexCount - 1, rcNormal.right);
	rcNormal.bottom = Math::minVal(config_.zVertexCount - 1, rcNormal.bottom);

	for (int j = rcNormal.top; j < rcNormal.bottom; ++j)
	{
		for (int i = rcNormal.left; i < rcNormal.right; ++i)
		{
			Vector3 a = getPosition(i - 1, j + 0);
			Vector3 b = getPosition(i + 0, j - 1);
			Vector3 c = getPosition(i + 1, j + 0);
			Vector3 d = getPosition(i + 0, j + 1);
			Vector3 p = getPosition(i + 0, j + 0);

			Vector3 L = a - p, T = b - p, R = c - p, B = d - p;

			Vector3 N = Vector3::ZERO;
			float len_L = L.length(), len_T = T.length();
			float len_R = R.length(), len_B = B.length();

			if (len_L > 0.01f && len_T > 0.01f)
				N += L.crossProduct(T);

			if (len_T > 0.01f && len_R > 0.01f)
				N += T.crossProduct(R);

			if (len_R > 0.01f && len_B > 0.01f)
				N += R.crossProduct(B);

			if (len_B > 0.01f && len_L > 0.01f)
				N += B.crossProduct(L);

			N.normalise();

			int curr_idx = j * config_.xVertexCount + i;

			normals_[curr_idx] = N.x;
			normals_[curr_idx + 1] = N.y;
			normals_[curr_idx + 2] = N.z;
		}
	}

	// update sections
	for (int i = 0; i < sections_.size(); ++i)
	{
		sections_[i]->notifyUnlockHeight();
	}

	// update bound
	index = 0;
	for (int j = locked_rect_.top; j <= locked_rect_.bottom; ++j)
	{
		for (int i = locked_rect_.left; i <= locked_rect_.right; ++i)
		{
			float h = locked_data_[index++];
			bound_.getMinimum().y = Math::minVal(bound_.getMinimum().y, h);
			bound_.getMaximum().y = Math::maxVal(bound_.getMaximum().y, h);
		}
	}

	ENN_SAFE_FREE(locked_data_);
}


bool Terrain::isLockedHeight() 
{
	return locked_data_ ? true : false;
}

const IntRect& Terrain::getLockedHeightRect()
{
	return locked_rect_;
}


const float* Terrain::getLockedHeightData()
{
	return locked_data_;
}

float* Terrain::lockWeightMap(const IntRect& rect)
{
	ENN_ASSERT(!isLockedWeightMap());

	int w = rect.right - rect.left + 1;
	int h = rect.bottom - rect.top + 1;

	locked_weight_map_data_ = ENN_MALLOC_ARRAY_T(float, w * h);

	ENN_ASSERT(w > 0 && h > 0);

	int index = 0;
	for (int j = rect.top; j <= rect.bottom; ++j)
	{
		for (int i = rect.left; i <= rect.right; ++i)
		{
			locked_weight_map_data_[index++] = 0;
		}
	}

	locked_weight_map_rect_ = rect;
	return locked_weight_map_data_;
}

void Terrain::unlockWeightMap(int layer)
{
	ENN_ASSERT(isLockedWeightMap());

	int index = 0;
	for (int j = locked_weight_map_rect_.top; j <= locked_weight_map_rect_.bottom; ++j)
	{
		for (int i = locked_weight_map_rect_.left; i <= locked_weight_map_rect_.right; ++i)
		{
			float weight = locked_weight_map_data_[index++];

			int xSection = i / kWeightMapSize;
			int zSection = j / kWeightMapSize;

			TerrainSection * section = getSection(xSection, zSection);
			int layer0 = section->getLayer(0);
			int layer1 = section->getLayer(1);
			int layer2 = section->getLayer(2);
			int layer3 = section->getLayer(3);

			Color2 c = weights_[j * config_.zWeightMapSize + i];
			Color c4;

			c4.a = c.a / 255.0f;
			c4.r = c.r / 255.0f;
			c4.g = c.g / 255.0f;
			c4.b = c.b / 255.0f;

			if (layer == layer0)
				c4.a += weight;
			else if (layer == layer1)
				c4.r += weight;
			else if (layer == layer2)
				c4.g += weight;
			else if (layer == layer3)
				c4.b += weight;

			else
			{
				if (layer0 == -1)
				{
					c4.a += weight;
					section->setLayer(0, layer);
				}
				else if (layer1 == -1)
				{
					c4.r += weight;
					section->setLayer(1, layer);
				}
				else if (layer2 == -1)
				{
					c4.g += weight;
					section->setLayer(2, layer);
				}
				else if (layer3 == -1)
				{
					c4.b += weight;
					section->setLayer(3, layer);
				}
			}

			c4.normalize();

			c.r = unsigned char(c4.r * 255);
			c.g = unsigned char(c4.g * 255);
			c.b = unsigned char(c4.b * 255);
			c.a = unsigned char(c4.a * 255);

			weights_[j * config_.xWeightMapSize + i] = c;
		}
	}

	// update weight map
	for (int i = 0; i < sections_.size(); ++i)
	{
		TerrainSection* section = sections_[i];

		int xtile = Terrain::kWeightMapSize;
		int ztile = Terrain::kWeightMapSize;

		int x = section->getSectionX();
		int z = section->getSectionZ();

		IntRect myRect;

		myRect.left = x * xtile;
		myRect.top = z * ztile;
		myRect.right = x * xtile + xtile;
		myRect.bottom = z * ztile + ztile;

		if (locked_weight_map_rect_.left > myRect.right ||
			locked_weight_map_rect_.right < myRect.left ||
			locked_weight_map_rect_.top > myRect.bottom ||
			locked_weight_map_rect_.bottom < myRect.top)
		{
			continue;
		}

		D3D11TextureObj* weightMap = getWeightMap(x, z);

		D3D11_MAPPED_SUBRESOURCE map_res = g_render_device_->mapResource(weightMap->getResourceProxy(), 
			0, D3D11_MAP_WRITE_DISCARD, 0);

		Color2 * data = weights_ + z * kWeightMapSize * config_.xWeightMapSize + x * kWeightMapSize;
		int* dest = (int*)map_res.pData;

		for (int k = 0; k < kWeightMapSize; ++k)
		{
			for (int p = 0; p < kWeightMapSize; ++p)
			{
				dest[p] = M_RGBA(data[p].r, data[p].g, data[p].b, data[p].a);

				dest += kWeightMapSize;
				data += config_.xWeightMapSize;
			}
		}


		g_render_device_->unMapResource(weightMap->getResourceProxy(), 0);
	}

	ENN_SAFE_FREE(locked_weight_map_data_);
}

bool Terrain::isLockedWeightMap()
{
	return locked_weight_map_data_ != 0;
}

const IntRect& Terrain::getLockedWeightMapRect()
{
	return locked_weight_map_rect_;
}

const float* Terrain::getLockedWeightMapData()
{
	return locked_weight_map_data_;
}

D3D11TextureObj* Terrain::getDetailMap(int layer)
{
	if (layer >= 0 && layer < kMaxLayers)
		return detail_maps_[layer];
	else
		return default_detail_map_;
}

D3D11TextureObj* Terrain::getNormalMap(int layer)
{
	if (layer >= 0 && layer < kMaxLayers)
		return normal_maps_[layer];
	else
		return default_normal_map_;
}

D3D11TextureObj* Terrain::getSpecularMap(int layer)
{
	if (layer >= 0 && layer < kMaxLayers)
		return specular_maps_[layer];
	else
		return default_specular_map_;
}

Vector3 Terrain::_getPosition(int x, int z)
{
	x = Math::maxVal(0, x);
	z = Math::maxVal(0, z);

	x = Math::minVal(x, config_.xVertexCount - 1);
	z = Math::minVal(z, config_.zVertexCount - 1);

	return getPosition(x, z);
}

float Terrain::_getHeight(int x, int z)
{
	x = Math::maxVal(0, x);
	z = Math::maxVal(0, z);

	x = Math::minVal(x, config_.xVertexCount - 1);
	z = Math::minVal(z, config_.zVertexCount - 1);

	return getHeight(x, z);
}

Vector3 Terrain::_getNormal(int x, int z)
{
	x = Math::maxVal(0, x);
	z = Math::maxVal(0, z);

	x = Math::minVal(x, config_.xVertexCount - 1);
	z = Math::minVal(z, config_.zVertexCount - 1);

	return getNormal(x, z);
}

void Terrain::create(const Config& config)
{
	ENN_ASSERT(!inited_);
	config_ = config;

	int x = config.xVertexCount - 1;
	int z = config.zVertexCount - 1;
	int k = kSectionVertexSize - 1;

	if ((x % k) != 0)
		config_.xVertexCount = (x / k + 1) * k + 1;

	if ((z % k) != 0)
		config_.zVertexCount = (z / k + 1) * k + 1;

	config_.iVertexCount = config_.xVertexCount * config_.zVertexCount;
	config_.xSectionCount = config_.xVertexCount / (kSectionVertexSize - 1);
	config_.zSectionCount = config_.zVertexCount / (kSectionVertexSize - 1);

	config_.iSectionCount = config_.xSectionCount * config_.zSectionCount;

	config_.xSectionSize = config_.xSize / config_.xSectionCount;
	config_.zSectionSize = config_.zSize / config_.zSectionCount;

	heights_ = ENN_MALLOC_ARRAY_T(float, config_.iVertexCount);
	for (int i = 0; i < config_.iVertexCount; ++i)
	{
		heights_[i] = 0;
	}

	normals_ = ENN_MALLOC_ARRAY_T(Vector3, config_.iVertexCount);
	for (int i = 0; i < config_.iVertexCount; ++i)
	{
		normals_[i].x = 0.5f;
		normals_[i].y = 1.0f;
		normals_[i].z = 0.5f;
	}

	config_.xWeightMapSize = Terrain::kWeightMapSize * config_.xSectionCount;
	config_.zWeightMapSize = Terrain::kWeightMapSize * config_.zSectionCount;
	config_.iWeightMapSize = config_.xWeightMapSize * config_.zWeightMapSize;

	weights_ = ENN_MALLOC_ARRAY_T(Color2, config_.iWeightMapSize);
	for (int i = 0; i < config_.iWeightMapSize; ++i)
	{
		weights_[i] = Color2(0, 0, 0, 255);
	}

	bound_.setExtents(Vector3(0, 0, 0), Vector3(config_.xSize, 0, config_.zSize));

	init();
	inited_ = true;
}

void Terrain::load(const char* filename)
{
	ENN_ASSERT(!inited_);

	MemoryBuffer* file_buffer = 0;
	g_file_system_->getFileData(filename, file_buffer);

	ENN_ASSERT(file_buffer);
	int magic, version;

	int* sectionLayers = 0;

	file_buffer->ReadBin(magic);
	ENN_ASSERT(magic == K_Terrain_Magic);

	file_buffer->ReadBin(version);

	if (version == 0)
	{
		int count = 0;

		Terrain_ReadConfig(&config_, file_buffer, version);

		// layers
		file_buffer->Read(layers_, sizeof(Layer) * kMaxLayers);

		// bound
		Vector3 min_pos, max_pos;
		file_buffer->ReadBin(min_pos);
		file_buffer->ReadBin(max_pos);

		bound_.setExtents(min_pos, max_pos);

		// height data
		heights_ = ENN_MALLOC_ARRAY_T(float, config_.iVertexCount);
		file_buffer->Read(heights_, sizeof(float) * config_.iVertexCount);

		// normal data
		Color2* color_normals = ENN_MALLOC_ARRAY_T(Color2, config_.iVertexCount);
		normals_ = ENN_MALLOC_ARRAY_T(Vector3, config_.iVertexCount);

		file_buffer->Read(color_normals, sizeof(Color2)* config_.iVertexCount);

		for (int i = 0; i < config_.iVertexCount; ++i)
		{
			normals_[i] = toFloatNormal(color_normals[i]);
		}

		// weight data
		weights_ = ENN_MALLOC_ARRAY_T(Color2, config_.iWeightMapSize);
		file_buffer->Read(weights_, sizeof(Color2) * config_.iWeightMapSize);

		sectionLayers = ENN_MALLOC_ARRAY_T(int, config_.iSectionCount * kMaxBlendLayers);
		file_buffer->Read(sectionLayers, sizeof(int) * config_.iSectionCount * kMaxBlendLayers);
	}
	else
	{
		ENN_ASSERT(0);
	}

	init();

	if (sectionLayers)
	{
		int index = 0;
		for (int j = 0; j < config_.zSectionCount; ++j)
		{
			for (int i = 0; i < config_.xSectionCount; ++i)
			{
				TerrainSection* section = getSection(i, j);

				for (int l = 0; l < kMaxBlendLayers; ++l)
				{
					section->setLayer(l, sectionLayers[index * kMaxBlendLayers + l]);
				}

				++index;
			}
		}
	}

	ENN_FREE(sectionLayers);
	inited_ = true;
}

void Terrain::init()
{
	// create xy stream
	uint32 xy_stream_size = 8 * kSectionVertexSize * kSectionVertexSize;
	float* xy_buffer = (float*)ENN_MALLOC(xy_stream_size);
	float* temp_buffer = xy_buffer;

	float w = config_.xSize / config_.xSectionCount;
	float h = config_.zSize / config_.zSectionCount;
	for (int j = 0; j < kSectionVertexSize; ++j)
	{
		for (int i = 0; i < kSectionVertexSize; ++i)
		{
			*temp_buffer++ = i / (float)(kSectionVertexSize - 1) * w;
			*temp_buffer++ = j / (float)(kSectionVertexSize - 1) * h;
		}
	}

	D3D11BufferDesc xy_desc;

	xy_desc.setDefaultVertexBuffer(xy_stream_size, false);
	D3D11_SUBRESOURCE_DATA data;

	data.pSysMem = xy_buffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	xy_stream_ = g_render_device_->createVertexBuffer(xy_desc, &data);

	// create sections
	sections_.resize(config_.xSectionCount * config_.zSectionCount);

	int index = 0;
	for (int j = 0; j < config_.zSectionCount; ++j)
	{
		for (int i = 0; i < config_.xSectionCount; ++i)
		{
			sections_[index] = g_scene_graph_->createSceneNode<TerrainSection>();
			sections_[index]->create(this, i, j);

			g_scene_graph_->getRootNode()->addChild(sections_[index]);
			++index;
		}
	}

	weight_maps_.resize(config_.iSectionCount);

	index = 0;

	for (int j = 0; j < config_.zSectionCount; ++j)
	{
		for (int i = 0; i < config_.xSectionCount; ++i)
		{
			D3D11TextureObj* tex = ENN_NEW D3D11TextureObj();

			TexObjLoadRawParams params;
			params.data_ = 0;
			params.width_ = kWeightMapSize;
			params.height_ = kWeightMapSize;
			params.format_ = DXGI_FORMAT_B8G8R8A8_UNORM;
			params.dynamic_ = true;
			bool ret = tex->loadRaw(params);

			weight_maps_[index++] = tex;
		}
	}

	for (int i = 0; i < sections_.size(); ++i)
	{
		TerrainSection* section = sections_[i];

		int x = section->getSectionX();
		int z = section->getSectionZ();

		D3D11TextureObj* tex = getWeightMap(x, z);
		D3D11_MAPPED_SUBRESOURCE map_res = g_render_device_->mapResource(tex->getResourceProxy(), 0, D3D11_MAP_WRITE_DISCARD, 0);
		int* dest = (int*)map_res.pData;
		Color2 * data = weights_ + z * kWeightMapSize * config_.xWeightMapSize + x * kWeightMapSize;

		for (int k = 0; k < kWeightMapSize; ++k)
		{
			for (int p = 0; p < kWeightMapSize; ++p)
			{
				dest[p] = M_RGBA(data[p].r, data[p].g, data[p].b, data[p].a);

				if (data[p].r != 0)
				{
					int c = 1;
				}
			}
			
			dest += kWeightMapSize;
			data += config_.xWeightMapSize;
		}

		g_render_device_->unMapResource(tex->getResourceProxy(), 0);
	}

	// load default map
	LoadCommTex("terrain/TerrainDefault.png", default_detail_map_);
	LoadCommTex("terrain/TerrainDefault_n.dds", default_normal_map_);
	default_specular_map_ = g_tex_manager_->getDefaultTexture()->getTextureObj();

	for (int i = 0; i < kMaxLayers; ++i)
	{
		detail_maps_[i] = default_detail_map_;
		normal_maps_[i] = default_normal_map_;
		specular_maps_[i] = default_specular_map_;
	}


	String prefix = "terrain/decals/";
	for (int i = 0; i < kMaxLayers; ++i)
	{
		if (!String(layers_[i].detail).empty())
		{
			detail_maps_[i] = ENN_NEW D3D11TextureObj();
			detail_maps_[i]->loadFile(prefix + layers_[i].detail);
		}

		if (!String(layers_[i].normal).empty())
		{
			normal_maps_[i] = ENN_NEW D3D11TextureObj();
			normal_maps_[i]->loadFile(prefix + layers_[i].normal);
		}

		if (!String(layers_[i].specular).empty())
		{
			specular_maps_[i] = ENN_NEW D3D11TextureObj();
			specular_maps_[i]->loadFile(prefix + layers_[i].specular);
		}
	}
}

void Terrain::calcuNormals()
{
	ENN_ASSERT(!normals_);

	normals_ = ENN_MALLOC_ARRAY_T(Vector3, config_.iVertexCount);

	for (int j = 0; j < config_.zVertexCount; ++j)
	{
		for (int i = 0; i < config_.xVertexCount; ++i)
		{
			Vector3 a = _getPosition(i - 1, j + 0);
			Vector3 b = _getPosition(i + 0, j - 1);
			Vector3 c = _getPosition(i + 1, j + 0);
			Vector3 d = _getPosition(i + 0, j + 1);
			Vector3 p = _getPosition(i + 0, j + 0);

			Vector3 L = a - p, T = b - p, R = c - p, B = d - p;

			Vector3 N = Vector3::ZERO;
			float len_L = L.length(), len_T = T.length();
			float len_R = R.length(), len_B = B.length();

			if (len_L > 0.01f && len_T > 0.01f)
				N += L.crossProduct(T);

			if (len_T > 0.01f && len_R > 0.01f)
				N += T.crossProduct(R);

			if (len_R > 0.01f && len_B > 0.01f)
				N += R.crossProduct(B);

			if (len_B > 0.01f && len_L > 0.01f)
				N += B.crossProduct(L);

			N.normalise();

			int curr_idx = j * config_.xVertexCount + i;

			normals_[curr_idx] = N.x;
			normals_[curr_idx + 1] = N.y;
			normals_[curr_idx + 2] = N.z;
		}
	}
}

Vector3 Terrain::toFloatNormal(const Color2& clr)
{
	Vector3 normal;

	normal.x = (clr.r / 255.0f) * 2.0f - 1.0f;
	normal.y = (clr.g / 255.0f) * 2.0f - 1.0f;
	normal.z = (clr.b / 255.0f) * 2.0f - 1.0f;

	return normal;
}

}