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
// Time: 2014/03/14
// File: enn_material_manager.h
//

#pragma once

#include "enn_resource_manager.h"
#include "enn_material.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class MaterialResAutoCreator : public ResAutoCreatorBase
{
public:
	MaterialResAutoCreator();
	~MaterialResAutoCreator();

public:
	virtual bool buildResource(Resource* res);


protected:
	virtual bool prepareResourceImpl(Resource* res, MemoryBuffer* buff);
};

//////////////////////////////////////////////////////////////////////////
class MaterialManager : public ResourceManager
{
public:
	MaterialManager();
	~MaterialManager();

	virtual void init();

	void save(Material* mtr, const String& name);

	KHAOS_RESMAN_COMM_OVERRIDE(Material)

protected:
	virtual Resource* createImpl();
};

extern MaterialManager* g_mtr_mgr_;
}