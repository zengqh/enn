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
// File: sample_frame.cpp

#include "sample_frame.h"


namespace enn
{

SampleFrame* g_sample_frame_ = 0;
//////////////////////////////////////////////////////////////////////////
SampleEventHandler::SampleEventHandler()
: camera_node_(0)
, trans_k_(0)
, rot_k_(0)
, delta_x_(0)
, delta_y_(0)
{
	mouse_pos_.x = mouse_pos_.y = 0;
}

SampleEventHandler::~SampleEventHandler()
{
	OIS::InputManager::destroyInputSystem(input_mgr_);
}

void SampleEventHandler::init(enn::CameraNode* cn, float transK, float rotK, HWND hwnd)
{
	camera_node_ = cn;
	trans_k_ = transK;
	rot_k_ = rotK;

	hwnd_ = hwnd;

	createOIS();
}

void SampleEventHandler::update()
{
	key_board_->capture();
	mouse_->capture();

	float elapsed_time = enn::g_time_system_->getElapsedTime();
	float units = elapsed_time * trans_k_;

	if (key_board_->isKeyDown(OIS::KC_W))
	{
		camera_node_->translate(enn::Vector3::NEGATIVE_UNIT_Z * units, enn::Node::TS_LOCAL);
	}
	else if (key_board_->isKeyDown(OIS::KC_S))
	{
		camera_node_->translate(enn::Vector3::UNIT_Z * units, enn::Node::TS_LOCAL);
	}

	if (key_board_->isKeyDown(OIS::KC_D))
	{
		camera_node_->translate(enn::Vector3::UNIT_X * units, enn::Node::TS_LOCAL);
	}
	else if (key_board_->isKeyDown(OIS::KC_A))
	{
		camera_node_->translate(enn::Vector3::NEGATIVE_UNIT_X * units, enn::Node::TS_LOCAL);
	}
}

bool SampleEventHandler::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool SampleEventHandler::mouseMoved(const OIS::MouseEvent &arg)
{
	if (arg.state.buttonDown(OIS::MB_Right))
	{
		float units = rot_k_;
		LONG  offx = -arg.state.X.rel;
		LONG  offy = -arg.state.Y.rel;

		float fPercentOfNew = 1.0f / 2;
		float fPercentOfOld = 1.0f - fPercentOfNew;

		delta_x_ = delta_x_ * fPercentOfOld + offx * units * fPercentOfNew;
		delta_y_ = delta_y_ * fPercentOfOld + offy * units * fPercentOfNew;

		camera_node_->yaw(delta_x_, enn::Node::TS_PARENT);
		camera_node_->pitch(delta_y_, enn::Node::TS_LOCAL);
	}

	return true;
}

bool SampleEventHandler::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

void SampleEventHandler::createOIS()
{
	OIS::ParamList pl;
	std::ostringstream wnd;
	wnd << (size_t)hwnd_;

	pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

	input_mgr_ = OIS::InputManager::createInputSystem(pl);
	input_mgr_->enableAddOnFactory(OIS::InputManager::AddOn_All);

	key_board_ = (OIS::Keyboard*)input_mgr_->createInputObject(OIS::OISKeyboard, true);
	key_board_->setEventCallback(this);

	mouse_ = (OIS::Mouse*)input_mgr_->createInputObject(OIS::OISMouse, true);
	mouse_->setEventCallback(this);
}

//////////////////////////////////////////////////////////////////////////
inline int getRectWidth(const RECT& rc)
{
	return rc.right - rc.left;
}

inline int getRectHeight(const RECT& rc)
{
	return rc.bottom - rc.top;
}


SampleFrame::SampleFrame()
{
	g_sample_frame_ = this;
}

SampleFrame::~SampleFrame()
{

}

void SampleFrame::create(const SampleCreateContext& ctx)
{
	RootSetting root_setting;

	root_setting.clear_clr_ = enn::Color::BLUE;
	root_setting.file_system_path_ = String(ctx.exe_path) + "../media";
	root_setting.num_threads_ = 2;
	root_setting.render_setting_.vsync_enable_ = false;
	root_setting.render_setting_.full_screen_ = false;
	root_setting.render_setting_.width_ = ctx.windowWidth;
	root_setting.render_setting_.height_ = ctx.windowHeight;
	root_setting.render_setting_.sample_count_ = 1;
	root_setting.render_setting_.sample_quality_ = 0;
	root_setting.render_setting_.window_handle_ = hwnd_ = ctx.handleWindow;

	ENN_NEW enn::Root();
	g_root_->init(root_setting);
	g_root_->start();

	onCreateScene();
	createSampleEventHandler();
}

void SampleFrame::destroy()
{
	onDestroyScene();
	g_root_->shutdown();
	ENN_SAFE_DELETE(g_root_);
}

bool SampleFrame::idle()
{
	if (!onUpdate())
	{
		return false;
	}

	g_root_->run();

	return true;
}

void SampleFrame::resize(int w, int h)
{
	g_render_wnd_->resize(w, h);
}

bool SampleFrame::onUpdate()
{
	evt_handler_->update();
	return true;
}

}