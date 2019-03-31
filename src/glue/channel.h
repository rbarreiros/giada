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


#ifndef G_GLUE_CHANNEL_H
#define G_GLUE_CHANNEL_H


#include <string>
#include "core/types.h"


namespace giada {
namespace m
{
class Channel;
}
namespace c {
namespace channel 
{
/* addChannel
Adds an empty new channel to the stack. Returns the new channel. */

m::Channel* addChannel(size_t columnIndex, ChannelType type, int size);

/* loadChannel
Fills an existing channel with a wave. */

int loadChannel(size_t chanIndex, const std::string& fname);

/* deleteChannel
Removes a channel from Mixer. */

void deleteChannel(size_t chanIndex);

/* freeChannel
Unloads the sample from a sample channel. */

void freeChannel(size_t chanIndex);

/* cloneChannel
Makes an exact copy of Channel *ch. */

void cloneChannel(size_t chanIndex);

/* set*
Sets several channel properties. If gui == true the signal comes from a manual 
interaction on the GUI, otherwise it's a MIDI/Jack/external signal. */

void setArm(size_t chanIndex, bool value, bool gui=true);
void setInputMonitor(size_t chanIndex, bool value);
void kill(size_t chanIndex);
void setMute(size_t chanIndex, bool value, bool gui=true);
void setSolo(size_t chanIndex, bool value, bool gui=true);
void setVolume(size_t chanIndex, float v, bool gui=true, bool editor=false);
void setName(size_t chanIndex, const std::string& name);
void setPitch(size_t chanIndex, float val, bool gui=true);
void setPan(size_t chanIndex, float val, bool gui=true);
void setBoost(size_t chanIndex, float val, bool gui=true);

/* toggleReadingRecs
Handles the 'R' button. If gui == true the signal comes from an user interaction
on the GUI, otherwise it's a MIDI/Jack/external signal. */

void toggleReadingActions(size_t chanIndex, bool gui=true);
void startReadingActions(size_t chanIndex, bool gui=true);
void stopReadingActions(size_t chanIndex, bool gui=true);

}}}; // giada::c::channel::

#endif
