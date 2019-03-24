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
#include <vector>
#include <algorithm>
#include "utils/fs.h"
#include "utils/string.h"
#include "utils/log.h"
#include "utils/vector.h"
#include "glue/main.h"
#include "glue/channel.h"
#include "core/model/model.h"
#include "core/model/data.h"
#include "core/kernelMidi.h"
#include "core/mixer.h"
#include "core/const.h"
#include "core/init.h"
#include "core/pluginHost.h"
#include "core/pluginManager.h"
#include "core/plugin.h"
#include "core/waveFx.h"
#include "core/conf.h"
#include "core/patch.h"
#include "core/recorder.h"
#include "core/clock.h"
#include "core/channel.h"
#include "core/kernelAudio.h"
#include "core/midiMapConf.h"
#include "core/sampleChannel.h"
#include "core/midiChannel.h"
#include "core/wave.h"
#include "core/waveManager.h"
#include "core/channelManager.h"
#include "mixerHandler.h"


namespace giada {
namespace m {
namespace mh
{
namespace
{
#ifdef WITH_VST

int readPatchPlugins_(const std::vector<patch::plugin_t>& list, pluginHost::StackType t)
{
	int ret = 1;
	for (const patch::plugin_t& ppl : list) {
		std::unique_ptr<Plugin> p = pluginManager::makePlugin(ppl.path);
		if (p != nullptr) {
			p->setBypass(ppl.bypass);
			for (unsigned j=0; j<ppl.params.size(); j++)
				p->setParameter(j, ppl.params.at(j));
			pluginHost::addPlugin(std::move(p), t, 0);
			ret &= 1;
		}
		else
			ret &= 0;
	}
	return ret;
}

#endif


/* -------------------------------------------------------------------------- */

#if 0
int getNewChanIndex()
{
	/* Always skip last channel: it's the last one just added. */

	const std::vector<Channel*>& channels = model::get()->channels;
	
	if (channels.size() == 1)
		return 0;

	int index = 0;
	for (unsigned i=0; i<channels.size()-1; i++) {
		if (channels.at(i)->index > index)
			index = channels.at(i)->index;
		}
	index += 1;
	return index;
}
#endif

}; // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


bool uniqueSamplePath(const SampleChannel* skip, const std::string& path)
{
	for (const Channel* ch : mixer::channels) {
		if (skip == ch || ch->type != ChannelType::SAMPLE) // skip itself and MIDI channels
			continue;
		const SampleChannel* sch = static_cast<const SampleChannel*>(ch);
		if (sch->wave != nullptr && path == sch->wave->getPath())
			return false;
	}
	return true;
}


/* -------------------------------------------------------------------------- */


Channel* addChannel(ChannelType type, size_t column)
{
	std::shared_ptr<model::Data> data = model::clone();

	std::unique_ptr<Channel> ch = channelManager::create(type, 
		kernelAudio::getRealBufSize(), conf::inputMonitorDefaultOn, column);
	ch->index = data->channels.size();
	data->channels.push_back(std::move(ch));
	model::swap(data);

	return data->channels.back().get();
}


/* -------------------------------------------------------------------------- */


int loadChannel(size_t chanIndex, const std::string& fname)
{
	waveManager::Result res = waveManager::createFromFile(fname); 

	if (res.status != G_RES_OK)
		return res.status;

	if (res.wave->getRate() != conf::samplerate) {
		gu_log("[mh::loadChannel] input rate (%d) != system rate (%d), conversion needed\n",
			res.wave->getRate(), conf::samplerate);
		res.status = waveManager::resample(res.wave.get(), conf::rsmpQuality, conf::samplerate); 
		if (res.status != G_RES_OK)
			return res.status;
	}

	std::shared_ptr<model::Data> data = model::clone();
	static_cast<SampleChannel*>(data->channels[chanIndex].get())->pushWave(std::move(res.wave));
	model::swap(data);

	return res.status;
}


/* -------------------------------------------------------------------------- */


void cloneChannel(size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	data->channels.push_back(channelManager::create(*data->channels[chanIndex]));
	data->channels.back()->index = data->channels.size() - 1;
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void freeChannel(size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	static_cast<SampleChannel*>(data->channels[chanIndex].get())->empty();
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void deleteChannel(size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	data->channels.erase(data->channels.begin() + chanIndex);
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


Channel* getChannelByIndex(int index)
{
	for (Channel* ch : mixer::channels)
		if (ch->index == index)
			return ch;
	gu_log("[getChannelByIndex] channel at index %d not found!\n", index);
	return nullptr;
}


/* -------------------------------------------------------------------------- */


void stopSequencer()
{
	clock::setStatus(ClockStatus::STOPPED);
	for (Channel* ch : mixer::channels)
		ch->stopBySeq(conf::chansStopOnSeqHalt);
}


/* -------------------------------------------------------------------------- */


void updateSoloCount()
{
	for (const Channel* ch : mixer::channels)
		if (ch->solo.load() == true) {
			mixer::hasSolos = true;
			return;
		}
	mixer::hasSolos = false;
}


/* -------------------------------------------------------------------------- */


void readPatch()
{
	mixer::ready = false;

	mixer::outVol.store(patch::masterVolOut);
	mixer::inVol.store(patch::masterVolIn);
	clock::setBpm(patch::bpm);
	clock::setBars(patch::bars);
	clock::setBeats(patch::beats);
	clock::setQuantize(patch::quantize);
	clock::updateFrameBars();
	mixer::setMetronome(patch::metronome);

#ifdef WITH_VST

	readPatchPlugins_(patch::masterInPlugins, pluginHost::StackType::MASTER_IN);
	readPatchPlugins_(patch::masterOutPlugins, pluginHost::StackType::MASTER_OUT);

#endif

	/* Rewind and update frames in Mixer. Also alloc new space in the virtual
	input buffer, in case the patch has a sequencer size != default one (which is
	very likely). */

	mixer::rewind();
	mixer::allocVirtualInput(clock::getFramesInLoop());
	mixer::ready = true;
}


/* -------------------------------------------------------------------------- */


void rewindSequencer()
{
	if (clock::getQuantize() > 0 && clock::isRunning())   // quantize rewind
		mixer::rewindWait = true;
	else
		mixer::rewind();
}


/* -------------------------------------------------------------------------- */


bool startInputRec()
{
	if (!hasRecordableSampleChannels())
		return false;

	for (Channel* ch : mixer::channels) {

		if (!ch->canInputRec())
			continue;

		SampleChannel* sch = static_cast<SampleChannel*>(ch);

		/* Allocate empty sample for the current channel. */

		std::string name    = std::string("TAKE-" + u::string::iToString(patch::lastTakeId++));
		std::string nameExt = name + ".wav";

		sch->pushWave(waveManager::createEmpty(clock::getFramesInLoop(), 
			G_MAX_IO_CHANS, conf::samplerate, nameExt));
		sch->name = name; 

		gu_log("[startInputRec] start input recs using Channel %d with size %d "
			"on frame=%d\n", sch->index, clock::getFramesInLoop(), clock::getCurrentFrame());
	}

	mixer::startInputRec();
	return true;
}


/* -------------------------------------------------------------------------- */


void stopInputRec()
{
	mixer::mergeVirtualInput();
	mixer::recording = false;

	for (Channel* ch : mixer::channels)
		ch->stopInputRec(clock::getCurrentFrame());

	gu_log("[mh] stop input recs\n");
}


/* -------------------------------------------------------------------------- */


bool hasArmedSampleChannels()
{
	return std::any_of(mixer::channels.begin(), mixer::channels.end(), [](const Channel* ch)
	{
		return ch->type == ChannelType::SAMPLE && ch->armed;
	});
}


bool hasRecordableSampleChannels()
{
	return std::any_of(mixer::channels.begin(), mixer::channels.end(), [](const Channel* ch)
	{
		return ch->canInputRec();
	});
}


bool hasLogicalSamples()
{
	return std::any_of(mixer::channels.begin(), mixer::channels.end(), [](const Channel* ch)
	{
		return ch->hasLogicalData();
	});
}


bool hasEditedSamples()
{
	return std::any_of(mixer::channels.begin(), mixer::channels.end(), [](const Channel* ch)
	{
		return ch->hasEditedData();
	});
}


}}}; // giada::m::mh::
