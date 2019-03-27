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


#include <cassert>
#include "utils/gui.h"
#include "utils/string.h"
#include "core/conf.h"
#include "core/channel.h"
#include "core/sampleChannel.h"
#include "core/midiChannel.h"
#include "utils/log.h"
#include "gui/elems/basics/box.h"
#include "gui/elems/mainWindow/keyboard/keyboard.h"
#include "gui/elems/mainWindow/keyboard/channel.h"
#include "gui/elems/mainWindow/keyboard/channelButton.h"
#include "keyGrabber.h"
#include "config.h"
#include "mainWindow.h"


extern gdMainWindow *mainWin;


using std::string;
using namespace giada;


gdKeyGrabber::gdKeyGrabber(m::Channel* ch)
	: gdWindow(300, 126, "Key configuration"), ch(ch)
{
	set_modal();
	text   = new geBox(8, 8, 284, 80, "");
	clear  = new geButton(w()-88, text->y()+text->h()+8, 80, 20, "Clear");
	cancel = new geButton(clear->x()-88, clear->y(), 80, 20, "Close");
	end();

	clear->callback(cb_clear, (void*)this);
	cancel->callback(cb_cancel, (void*)this);

	updateText(ch->key);

	u::gui::setFavicon(this);
	show();
}


/* -------------------------------------------------------------------------- */


void gdKeyGrabber::cb_clear (Fl_Widget* w, void* p) { ((gdKeyGrabber*)p)->cb_clear(); }
void gdKeyGrabber::cb_cancel(Fl_Widget* w, void* p) { ((gdKeyGrabber*)p)->cb_cancel(); }


/* -------------------------------------------------------------------------- */


void gdKeyGrabber::cb_cancel()
{
	do_callback();
}


/* -------------------------------------------------------------------------- */


void gdKeyGrabber::cb_clear()
{
	updateText(0);
	setButtonLabel(0);
}


/* -------------------------------------------------------------------------- */


void gdKeyGrabber::setButtonLabel(int key)
{
	assert(false);
	//ch->guiChannel->mainButton->setKey(key);
	//ch->key = key;
}

/* -------------------------------------------------------------------------- */


void gdKeyGrabber::updateText(int key)
{
	string tmp = "Press a key.\n\nCurrent binding: ";
	if (key != 0)
		tmp += static_cast<char>(key);
	else
		tmp += "[none]";
	text->copy_label(tmp.c_str());
}


/* -------------------------------------------------------------------------- */


int gdKeyGrabber::handle(int e)
{
	int ret = Fl_Group::handle(e);
	switch(e) {
		case FL_KEYUP: {
			int x = Fl::event_key();
			if (strlen(Fl::event_text()) != 0
			    && x != FL_BackSpace
			    && x != FL_Enter
			    && x != FL_Delete
			    && x != FL_Tab
			    && x != FL_End
			    && x != ' ')
			{
				gu_log("set key '%c' (%d) for channel %d\n", x, x, ch->index);
				setButtonLabel(x);
				updateText(x);
				break;
			}
			else
				gu_log("invalid key\n");
		}
	}
	return(ret);
}
