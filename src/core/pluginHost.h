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


#ifndef G_PLUGIN_HOST_H
#define G_PLUGIN_HOST_H


#include <functional>
#include <pthread.h>
#include "deps/juce-config.h"


namespace giada {
namespace m 
{
class Plugin;
class Channel;
class AudioBuffer;

namespace pluginHost
{
using Stack = std::vector<std::unique_ptr<Plugin>>;

enum class StackType 
{ 
	MASTER_OUT, MASTER_IN, CHANNEL 
};

struct StackInfo
{
	StackType type;
	size_t    chanIndex;
};

void init(int buffersize);
void close();

/* addPlugin
Adds a new plugin to stack 't'. */

void addPlugin(std::unique_ptr<Plugin> p, StackInfo info);

/* freeStack
Frees plugin stack of type 't'. */

void freeStack(StackInfo info);

/* processStack
Applies the fx list to the buffer. */

void processAudioStack(AudioBuffer& outBuf, const Stack& stack);
void processMidiStack(AudioBuffer& outBuf, const Stack& stack, 
    juce::MidiBuffer& events);

/* getPluginByIndex */

const Plugin* getPluginByIndex(size_t pluginIndex, StackInfo info);

/* swapPlugin */

void swapPlugin(size_t index1, size_t index2, StackInfo info);

/* freePlugin.
Returns the internal stack index of the deleted plugin. */

void freePlugin(size_t pluginIndex, StackInfo info);

void setPluginParameter(size_t pluginIndex, int paramIndex, float value, StackInfo info);
void setPluginProgram(size_t pluginIndex, int programIndex, StackInfo info); 

void toggleBypass(size_t pluginIndex, StackInfo info);

/* runDispatchLoop
Wakes up plugins' GUI manager for N milliseconds. */

void runDispatchLoop();

/* freeAllStacks
Frees everything. */

void freeAllStacks();

void forEachPlugin(StackInfo info, std::function<void(const Plugin* p)> f);

}}}; // giada::m::pluginHost::


#endif

#endif // #ifdef WITH_VST
