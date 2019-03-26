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


void processPlugins_(const Stack& stack, juce::MidiBuffer& events)
{
	for (const std::unique_ptr<Plugin>& p : stack) {
		if (p->isSuspended() || p->isBypassed())
			continue;
		const_cast<std::unique_ptr<Plugin>&>(p)->process(audioBuffer_, events);
		events.clear();
	}
}


/* -------------------------------------------------------------------------- */


/* getStack_ (1)
Returns the proper stack given the stack information. */

const Stack& getStack_(const std::shared_ptr<model::Data>& data, StackInfo info)
{
	switch(info.type) {
		case StackType::MASTER_OUT:
			return data->masterOutPlugins; break;
		case StackType::MASTER_IN:
			return data->masterInPlugins; break;
		case StackType::CHANNEL:
			assert(info.chanIndex < data->channels.size());
			return data->channels[info.chanIndex]->plugins; break;
		default:
			assert(false);
	}
}


/* getStack_ (1)
Same as above, for non-const data. It's ugly, but if you omit the const_cast on 
'data' the function would call itself in an infinite recursion. */

Stack& getStack_(std::shared_ptr<model::Data>& data, StackInfo info)
{
	return const_cast<Stack&>(getStack_(const_cast<const std::shared_ptr<model::Data>&>(data), info));
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


void addPlugin(std::unique_ptr<Plugin> p, StackInfo info)
{
	std::shared_ptr<model::Data> data = model::clone();

	Stack& stack = getStack_(data, info);
	p->index = stack.size();
	stack.push_back(std::move(p));

	model::swap(data);	
}


/* -------------------------------------------------------------------------- */


void freeStack(StackInfo info)
{
	std::shared_ptr<model::Data> data = model::clone();
	
	Stack& stack = getStack_(data, info);
	stack.clear();
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void processAudioStack(AudioBuffer& outBuf, const Stack& stack)
{
	if (stack.size() == 0)
		return;	

	assert(outBuf.countFrames() == audioBuffer_.getNumSamples());

	giadaToJuceTempBuf_(outBuf);
	juce::MidiBuffer events; // empty
	processPlugins_(stack, events);
	juceToGiadaOutBuf_(outBuf);
}


void processMidiStack(AudioBuffer& outBuf, const Stack& stack, juce::MidiBuffer& events)
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


const Plugin* getPluginByIndex(size_t pluginIndex, StackInfo info)
{
	return getStack_(model::get(), info)[pluginIndex].get();
}


/* -------------------------------------------------------------------------- */


void swapPlugin(size_t index1, size_t index2, StackInfo info)
{
	size_t size = getStack_(model::get(), info).size();

	/* Nothing to do if there's only one plugin or on edges. */
	
	if (size == 1 || index2 < 0 || index2 >= size)
		return;

	std::shared_ptr<model::Data> data = model::clone();

	Stack& stack = getStack_(data, info);
	std::swap(stack.at(index1), stack.at(index2));
	stack.at(index1)->index = index2;
	stack.at(index2)->index = index1;
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void freePlugin(size_t pluginIndex, StackInfo info)
{
	std::shared_ptr<model::Data> data = model::clone();
	
	Stack& stack = getStack_(data, info);
	stack.erase(stack.begin() + pluginIndex); 
	
	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void setPluginParameter(size_t pluginIndex, int paramIndex, float value, StackInfo info)
{
	getStack_(model::get(), info)[pluginIndex]->setParameter(paramIndex, value);
}


/* -------------------------------------------------------------------------- */


void setPluginProgram(size_t pluginIndex, int programIndex, StackInfo info)
{
	getStack_(model::get(), info)[pluginIndex]->setCurrentProgram(programIndex);
}


/* -------------------------------------------------------------------------- */


void runDispatchLoop()
{
	messageManager_->runDispatchLoopUntil(10);
}


/* -------------------------------------------------------------------------- */


void freeAllStacks()
{
	std::shared_ptr<model::Data> data = model::clone();

	getStack_(data, {StackType::MASTER_OUT, 0}).clear();
	getStack_(data, {StackType::MASTER_IN, 0}).clear();
	for (const std::unique_ptr<Channel>& c : data->channels)
		getStack_(data, {StackType::CHANNEL, c->index}).clear();

	model::swap(data);
}


/* -------------------------------------------------------------------------- */


void forEachPlugin(StackInfo info, std::function<void(const Plugin* p)> f)
{
	const Stack& stack = getStack_(model::get(), info);
	for (const std::unique_ptr<Plugin>& p : stack)
		f(p.get());
}

}}}; // giada::m::pluginHost::


#endif // #ifdef WITH_VST
