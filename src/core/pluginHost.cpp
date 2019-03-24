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


#ifdef WITH_VST


#include <cassert>
#include "utils/log.h"
#include "utils/vector.h"
#include "core/model/model.h"
#include "core/model/data.h"
#include "const.h"
#include "channel.h"
#include "plugin.h"
#include "pluginHost.h"


namespace giada {
namespace m {
namespace pluginHost
{
namespace
{
juce::MessageManager* messageManager_;
juce::AudioBuffer<float> audioBuffer_;

std::vector<std::unique_ptr<Plugin>> masterOut_;
std::vector<std::unique_ptr<Plugin>> masterIn_;


/* -------------------------------------------------------------------------- */


void processPlugin_(Plugin& p, Channel* ch)
{
	if (p.isSuspended() || p.isBypassed())
		return;

	juce::MidiBuffer events;
	if (ch != nullptr)
		events = ch->getPluginMidiEvents();

	p.process(audioBuffer_, events);
}


/* -------------------------------------------------------------------------- */

/* getStack_DEPR_
Returns a vector of unique_ptr's given the stackType. If stackType == CHANNEL
a pointer to Channel is also required. */

std::vector<std::unique_ptr<Plugin>>& getStack_DEPR_(StackType t, Channel* ch=nullptr)
{
	switch(t) {
		case StackType::MASTER_OUT:
			return masterOut_;
		case StackType::MASTER_IN:
			return masterIn_;
		case StackType::CHANNEL:
			return ch->plugins;
		default:
			assert(false);
	}
}


std::vector<std::unique_ptr<Plugin>>& getStack_(const std::shared_ptr<model::Data>&& data, 
	StackType stack, size_t chanIndex)
{
	switch(stack) {
		case StackType::MASTER_OUT:
			return data->masterOutPlugins; break;
		case StackType::MASTER_IN:
			return data->masterInPlugins; break;
		case StackType::CHANNEL:
			return data->channels[chanIndex]->plugins; break;
		default:
			assert(false);
	}
}

}; // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


pthread_mutex_t mutex;


/* -------------------------------------------------------------------------- */


void close()
{
	messageManager_->deleteInstance();
	pthread_mutex_destroy(&mutex);
}


/* -------------------------------------------------------------------------- */


void init(int buffersize)
{
	messageManager_ = juce::MessageManager::getInstance();
	audioBuffer_.setSize(G_MAX_IO_CHANS, buffersize);

	pthread_mutex_init(&mutex, nullptr);
}


/* -------------------------------------------------------------------------- */


void addPlugin(std::unique_ptr<Plugin> p, StackType stack, size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	switch(stack) {
		case StackType::MASTER_OUT:
			p->index     = data->masterOutPlugins.size();
			p->stackType = stack;
			data->masterOutPlugins.push_back(std::move(p)); break;
		case StackType::MASTER_IN:
			p->index     = data->masterInPlugins.size();
			p->stackType = stack;
			data->masterInPlugins.push_back(std::move(p)); break;
		case StackType::CHANNEL:
			p->index     = data->channels[chanIndex]->plugins.size();
			p->stackType = stack;
			p->chanIndex = chanIndex;
			data->channels[chanIndex]->plugins.push_back(std::move(p)); break;
	}
	model::swap(data);	
}


/* -------------------------------------------------------------------------- */


std::vector<Plugin*> getStack(StackType t, Channel* ch)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_DEPR_(t, ch);

	std::vector<Plugin*> out;
	for (const std::unique_ptr<Plugin>& p : stack)
		out.push_back(p.get());

	return out;
}


/* -------------------------------------------------------------------------- */


int countPlugins(StackType t, Channel* ch)
{
	return getStack_DEPR_(t, ch).size();
}


/* -------------------------------------------------------------------------- */


void freeStack(StackType t, pthread_mutex_t* mixerMutex, Channel* ch)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_DEPR_(t, ch);

	if (stack.size() == 0)
		return;

	pthread_mutex_lock(mixerMutex);
	stack.clear();
	pthread_mutex_unlock(mixerMutex);

	gu_log("[pluginHost::freeStack] stack type=%d freed\n", t);
}


/* -------------------------------------------------------------------------- */


void processStack(AudioBuffer& outBuf, StackType t, Channel* ch)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_DEPR_(t, ch);

	if (stack.size() == 0)
		return;

	assert(outBuf.countFrames() == audioBuffer_.getNumSamples());

	/* MIDI channels must not process the current buffer: give them an empty one. 
	Sample channels and Master in/out want audio data instead: let's convert the 
	internal buffer from Giada to Juce. */

	if (ch != nullptr && ch->type == ChannelType::MIDI) 
		audioBuffer_.clear();
	else
		for (int i=0; i<outBuf.countFrames(); i++)
			for (int j=0; j<outBuf.countChannels(); j++)
				audioBuffer_.setSample(j, i, outBuf[i][j]);

