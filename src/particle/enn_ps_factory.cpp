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
// Time: 2015/12/30
// File: enn_ps_factory.cpp
//

#include "enn_ps_factory.h"


namespace enn
{

PSFactory* g_ps_factory_ = 0;

PSFactory::PSFactory()
{
	g_ps_factory_ = this;
}

PSFactory::~PSFactory()
{

}

PSEmitter* PSFactory::createEmitter(const String& type)
{
	return createPSObj<PSEmitter>();
}

PSAffector* PSFactory::createAffector(const String& type)
{
	return createPSObj<PSAffector>();
}

PSRender* PSFactory::createRender(const String& type)
{
	return createPSObj<PSRender>();
}

template <class T> void PSFactory::registerCreater()
{
	creator_map_.insert(CreatorMap::value_type(T::getType(), ENN_NEW T));
}

void PSFactory::_init()
{
	registerCreater<PSRenderBillboard>();
	registerCreater<PSRenderDecal>();
	registerCreater<PSRenderRibbon>();

	registerCreater<PSAffectorForce>();
	registerCreater<PSAffectorRot>();
	registerCreater<PSAffectorRot2>();

	registerCreater<PSEmitterBox>();
	registerCreater<PSEmitterClinder>();
	registerCreater<PSEmitterEllipsoid>();
	registerCreater<PSEmitterPoint>();
}

template <class T> T* PSFactory::createPSObj()
{
	CreatorMap::iterator it = creator_map_.find(type);
	if (it != creator_map_.end())
	{
		return static_cast<T*>(it->second);
	}

	ENN_ASSERT(0);

	return 0;
}

}