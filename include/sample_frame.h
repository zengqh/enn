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
// Time: 2014/06/16
// File: sample_frame.h

#pragma once

#include "enn_root.h"

namespace enn
{

class SampleEventHandler : public OIS::KeyListener, public OIS::MouseListener, public enn::AllocatedObject
{
public:
	SampleEventHandler();
	virtual ~SampleEventHandler();

public:
	void init(enn::CameraNode* cn, float transK, float rotK, HWND hwnd);
	void update();

public:
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg) { return true; }

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

protected:
	void createOIS();

protected:
	enn::CameraNode*		camera_node_;
	float					trans_k_;
	float					rot_k_;
	POINT					mouse_pos_;
	float					delta_x_;
	float					delta_y_;

	OIS::InputManager*		input_mgr_;
	OIS::Keyboard*			key_board_;
	OIS::Mouse*				mouse_;
	HWND					hwnd_;
};

//////////////////////////////////////////////////////////////////////////
struct SampleCreateContext
{
	void*				handleWindow;
	int					windowWidth;
	int					windowHeight;
	const char*			exe_path;
};

class SampleFrame : public AllocatedObject
{
public:
	SampleFrame();
	virtual ~SampleFrame();

public:
	virtual void create(const SampleCreateContext& ctx);
	virtual void destroy();
	virtual bool idle();
	virtual void resize(int w, int h);

protected:
	virtual bool onCreateScene() { return true; }
	virtual void onDestroyScene() {}
	virtual bool onUpdate();

protected:
	virtual void createSampleEventHandler() = 0;

protected:
	void*							hwnd_;
	int								w_;
	int								h_;

	SampleEventHandler*				evt_handler_;
};

}