	/* Hardcore processing. Part of this loop must be guarded by mutexes, i.e. 
	the MIDI process part. You definitely don't want a situation like the 
	following one:
		1. this::processStack()
		2. [a new midi event comes in from kernelMidi thread]
		3. channel::clearMidiBuffer()
	The midi event in between would be surely lost, deleted by the last call to
	channel::clearMidiBuffer()! 
	TODO - that's why we need a proper queue for MIDI events in input... */

	if (ch != nullptr)
		pthread_mutex_lock(&mutex);

	for (std::unique_ptr<Plugin>& plugin : stack)
		processPlugin_(*plugin.get(), ch);

	if (ch != nullptr) {
		ch->clearMidiBuffer();
		pthread_mutex_unlock(&mutex);
	}

	/* Converting buffer from Juce to Giada. A note for the future: if we 
	overwrite (=) (as we do now) it's SEND, if we add (+) it's INSERT. */

	for (int i=0; i<outBuf.countFrames(); i++)
		for (int j=0; j<outBuf.countChannels(); j++)	
			outBuf[i][j] = audioBuffer_.getSample(j, i);
}


/* -------------------------------------------------------------------------- */


Plugin* getPluginByIndex(size_t pluginIndex, StackType stack, size_t chanIndex)
{
	//return getStack_(m::model::get(), stack, chanIndex)[pluginIndex].get();
	switch(stack) {
		case StackType::MASTER_OUT:
			return m::model::get()->masterOutPlugins[pluginIndex].get();
		case StackType::MASTER_IN:
			return m::model::get()->masterInPlugins[pluginIndex].get();
		case StackType::CHANNEL:
			return m::model::get()->channels[chanIndex]->plugins[pluginIndex].get();
		default:
			assert(false);
	}
}


/* -------------------------------------------------------------------------- */


int getPluginIndex(int id, StackType t, Channel* ch)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_DEPR_(t, ch);
	return u::vector::indexOf(stack, [&](const std::unique_ptr<Plugin>& p) 
	{ 
		return p->getId() == id;
	});
}


/* -------------------------------------------------------------------------- */


void swapPlugin(size_t pluginIndex1, size_t pluginIndex2, StackType stack, 
    size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	switch(stack) {
		case StackType::MASTER_OUT:
			std::swap(data->masterOutPlugins.at(pluginIndex1), data->masterOutPlugins.at(pluginIndex2)); break;
		case StackType::MASTER_IN:
			std::swap(data->masterInPlugins.at(pluginIndex1), data->masterInPlugins.at(pluginIndex2)); break;
		case StackType::CHANNEL:
			std::swap(data->channels[chanIndex]->plugins.at(pluginIndex1), data->channels[chanIndex]->plugins.at(pluginIndex2)); break;
		default: break;
	}
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void freePlugin(size_t pluginIndex, StackType stack, size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	switch(stack) {
		case StackType::MASTER_OUT: {
			auto it = data->masterOutPlugins.begin() + pluginIndex;
			data->masterOutPlugins.erase(it); break;
		}
		case StackType::MASTER_IN: {
			auto it = data->masterInPlugins.begin() + pluginIndex;
			data->masterInPlugins.erase(it); break;
		}	
		case StackType::CHANNEL: {
			auto it = data->channels[chanIndex]->plugins.begin() + pluginIndex;
			data->channels[chanIndex]->plugins.erase(it); break;
		}
	}
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void setParameter(size_t pluginIndex, int paramIndex, float value, StackType stack, 
    size_t chanIndex)
{
	switch(stack) {
		case StackType::MASTER_OUT:
			m::model::get()->masterOutPlugins[pluginIndex]->setParameter(paramIndex, value); break;
		case StackType::MASTER_IN:
			m::model::get()->masterInPlugins[pluginIndex]->setParameter(paramIndex, value); break;
		case StackType::CHANNEL:
			m::model::get()->channels[chanIndex]->plugins[pluginIndex]->setParameter(paramIndex, value); break;
	}
}


/* -------------------------------------------------------------------------- */


void runDispatchLoop()
{
	messageManager_->runDispatchLoopUntil(10);
}


/* -------------------------------------------------------------------------- */


void freeAllStacks(std::vector<Channel*>* channels, pthread_mutex_t* mixerMutex)
{
	freeStack(StackType::MASTER_OUT, mixerMutex);
	freeStack(StackType::MASTER_IN, mixerMutex);
	for (Channel* c : *channels)
		freeStack(StackType::CHANNEL, mixerMutex, c);
}


/* -------------------------------------------------------------------------- */


void forEachPlugin(StackType t, const Channel* ch, std::function<void(const Plugin* p)> f)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_DEPR_(t, const_cast<Channel*>(ch));
	for (const std::unique_ptr<Plugin>& p : stack)
		f(p.get());
}

}}}; // giada::m::pluginHost::


#endif // #ifdef WITH_VST
