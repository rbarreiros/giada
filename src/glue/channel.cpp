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


#include <cmath>
#include <cassert>
#include <FL/Fl.H>
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/sampleEditor.h"
#include "gui/dialogs/warnings.h"
#include "gui/elems/basics/input.h"
#include "gui/elems/basics/dial.h"
#include "gui/elems/sampleEditor/waveTools.h"
#include "gui/elems/sampleEditor/volumeTool.h"
#include "gui/elems/sampleEditor/boostTool.h"
#include "gui/elems/sampleEditor/panTool.h"
#include "gui/elems/sampleEditor/pitchTool.h"
#include "gui/elems/sampleEditor/rangeTool.h"
#include "gui/elems/sampleEditor/waveform.h"
#include "gui/elems/mainWindow/keyboard/keyboard.h"
#include "gui/elems/mainWindow/keyboard/channel.h"
#include "gui/elems/mainWindow/keyboard/sampleChannel.h"
#include "gui/elems/mainWindow/keyboard/channelButton.h"
#include "utils/gui.h"
#include "utils/fs.h"
#include "utils/log.h"
#include "core/model/model.h"
#include "core/model/data.h"
#include "core/kernelAudio.h"
#include "core/mixerHandler.h"
#include "core/mixer.h"
#include "core/clock.h"
#include "core/pluginHost.h"
#include "core/conf.h"
#include "core/wave.h"
#include "core/channel.h"
#include "core/sampleChannel.h"
#include "core/midiChannel.h"
#include "core/recorder.h"
#include "core/plugin.h"
#include "core/waveManager.h"
#include "main.h"
#include "channel.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace c {
namespace channel 
{
int loadChannel(size_t chanIndex, const std::string& fname)
{
	/* Save the patch and take the last browser's dir in order to re-use it the 
	next time. */

	m::conf::samplePath = gu_dirname(fname);

	return m::mh::loadChannel(chanIndex, fname);
}


/* -------------------------------------------------------------------------- */


m::Channel* addChannel(size_t columnIndex, ChannelType type, int size)
{
	return m::mh::addChannel(type, columnIndex);
}


/* -------------------------------------------------------------------------- */


void deleteChannel(size_t chanIndex)
{
	if (!gdConfirmWin("Warning", "Delete channel: are you sure?"))
		return;
	u::gui::closeAllSubwindows();
	m::recorder::clearChannel(chanIndex);
	m::mh::deleteChannel(chanIndex);
}


/* -------------------------------------------------------------------------- */


void freeChannel(size_t chanIndex)
{
	if (!gdConfirmWin("Warning", "Free channel: are you sure?"))
		return;
	u::gui::closeAllSubwindows();
	m::recorder::clearChannel(chanIndex);
	m::mh::freeChannel(chanIndex);
}


/* -------------------------------------------------------------------------- */


void setArm(size_t chanIndex, bool value, bool gui)
{
	m::model::get()->channels[chanIndex]->armed.store(value);
	if (!gui) {
		Fl::lock();
		G_MainWin->keyboard->getChannel(chanIndex)->arm->value(value);
		Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setInputMonitor(size_t chanIndex, bool value)
{
	static_cast<m::SampleChannel*>(m::model::get()->channels[chanIndex].get())->inputMonitor.store(value);
}


/* -------------------------------------------------------------------------- */


void cloneChannel(size_t chanIndex)
{
	m::mh::cloneChannel(chanIndex);
}


/* -------------------------------------------------------------------------- */


void setVolume(size_t chanIndex, float value, bool gui, bool editor)
{
	m::model::get()->channels[chanIndex]->volume.store(value);

	/* Changing channel volume? Update wave editor (if it's shown). */

	if (editor) {
		gdSampleEditor* gdEditor = static_cast<gdSampleEditor*>(u::gui::getSubwindow(G_MainWin, WID_SAMPLE_EDITOR));
		if (gdEditor) {
			if (!gui) Fl::lock();
			gdEditor->volumeTool->refresh();
			if (!gui) Fl::unlock();
		}
	}

	if (!gui) {
		Fl::lock();
		G_MainWin->keyboard->getChannel(chanIndex)->vol->value(value);
		Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setPitch(size_t chanIndex, float val, bool gui)
{
	static_cast<m::SampleChannel*>(m::model::get()->channels[chanIndex].get())->setPitch(val);

	gdSampleEditor* gdEditor = static_cast<gdSampleEditor*>(u::gui::getSubwindow(G_MainWin, WID_SAMPLE_EDITOR));
	if (gdEditor) {
		if (!gui) Fl::lock();
		gdEditor->pitchTool->refresh();
		if (!gui) Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setPan(size_t chanIndex, float val, bool gui)
{
	m::model::get()->channels[chanIndex]->setPan(val);

	gdSampleEditor* gdEditor = static_cast<gdSampleEditor*>(u::gui::getSubwindow(G_MainWin, WID_SAMPLE_EDITOR));
	if (gdEditor) {
		if (!gui) Fl::lock();
		gdEditor->panTool->refresh();
		if (!gui) Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setMute(size_t chanIndex, bool value, bool gui)
{
	m::model::get()->channels[chanIndex]->setMute(value);
	if (!gui) {
		Fl::lock();
		G_MainWin->keyboard->getChannel(chanIndex)->mute->value(value);
		Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setSolo(size_t chanIndex, bool value, bool gui)
{
	m::model::get()->channels[chanIndex]->setSolo(value);
	if (!gui) {
		Fl::lock();
		G_MainWin->keyboard->getChannel(chanIndex)->solo->value(value);
		Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void kill(size_t chanIndex)
{
	m::model::get()->channels[chanIndex]->kill(0); // on frame 0: it's a user-generated event
}


/* -------------------------------------------------------------------------- */


void setBoost(size_t chanIndex, float val, bool gui)
{
	static_cast<m::SampleChannel*>(m::model::get()->channels[chanIndex].get())->setBoost(val);

	gdSampleEditor* gdEditor = static_cast<gdSampleEditor*>(u::gui::getSubwindow(G_MainWin, WID_SAMPLE_EDITOR));
	if (gdEditor) {
		if (!gui) Fl::lock();
		gdEditor->boostTool->refresh();
		if (!gui) Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void setName(size_t chanIndex, const std::string& name)
{
	m::model::get()->channels[chanIndex]->name = name;
}


/* -------------------------------------------------------------------------- */


void toggleReadingActions(size_t chanIndex, bool gui)
{
	const m::Channel* ch = m::model::get()->channels[chanIndex].get();

	/* When you call startReadingRecs with conf::treatRecsAsLoops, the
	member value ch->readActions actually is not set to true immediately, because
	the channel is in wait mode (REC_WAITING). ch->readActions will become true on
	the next first beat. So a 'stop rec' command should occur also when
	ch->readActions is false but the channel is in wait mode; this check will
	handle the case of when you press 'R', the channel goes into REC_WAITING and
	then you press 'R' again to undo the status. */

	if (ch->readActions || (!ch->readActions && ch->recStatus == ChannelStatus::WAIT))
		stopReadingActions(chanIndex, gui);
	else
		startReadingActions(chanIndex, gui);
}


/* -------------------------------------------------------------------------- */


void startReadingActions(size_t chanIndex, bool gui)
{
	m::model::get()->channels[chanIndex]->startReadingActions(m::conf::treatRecsAsLoops, 
		m::conf::recsStopOnChanHalt); 

	if (!gui) {
		Fl::lock();
		static_cast<v::geSampleChannel*>(G_MainWin->keyboard->getChannel(chanIndex))->readActions->value(1);
		Fl::unlock();
	}
}


/* -------------------------------------------------------------------------- */


void stopReadingActions(size_t chanIndex, bool gui)
{
	m::model::get()->channels[chanIndex]->stopReadingActions(m::clock::isRunning(), 
		m::conf::treatRecsAsLoops, m::conf::recsStopOnChanHalt);

	if (!gui) {
		Fl::lock();
		static_cast<v::geSampleChannel*>(G_MainWin->keyboard->getChannel(chanIndex))->readActions->value(0);
		Fl::unlock();
	}
}

}}}; // giada::c::channel::
