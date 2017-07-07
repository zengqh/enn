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
// Time: 2014/03/13
// File: enn_d3d11_texture_obj.h
//

#pragma once

#include "enn_d3d11_graphics_def.h"
#include "enn_d3d11_gpu_resource.h"
#include "enn_d3d11_render_target.h"

namespace enn
{

class D3D11ResourceProxy;
//////////////////////////////////////////////////////////////////////////
class D3D11TextureDesc : public AllocatedObject
{
public:
	virtual ~D3D11TextureDesc() {}

	static DXGI_FORMAT getTextureFormat(ElementFormat format);
};

class D3D11Texture1DDesc : public D3D11TextureDesc
{
public:
	D3D11Texture1DDesc();
	~D3D11Texture1DDesc();

	void setDefault();

	D3D11_TEXTURE1D_DESC& getRaw() { return raw_; }
	const D3D11_TEXTURE1D_DESC& getRaw() const { return raw_; }

public:
	D3D11_TEXTURE1D_DESC		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11Texture2DDesc : public D3D11TextureDesc
{
public:
	D3D11Texture2DDesc();
	~D3D11Texture2DDesc();

	void setDefault();
	void setDepthBuffer(uint32 width, uint32 height);
	void setColorBuffer(uint32 width, uint32 height);
	void setColorBuffer(uint32 width, uint32 height, DXGI_FORMAT format, bool dynamic = false);

	void setColorRTTBuffer(uint32 width, uint32 height, DXGI_FORMAT format);
	void setRTTDepthBuffer(uint32 width, uint32 height, DXGI_FORMAT format);

	void setColorRTTBuffer(uint32 widht, uint32 height, ElementFormat format);
	void setRTTDepthBuffer(uint32 width, uint32 height, ElementFormat format);

	D3D11_TEXTURE2D_DESC& getRaw() { return raw_; }
	const D3D11_TEXTURE2D_DESC& getRaw() const { return raw_; }

public:
	D3D11_TEXTURE2D_DESC		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11Texture3DDesc : public D3D11TextureDesc
{
public:
	D3D11Texture3DDesc();
	~D3D11Texture3DDesc();

	void setDefault();

	D3D11_TEXTURE3D_DESC& getRaw() { return raw_; }
	const D3D11_TEXTURE3D_DESC& getRaw() const { return raw_; }

public:
	D3D11_TEXTURE3D_DESC		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11Texture1D : public D3D11Resource
{
public:
	D3D11Texture1D(ID3D11Texture1D* raw);
	~D3D11Texture1D();

	virtual ResourceType getType() const { return RT_TEXTURE1D; }
	virtual ID3D11Resource* getResource() const { return raw_; }
	const D3D11_TEXTURE1D_DESC& getDesc() const { return desc_; }

	int getWidth() const { return (int)desc_.Width; }
	int getLevels() const { return (int)desc_.MipLevels; }

protected:
	ID3D11Texture1D*			raw_;
	D3D11_TEXTURE1D_DESC		desc_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11Texture2D : public D3D11Resource
{
public:
	D3D11Texture2D(ID3D11Texture2D* raw);
	~D3D11Texture2D();

	virtual ResourceType getType() const { return RT_TEXTURE2D; }
	virtual ID3D11Resource* getResource() const { return raw_; }
	const D3D11_TEXTURE2D_DESC& getDesc() const { return desc_; }

	int getWidth() const { return (int)desc_.Width; }
	int getHeight() const { return (int)desc_.Height; }
	int getLevels() const { return (int)desc_.MipLevels; }

protected:
	ID3D11Texture2D*			raw_;
	D3D11_TEXTURE2D_DESC		desc_;
};


//////////////////////////////////////////////////////////////////////////
class D3D11Texture3D : public D3D11Resource
{
public:
	D3D11Texture3D(ID3D11Texture3D* raw);
	~D3D11Texture3D();

	virtual ResourceType getType() const { return RT_TEXTURE3D; }
	virtual ID3D11Resource* getResource() const { return raw_; }
	const D3D11_TEXTURE3D_DESC& getDesc() const { return desc_; }

	int getWidth() const { return (int)desc_.Width; }
	int getHeight() const { return (int)desc_.Height; }
	int getDepth() const { return (int)desc_.Depth; }
	int getLevels() const { return (int)desc_.MipLevels; }

protected:
	ID3D11Texture3D*				raw_;
	D3D11_TEXTURE3D_DESC		desc_;
};

//////////////////////////////////////////////////////////////////////////
struct TexObjLoadParams
{
	TexObjLoadParams() : data_(0), data_size_(0), auto_generate_mipmap_(true), use_srgb_(true) {}

	void*		data_;
	int         data_size_;
	bool		auto_generate_mipmap_;
	bool		use_srgb_;
};

struct TexObjLoadRawParams
{
	TexObjLoadRawParams() : data_(0), dynamic_(false) {}

	void*					data_;
	int						width_;
	int						height_;
	DXGI_FORMAT				format_;
	bool					dynamic_;
};

struct TexObjRTTRawParams
{
	int						width_;
	int						height_;
	ElementFormat			format_;
};

class D3D11TextureObj : public AllocatedObject
{
public:
	D3D11TextureObj();
	~D3D11TextureObj();

public:
	bool load(const TexObjLoadParams& params);
	bool loadRaw(const TexObjLoadRawParams& params);
	bool loadFile(const String& name, bool use_srgb = true, bool auto_generate_mip = true);

	bool loadRTTColor(const TexObjRTTRawParams& params);
	bool loadRTTDepth(const TexObjRTTRawParams& params);

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

	D3D11ResourceProxy* getResourceProxy() const { return tex_proxy_; }

protected:
	D3D11ResourceProxy*			tex_proxy_;
	int							width_;
	int							height_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11RTT2DHelper : public AllocatedObject
{
public:
	D3D11RTT2DHelper();
	virtual ~D3D11RTT2DHelper();

public:
	void setDimension(uint32 width, uint32 height);
	void getDimension(uint32& widht, uint32& height);
	bool createColorBuffer(int idx, ElementFormat format);
	bool createDepthBuffer(ElementFormat format);

	void attachColorBuffer(int idx, D3D11TextureObj* color);
	void attachDepthBuffer(D3D11TextureObj* depth);
	void attachRawDepthBuffer(D3D11DepthStencilView* depth);

	D3D11RenderTarget* getRTT();

	D3D11TextureObj* getColorBuffer(int idx) const { return color_buffers_[idx]; }
	D3D11TextureObj* getDepthBuffer() const { return depth_buffer_; }
	
protected:
	void updateRTT();

protected:
	D3D11TextureObj* color_buffers_[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	D3D11TextureObj* depth_buffer_;

	D3D11RenderTarget* rtt_;

	uint32				w_, h_;

	bool				color_buffers_own_[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	bool				depth_buffers_own_;
};

}