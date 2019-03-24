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


#ifndef G_MIXER_HANDLER_H
#define G_MIXER_HANDLER_H


#include <string>
#include "types.h"




namespace giada {
namespace m 
{
class Channel;
class SampleChannel;

namespace mh
{
/* addChannel
Adds a new channel of type 'type' into the channels stack. */

Channel* addChannel(ChannelType type, size_t column);

/* loadChannel
Loads a new Wave inside a Sample Channel. */

int loadChannel(size_t chanIndex, const std::string& fname);

/* freeChannel
Unloads existing Wave from a Sample Channel. */

void freeChannel(size_t chanIndex);

/* deleteChannel
Completely removes a channel from the stack. */

void deleteChannel(size_t chanIndex);

void cloneChannel(size_t chanIndex);

/* getChannelByIndex
Returns channel with given index 'i'. */

Channel* getChannelByIndex(int i);

/* stopSequencer
Stops the sequencer, with special case if samplesStopOnSeqHalt is true. */

void stopSequencer();

void rewindSequencer();

/* updateSoloCount
Updates the number of solo-ed channels in mixer. */

void updateSoloCount();

/* loadPatch
Loads a path or a project (if isProject) into Mixer. If isProject, path must 
contain the address of the project folder. */

void readPatch();

/* startInputRec - record from line in
Creates a new empty wave in the first available channels. Returns false if
there are no available channels. */

bool startInputRec();

void stopInputRec();

/* uniqueSamplePath
Returns true if path 'p' is unique. Requires SampleChannel 'skip' in order
to skip check against itself. */

bool uniqueSamplePath(const SampleChannel* skip, const std::string& p);

/* hasLogicalSamples
True if 1 or more samples are logical (memory only, such as takes) */

bool hasLogicalSamples();

/* hasEditedSamples
True if 1 or more samples was edited via gEditor */

bool hasEditedSamples();

/* hasArmedSampleChannels
Tells whether Mixer has one or more Sample Channels armed for input recording. */

bool hasArmedSampleChannels();

/* hasRecordableSampleChannels
Tells whether Mixer has one or more recordable Sample Channels, that is: 
a) armed; b) empty (no Wave). */

bool hasRecordableSampleChannels();
}}}  // giada::m::mh::


#endif
