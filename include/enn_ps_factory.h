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
// File: enn_ps_factory.h
//

#pragma once

#include "enn_ps_emitter.h"
#include "enn_ps_render.h"
#include "enn_ps_affector.h"

namespace enn
{

class PSFactory : public public AllocatedObject
{
	typedef void* (*CreatorType)();
	typedef unordered_map<String, CreatorType>::type CreatorMap;

public:
	PSFactory();
	virtual ~PSFactory();

	PSEmitter* createEmitter(const String& type);
	PSAffector* createAffector(const String& type);
	PSRender* createRender(const String& type);

protected:
	template <class T> static void* createCommObj() { return new T; }
	template <class T> void registerCreater();
	template <class T> T* createPSObj();
	void _init();


protected:
	CreatorMap	creator_map_;
};


//////////////////////////////////////////////////////////////////////////
extern PSFactory* g_ps_factory_;

}