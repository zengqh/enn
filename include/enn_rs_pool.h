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
// Time: 2014/01/23
// File: enn_rs_pool.h

namespace enn
{

class RenderStatePool : public AllocatedObject
{
	typedef map<RasterizerStateDesc, RasterizerStateObject*>::type RSPool;
	typedef map<DepthStencilStateDesc, DepthStencilStateObject*>::type DSPool;
	typedef map<BlendStateDesc, BlendStateObject*>::type BSPool;
	typedef map<SamplerStateDesc, SamplerStateObject*>::type SSPool;

public:
	RenderStatePool();
	virtual ~RenderStatePool();

	virtual RasterizerStateObject* getRSObject(const RasterizerStateDesc& desc);
	virtual DepthStencilStateObject* getDSObject(const DepthStencilStateDesc& desc);
	virtual BlendStateObject* getBSObject(const BlendStateDesc& desc);
	virtual SamplerStateObject* getSSObject(const SamplerStateDesc& desc);

protected:
	virtual RasterizerStateObject* createRSObject(const RasterizerStateDesc& desc) = 0;
	virtual DepthStencilStateObject* createDSObject(const DepthStencilStateDesc& desc) = 0;
	virtual BlendStateObject* createBSObject(const BlendStateDesc& desc) = 0;
	virtual SamplerStateObject* createSSObject(const SamplerStateDesc& desc) = 0;

protected:
	RSPool	rs_pool_;
	DSPool	ds_pool_;
	BSPool	bs_pool_;
	SSPool	ss_pool_;
}
