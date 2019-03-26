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


void giadaToJuceTempBuf_(const AudioBuffer& outBuf)
{
	for (int i=0; i<outBuf.countFrames(); i++)
		for (int j=0; j<outBuf.countChannels(); j++)
			audioBuffer_.setSample(j, i, outBuf[i][j]);
}


/* juceToGiadaOutBuf_
Converts buffer from Juce to Giada. A note for the future: if we overwrite (=) 
(as we do now) it's SEND, if we add (+) it's INSERT. */

void juceToGiadaOutBuf_(AudioBuffer& outBuf)
{
	for (int i=0; i<outBuf.countFrames(); i++)
		for (int j=0; j<outBuf.countChannels(); j++)	
			outBuf[i][j] = audioBuffer_.getSample(j, i);
}


/* -------------------------------------------------------------------------- */


void processPlugins_(const std::vector<std::unique_ptr<Plugin>>& stack, 
	juce::MidiBuffer& events)
{
	for (const std::unique_ptr<Plugin>& p : stack) {
		if (p->isSuspended() || p->isBypassed())
			continue;
		const_cast<std::unique_ptr<Plugin>&>(p)->process(audioBuffer_, events);
		events.clear();
	}
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


void close()
{
	messageManager_->deleteInstance();
}


/* -------------------------------------------------------------------------- */


void init(int buffersize)
{
	messageManager_ = juce::MessageManager::getInstance();
	audioBuffer_.setSize(G_MAX_IO_CHANS, buffersize);
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


Stack getStack(StackType type, size_t chanIndex)
{
	std::vector<std::unique_ptr<Plugin>>& stack = getStack_(model::get(), type, chanIndex);

	Stack out;
	out.type = type;
	out.chanIndex = chanIndex;
	for (const std::unique_ptr<Plugin>& p : stack)
		out.plugins.push_back(p.get());

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


void processAudioStack(AudioBuffer& outBuf, const std::vector<std::unique_ptr<Plugin>>& stack)
{
	if (stack.size() == 0)
		return;	

	assert(outBuf.countFrames() == audioBuffer_.getNumSamples());

	giadaToJuceTempBuf_(outBuf);
	juce::MidiBuffer events; // empty
	processPlugins_(stack, events);
	juceToGiadaOutBuf_(outBuf);
}


void processMidiStack(AudioBuffer& outBuf, const std::vector<std::unique_ptr<Plugin>>& stack,
	juce::MidiBuffer& events)
{
	if (stack.size() == 0)
		return;	

	assert(outBuf.countFrames() == audioBuffer_.getNumSamples());

	/* MIDI channels must not process the current buffer: give them an empty 
	and clean one. */

	audioBuffer_.clear();

	/* TODO - events: not sure how to pass/clear them yet */
	/* TODO - events: not sure how to pass/clear them yet */
	/* TODO - events: not sure how to pass/clear them yet */
	processPlugins_(stack, events);

	juceToGiadaOutBuf_(outBuf);
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
	for (const std::unique_ptr<Channel>& c : model::get()->channels)
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
