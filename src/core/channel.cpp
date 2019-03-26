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
#include <cstring>
#include "utils/log.h"
#include "gui/elems/mainWindow/keyboard/channel.h"
#include "const.h"
#include "channelManager.h"
#include "pluginManager.h"
#include "plugin.h"
#include "kernelMidi.h"
#include "patch.h"
#include "clock.h"
#include "wave.h"
#include "mixer.h"
#include "mixerHandler.h"
#include "recorderHandler.h"
#include "conf.h"
#include "patch.h"
#include "waveFx.h"
#include "midiMapConf.h"
#include "channel.h"


namespace giada {
namespace m 
{
Channel::Channel(ChannelType type, ChannelStatus status, int bufferSize, size_t column)
: guiChannel     (nullptr),
  type           (type),
  status         (status),
  recStatus      (ChannelStatus::OFF),
  column         (column),
  previewMode    (PreviewMode::NONE),
  pan            (0.5f),
  volume         (G_DEFAULT_VOL),
  armed          (false),
  key            (0),
  mute           (false),
  solo           (false),
  volume_i       (1.0f),
  volume_d       (0.0f),
  hasActions     (false),
  readActions    (false),
  midiIn         (true),
  midiInKeyPress (0x0),
  midiInKeyRel   (0x0),
  midiInKill     (0x0),
  midiInArm      (0x0),
  midiInVolume   (0x0),
  midiInMute     (0x0),
  midiInSolo     (0x0),
  midiInFilter   (-1),
  midiOutL       (false),
  midiOutLplaying(0x0),
  midiOutLmute   (0x0),
  midiOutLsolo   (0x0)
{
	buffer.alloc(bufferSize, G_MAX_IO_CHANS);
}


/* -------------------------------------------------------------------------- */


Channel::Channel(const Channel& o)
: type           (o.type),
  status         (o.status),
  recStatus      (o.recStatus),
  column         (o.column),
  index          (o.index),
  previewMode    (o.previewMode),
  pan            (o.pan.load()),
  volume         (o.volume.load()),
  armed          (o.armed.load()),
  key            (o.key),
  mute           (o.mute.load()),
  solo           (o.solo.load()),
  volume_i       (o.volume_i),
  volume_d       (o.volume_d),
  hasActions     (o.hasActions),
  readActions    (o.readActions),
  midiIn         (o.midiIn),
  midiInKeyPress (o.midiInKeyPress),
  midiInKeyRel   (o.midiInKeyRel),
  midiInKill     (o.midiInKill),
  midiInArm      (o.midiInArm),
  midiInVolume   (o.midiInVolume),
  midiInMute     (o.midiInMute),
  midiInSolo     (o.midiInSolo),
  midiInFilter   (o.midiInFilter),
  midiOutL       (o.midiOutL),
  midiOutLplaying(o.midiOutLplaying),
  midiOutLmute   (o.midiOutLmute),
  midiOutLsolo   (o.midiOutLsolo)
{
	buffer.alloc(o.buffer.countFrames(), G_MAX_IO_CHANS);

#ifdef WITH_VST

	for (const std::unique_ptr<Plugin>& plugin : o.plugins)
        plugins.push_back(pluginManager::makePlugin(*plugin.get()));

#endif

	// hasActions = recorderHandler::cloneActions(o.index, index);	
}


/* -------------------------------------------------------------------------- */


bool Channel::isPlaying() const
{
	return status == ChannelStatus::PLAY || status == ChannelStatus::ENDING;
}


/* -------------------------------------------------------------------------- */


void Channel::writePatch(int i, bool isProject)
{
	channelManager::writePatch(this, isProject);
}


/* -------------------------------------------------------------------------- */


void Channel::readPatch(const std::string& path, const patch::channel_t& pch)
{
	channelManager::readPatch(this, pch);
}


/* -------------------------------------------------------------------------- */


void Channel::sendMidiLmute()
{
	if (!midiOutL || midiOutLmute == 0x0)
		return;
	if (mute.load() == true)
		kernelMidi::sendMidiLightning(midiOutLmute, midimap::muteOn);
	else
		kernelMidi::sendMidiLightning(midiOutLmute, midimap::muteOff);
}


/* -------------------------------------------------------------------------- */


void Channel::sendMidiLsolo()
{
	if (!midiOutL || midiOutLsolo == 0x0)
		return;
	if (solo.load() == true)
		kernelMidi::sendMidiLightning(midiOutLsolo, midimap::soloOn);
	else
		kernelMidi::sendMidiLightning(midiOutLsolo, midimap::soloOff);
}


/* -------------------------------------------------------------------------- */


void Channel::sendMidiLstatus()
{
	if (!midiOutL || midiOutLplaying == 0x0)
		return;
	switch (status) {
		case ChannelStatus::OFF:
			kernelMidi::sendMidiLightning(midiOutLplaying, midimap::stopped);
			break;
		case ChannelStatus::WAIT:
			kernelMidi::sendMidiLightning(midiOutLplaying, midimap::waiting);
			break;
		case ChannelStatus::ENDING:
			kernelMidi::sendMidiLightning(midiOutLplaying, midimap::stopping);
			break;
		case ChannelStatus::PLAY:
			if ((mixer::isChannelAudible(this) && !mute.load()) || 
				!midimap::isDefined(midimap::playing_inaudible))
				kernelMidi::sendMidiLightning(midiOutLplaying, midimap::playing);
			else
				kernelMidi::sendMidiLightning(midiOutLplaying, midimap::playing_inaudible);
			break;
		default:
			break;
	}
}


/* -------------------------------------------------------------------------- */


bool Channel::isMidiInAllowed(int c) const
{
	return midiInFilter == -1 || midiInFilter == c;
}


/* -------------------------------------------------------------------------- */


void Channel::setPan(float v)
{
	if (v > 1.0f)
		pan.store(1.0f);
	else 
	if (v < 0.0f)
		pan.store(0.0f);
	else
		pan.store(v);
}


float Channel::getPan() const
{
	return pan.load();
}


/* -------------------------------------------------------------------------- */


float Channel::calcPanning(int ch) const
{	
	float p = pan.load();
	if (p  == 0.5f) // center: nothing to do
		return 1.0;
	if (ch == 0)
		return 1.0 - p;
	else  // channel 1
		return p; 
}


/* -------------------------------------------------------------------------- */


void Channel::calcVolumeEnvelope()
{
	volume_i += volume_d;
	if (volume_i < 0.0f)
		volume_i = 0.0f;
	else
	if (volume_i > 1.0f)
		volume_i = 1.0f;	
}


bool Channel::isPreview() const
{
	return previewMode != PreviewMode::NONE;
}


/* -------------------------------------------------------------------------- */


bool Channel::isReadingActions() const
{
	return hasActions && readActions;
}

}} // giada::m::
