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


#include "utils/string.h"
#include "core/midiChannel.h"
#include "midiChannelButton.h"


namespace giada {
namespace v
{
geMidiChannelButton::geMidiChannelButton(int x, int y, int w, int h, 
    const m::MidiChannel* ch)
: geChannelButton(x, y, w, h, ch)
{
    std::string l; 
    if (ch->name.empty())
        l = "-- MIDI --";
    else
        l = ch->name.c_str();

    if (ch->midiOut) 
        l += " (ch " + u::string::iToString(ch->midiOutChan + 1) + " out)";

    label(l.c_str());
}


/* -------------------------------------------------------------------------- */


int geMidiChannelButton::handle(int e)
{
	// Currently MIDI drag-n-drop does nothing.
	return geButton::handle(e);
}

}} // giada::v::