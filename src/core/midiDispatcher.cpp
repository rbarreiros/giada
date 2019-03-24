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


#include <vector>
#include "../glue/plugin.h"
#include "../glue/io.h"
#include "../glue/channel.h"
#include "../glue/transport.h"
#include "../glue/main.h"
#include "../utils/log.h"
#include "channel.h"
#include "sampleChannel.h"
#include "midiChannel.h"
#include "conf.h"
#include "mixer.h"
#include "pluginHost.h"
#include "plugin.h"
#include "midiDispatcher.h"


using std::vector;


namespace giada {
namespace m {
namespace midiDispatcher
{
namespace
{
/* cb_midiLearn, cb_data_
Callback prepared by the gdMidiGrabber window and called by midiDispatcher. It 
contains things to do once the midi message has been stored. */

cb_midiLearn* cb_learn_ = nullptr;
void* cb_data_ = nullptr;	

std::function<void()> signalCb_ = nullptr;


/* -------------------------------------------------------------------------- */


#ifdef WITH_VST

void processPlugins_(Channel* ch, const MidiEvent& midiEvent)
{
	uint32_t pure = midiEvent.getRawNoVelocity();

	/* Plugins' parameters layout reflects the structure of the matrix
	Channel::midiInPlugins. It is safe to assume then that i (i.e. Plugin*) and k 
	indexes match both the structure of Channel::midiInPlugins and 
	vector<Plugin*>* plugins. */

	std::vector<Plugin*> plugins = pluginHost::getStack(pluginHost::StackType::CHANNEL, ch->index);

	for (Plugin* plugin : plugins) {
		for (unsigned k=0; k<plugin->midiInParams.size(); k++) {
			uint32_t midiInParam = plugin->midiInParams.at(k);
			if (pure != midiInParam)
				continue;
			float vf = midiEvent.getVelocity() / 127.0f;
			c::plugin::setParameter(plugin->index, k, vf, pluginHost::StackType::CHANNEL, ch->index, /*gui=*/false);
			gu_log("  >>> [plugin %d parameter %d] ch=%d (pure=0x%X, value=%d, float=%f)\n",
				plugin->getId(), k, ch->index, pure, midiEvent.getVelocity(), vf);
		}
	}
}

#endif


/* -------------------------------------------------------------------------- */


void processChannels_(const MidiEvent& midiEvent)
{
	uint32_t pure = midiEvent.getRawNoVelocity();

	for (Channel* ch : mixer::channels) {

		/* Do nothing on this channel if MIDI in is disabled or filtered out for
		the current MIDI channel. */

		if (!ch->midiIn || !ch->isMidiInAllowed(midiEvent.getChannel()))
			continue;

		if      (pure == ch->midiInKeyPress) {
			gu_log("  >>> keyPress, ch=%d (pure=0x%X)\n", ch->index, pure);
			c::io::keyPress(ch, false, false, midiEvent.getVelocity());
		}
		else if (pure == ch->midiInKeyRel) {
			gu_log("  >>> keyRel ch=%d (pure=0x%X)\n", ch->index, pure);
			c::io::keyRelease(ch, false, false);
		}
		else if (pure == ch->midiInMute) {
			gu_log("  >>> mute ch=%d (pure=0x%X)\n", ch->index, pure);
			c::channel::setMute(ch->index, !ch->mute.load(), false);
		}		
		else if (pure == ch->midiInKill) {
			gu_log("  >>> kill ch=%d (pure=0x%X)\n", ch->index, pure);
			c::channel::kill(ch->index);
		}		
		else if (pure == ch->midiInArm) {
			gu_log("  >>> arm ch=%d (pure=0x%X)\n", ch->index, pure);
			c::channel::setArm(ch->index, ch->armed.load(), false);
		}
		else if (pure == ch->midiInSolo) {
			gu_log("  >>> solo ch=%d (pure=0x%X)\n", ch->index, pure);
			c::channel::setSolo(ch->index, !ch->solo.load(), false);
		}
		else if (pure == ch->midiInVolume) {
			float vf = midiEvent.getVelocity() / 127.0f; // TODO: u::math::map
			gu_log("  >>> volume ch=%d (pure=0x%X, value=%d, float=%f)\n",
				ch->index, pure, midiEvent.getVelocity(), vf);
			c::channel::setVolume(ch->index, vf, false);
		}
		else {
			SampleChannel* sch = static_cast<SampleChannel*>(ch);
			if (pure == sch->midiInPitch) {
				float vf = midiEvent.getVelocity() / (127/4.0f); // [0-127] ~> [0.0-4.0] TODO: u::math::map
				gu_log("  >>> pitch ch=%d (pure=0x%X, value=%d, float=%f)\n",
					sch->index, pure, midiEvent.getVelocity(), vf);
				c::channel::setPitch(sch->index, vf);
			}
			else 
			if (pure == sch->midiInReadActions) {
				gu_log("  >>> toggle read actions ch=%d (pure=0x%X)\n", sch->index, pure);
				c::channel::toggleReadingActions(sch->index, false);
			}
		}

#ifdef WITH_VST

		/* Process learned plugins parameters. */
		processPlugins_(ch, midiEvent); 

#endif

		/* Redirect full midi message (pure + velocity) to plugins. */
		ch->receiveMidi(midiEvent.getRaw());
	}
}


/* -------------------------------------------------------------------------- */


void processMaster_(const MidiEvent& midiEvent)
{
	uint32_t pure = midiEvent.getRawNoVelocity();

	if      (pure == conf::midiInRewind) {
		gu_log("  >>> rewind (master) (pure=0x%X)\n", pure);
		c::transport::rewindSeq(false);
	}
	else if (pure == conf::midiInStartStop) {
		gu_log("  >>> startStop (master) (pure=0x%X)\n", pure);
		c::transport::startStopSeq(false);
	}
	else if (pure == conf::midiInActionRec) {
		gu_log("  >>> actionRec (master) (pure=0x%X)\n", pure);
		c::io::toggleActionRec(false);
	}
	else if (pure == conf::midiInInputRec) {
		gu_log("  >>> inputRec (master) (pure=0x%X)\n", pure);
		c::io::toggleInputRec(false);
	}
	else if (pure == conf::midiInMetronome) {
		gu_log("  >>> metronome (master) (pure=0x%X)\n", pure);
		c::transport::toggleMetronome(false);
	}
	else if (pure == conf::midiInVolumeIn) {
		float vf = midiEvent.getVelocity() / 127.0f;
		gu_log("  >>> input volume (master) (pure=0x%X, value=%d, float=%f)\n",
			pure, midiEvent.getVelocity(), vf);
		c::main::setInVol(vf, false);
	}
	else if (pure == conf::midiInVolumeOut) {
		float vf = midiEvent.getVelocity() / 127.0f;
		gu_log("  >>> output volume (master) (pure=0x%X, value=%d, float=%f)\n",
			pure, midiEvent.getVelocity(), vf);
		c::main::setOutVol(vf, false);
	}
	else if (pure == conf::midiInBeatDouble) {
		gu_log("  >>> sequencer x2 (master) (pure=0x%X)\n", pure);
		c::main::beatsMultiply();
	}
	else if (pure == conf::midiInBeatHalf) {
		gu_log("  >>> sequencer /2 (master) (pure=0x%X)\n", pure);
		c::main::beatsDivide();
	}
}


/* -------------------------------------------------------------------------- */


void triggerSignalCb_()
{
	if (signalCb_ == nullptr) 
		return;
	signalCb_();
	signalCb_ = nullptr;
}
} // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


void startMidiLearn(cb_midiLearn* cb, void* data)
{
	cb_learn_ = cb;
	cb_data_  = data;
}


/* -------------------------------------------------------------------------- */


void stopMidiLearn()
{
	cb_learn_ = nullptr;
	cb_data_  = nullptr;
}


/* -------------------------------------------------------------------------- */


void dispatch(int byte1, int byte2, int byte3)
{
	/* Here we want to catch two things: a) note on/note off from a keyboard and 
	b) knob/wheel/slider movements from a controller. 
	We must also fix the velocity zero issue for those devices that sends NOTE
	OFF events as NOTE ON + velocity zero. Let's make it a real NOTE OFF event. */

	MidiEvent midiEvent(byte1, byte2, byte3);
	midiEvent.fixVelocityZero();

	gu_log("[midiDispatcher] MIDI received - 0x%X (chan %d)\n", midiEvent.getRaw(), 
		midiEvent.getChannel());

	/* Start dispatcher. If midi learn is on don't parse channels, just learn 
	incoming MIDI signal. Learn callback wants 'pure' MIDI event, i.e. with
	velocity value stripped off. If midi learn is off process master events first, 
	then each channel in the stack. This way incoming signals don't get processed 
	by glue_* when MIDI learning is on. */

	if (cb_learn_)
		cb_learn_(midiEvent.getRawNoVelocity(), cb_data_);
	else {
		processMaster_(midiEvent);
		processChannels_(midiEvent);
		triggerSignalCb_();
	}	
}


/* -------------------------------------------------------------------------- */


void setSignalCallback(std::function<void()> f)
{
	signalCb_ = f;
}

}}}; // giada::m::midiDispatcher::

