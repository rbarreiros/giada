/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */


#include <FL/Fl.H>
#include "core/const.h"
#include "core/channel.h"
#include "core/graphics.h"
#include "core/pluginHost.h"
#include "utils/gui.h"
#include "glue/channel.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/pluginList.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/dial.h"
#include "gui/elems/basics/statusButton.h"
#include "column.h"
#include "channelStatus.h"
#include "channelButton.h"
#include "channel.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace v
{
geChannel::geChannel(int X, int Y, int W, int H, const m::Channel* ch)
: Fl_Group(X, Y, W, H, nullptr),
  ch      (ch)
{
}


/* -------------------------------------------------------------------------- */


void geChannel::cb_arm(Fl_Widget* v, void* p) { ((geChannel*)p)->cb_arm(); }
void geChannel::cb_mute(Fl_Widget* v, void* p) { ((geChannel*)p)->cb_mute(); }
void geChannel::cb_solo(Fl_Widget* v, void* p) { ((geChannel*)p)->cb_solo(); }
void geChannel::cb_changeVol(Fl_Widget* v, void* p) { ((geChannel*)p)->cb_changeVol(); }
#ifdef WITH_VST
void geChannel::cb_openFxWindow(Fl_Widget* v, void* p) { ((geChannel*)p)->cb_openFxWindow(); }
#endif


/* -------------------------------------------------------------------------- */


void geChannel::cb_arm()
{
	c::channel::setArm(ch->index, arm->value());
}


/* -------------------------------------------------------------------------- */


void geChannel::cb_mute()
{
	c::channel::setMute(ch->index, mute->value());
}


/* -------------------------------------------------------------------------- */


void geChannel::cb_solo()
{
	c::channel::setSolo(ch->index, solo->value());
}


/* -------------------------------------------------------------------------- */


void geChannel::cb_changeVol()
{
	c::channel::setVolume(ch->index, vol->value());
}


/* -------------------------------------------------------------------------- */


#ifdef WITH_VST
void geChannel::cb_openFxWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdPluginList({m::pluginHost::StackType::CHANNEL, ch->index}), WID_FX_LIST);
}
#endif


/* -------------------------------------------------------------------------- */



int geChannel::getColumnIndex()
{
	return static_cast<geColumn*>(parent())->getIndex();
}


/* -------------------------------------------------------------------------- */


void geChannel::blink()
{
	if (u::gui::shouldBlink())
		mainButton->setPlayMode();
	else
		mainButton->setDefaultMode();
}


/* -------------------------------------------------------------------------- */


void geChannel::setColorsByStatus()
{
	switch (ch->status) {
		case ChannelStatus::OFF:
		case ChannelStatus::EMPTY:
			mainButton->setDefaultMode();
			button->imgOn  = channelPlay_xpm;
			button->imgOff = channelStop_xpm;
			button->redraw();
			break;
		case ChannelStatus::PLAY:
			mainButton->setPlayMode();
			if (!button->value()) { // If not manually pressed (it would interfere)
				button->imgOn  = channelStop_xpm;
				button->imgOff = channelPlay_xpm;
				button->redraw();			
			}
			break;
		case ChannelStatus::WAIT:
			blink();
			break;
		case ChannelStatus::ENDING:
			mainButton->setEndingMode();
			break;
		default: break;
	}

	switch (ch->recStatus) {
		case ChannelStatus::WAIT:
			blink();
			break;
		case ChannelStatus::ENDING:
			mainButton->setEndingMode();
			break;
		default: break;
	}
}


/* -------------------------------------------------------------------------- */


void geChannel::packWidgets()
{
	/* Count visible widgets and resize mainButton according to how many widgets
	are visible. */

	int visibles = 0;
	for (int i=0; i<children(); i++) {
		child(i)->size(MIN_ELEM_W, child(i)->h());  // also normalize widths
		if (child(i)->visible())
			visibles++;
	}
	mainButton->size(w() - ((visibles - 1) * (MIN_ELEM_W + G_GUI_INNER_MARGIN)),   // -1: exclude itself
		mainButton->h());

	/* Reposition everything else */

	for (int i=1, p=0; i<children(); i++) {
		if (!child(i)->visible())
			continue;
		for (int k=i-1; k>=0; k--) // Get the first visible item prior to i
			if (child(k)->visible()) {
				p = k;
				break;
			}
		child(i)->position(child(p)->x() + child(p)->w() + G_GUI_INNER_MARGIN, child(i)->y());
	}

	init_sizes(); // Resets the internal array of widget sizes and positions
}


/* -------------------------------------------------------------------------- */


bool geChannel::handleKey(int e)
{
	if (Fl::event_key() != ch->key) 
		return false;

	if (e == FL_KEYDOWN && !button->value()) {  // Key not already pressed
		button->take_focus();                   // Move focus to this button
		button->value(1);
		return true;
	}

	if (e == FL_KEYUP) {
		button->value(0);
		return true;
	}
	
	return false;
}


/* -------------------------------------------------------------------------- */


void geChannel::changeSize(int H)
{
	size(w(), H);
	
	int Y = y() + (H / 2 - (G_GUI_UNIT / 2));

	button->resize(x(), Y, w(), G_GUI_UNIT);
	arm->resize(x(), Y, w(), G_GUI_UNIT);   
	mainButton->resize(x(), y(), w(), H);
	mute->resize(x(), Y, w(), G_GUI_UNIT);
	solo->resize(x(), Y, w(), G_GUI_UNIT);
	vol->resize(x(), Y, w(), G_GUI_UNIT);
#ifdef WITH_VST
	fx->resize(x(), Y, w(), G_GUI_UNIT);
#endif
}


/* -------------------------------------------------------------------------- */


int geChannel::getSize()
{
	return h();
}

}} // giada::v::