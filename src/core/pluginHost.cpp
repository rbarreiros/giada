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


void processPlugin_(Plugin& p, size_t chanIndex)
{
	if (p.isSuspended() || p.isBypassed())
		return;

	juce::MidiBuffer events;
	if (p.stackType == StackType::CHANNEL)
		events = model::get()->channels[chanIndex]->getPluginMidiEvents();

	p.process(audioBuffer_, events);
}


/* -------------------------------------------------------------------------- */

/* getStack_
Returns a vector of unique_ptr's given the stackType. If stackType == CHANNEL
a channel index is also required. */

std::vector<std::unique_ptr<Plugin>>& getStack_(std::shared_ptr<model::Data> data, 
	StackType stack, size_t chanIndex=0)
{
	switch(stack) {
		case StackType::MASTER_OUT:
			return data->masterOutPlugins; break;
		case StackType::MASTER_IN:
			return data->masterInPlugins; break;
		case StackType::CHANNEL:
			assert(chanIndex < data->channels.size());
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


void addPlugin(std::unique_ptr<Plugin> p, StackType type, size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();

	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(data, type, chanIndex);
	p->index     = stack.size();
	p->stackType = type;
	stack.push_back(std::move(p));

	model::swap(data);	
}


/* -------------------------------------------------------------------------- */


std::vector<Plugin*> getStack(StackType type, size_t chanIndex)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(model::get(), type, chanIndex);

	std::vector<Plugin*> out;
	for (const std::unique_ptr<Plugin>& p : stack)
		out.push_back(p.get());

	return out;
}


/* -------------------------------------------------------------------------- */


int countPlugins(StackType type, size_t chanIndex)
{
	return getStack_(model::get(), type, chanIndex).size();
}


/* -------------------------------------------------------------------------- */


void freeStack(StackType type, size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(data, type, chanIndex);
	stack.clear();
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void processStack(AudioBuffer& outBuf, StackType stackType,size_t chanIndex)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(model::get(), 
		stackType, chanIndex);

	if (stack.size() == 0)
		return;

	assert(outBuf.countFrames() == audioBuffer_.getNumSamples());

	/* MIDI channels must not process the current buffer: give them an empty one. 
	Sample channels and Master in/out want audio data instead: let's convert the 
	internal buffer from Giada to Juce. */

	if (stackType == StackType::CHANNEL && model::get()->channels[chanIndex]->type == ChannelType::MIDI) 
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

	for (std::unique_ptr<Plugin>& plugin : stack)
		processPlugin_(*plugin.get(), chanIndex);

	if (stackType == StackType::CHANNEL)
		model::get()->channels[chanIndex]->clearMidiBuffer();

	/* Converting buffer from Juce to Giada. A note for the future: if we 
	overwrite (=) (as we do now) it's SEND, if we add (+) it's INSERT. */

	for (int i=0; i<outBuf.countFrames(); i++)
		for (int j=0; j<outBuf.countChannels(); j++)	
			outBuf[i][j] = audioBuffer_.getSample(j, i);
}


/* -------------------------------------------------------------------------- */


Plugin* getPluginByIndex(size_t pluginIndex, StackType stack, size_t chanIndex)
{
	return getStack_(model::get(), stack, chanIndex)[pluginIndex].get();
}


/* -------------------------------------------------------------------------- */


void swapPlugin(size_t pluginIndex1, size_t pluginIndex2, StackType type, 
    size_t chanIndex)
{
	size_t stackSize = getStack_(model::get(), type, chanIndex).size();

	/* Nothing to do if there's only one plugin or on edges. */
	
printf("%ld %ld\n", pluginIndex1, pluginIndex2);

	if (stackSize == 1 || pluginIndex2 < 0 || pluginIndex2 >= stackSize)
		return;

	std::shared_ptr<model::Data> data = model::clone();
	
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(data, type, chanIndex);
	std::swap(stack.at(pluginIndex1), stack.at(pluginIndex2));
	stack.at(pluginIndex1)->index = pluginIndex2;
	stack.at(pluginIndex2)->index = pluginIndex1;
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void freePlugin(size_t pluginIndex, StackType type, size_t chanIndex)
{
	std::shared_ptr<model::Data> data = model::clone();
	
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(data, type, chanIndex);
	stack.erase(stack.begin() + pluginIndex); 
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void setPluginParameter(size_t pluginIndex, int paramIndex, float value, StackType type, 
    size_t chanIndex)
{
	getStack_(model::get(), type, chanIndex)[pluginIndex]->setParameter(paramIndex, value);
}


/* -------------------------------------------------------------------------- */


void setPluginProgram(size_t pluginIndex, int programIndex, StackType type, 
    size_t chanIndex)
{
	getStack_(model::get(), type, chanIndex)[pluginIndex]->setCurrentProgram(programIndex);
}


/* -------------------------------------------------------------------------- */


void runDispatchLoop()
{
	messageManager_->runDispatchLoopUntil(10);
}


/* -------------------------------------------------------------------------- */


void freeAllStacks()
{
	freeStack(StackType::MASTER_OUT, 0);
	freeStack(StackType::MASTER_IN, 0);
	for (const Channel* c : model::get()->channels)
		freeStack(StackType::CHANNEL, c->index);
}


/* -------------------------------------------------------------------------- */


void forEachPlugin(StackType type, size_t chanIndex, std::function<void(const Plugin* p)> f)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(model::get(), type, chanIndex);
	for (const std::unique_ptr<Plugin>& p : stack)
		f(p.get());
}

}}}; // giada::m::pluginHost::


#endif // #ifdef WITH_VST
