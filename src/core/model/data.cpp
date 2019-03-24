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
#include "core/clock.h"
#include "core/channel.h"
#include "core/mixer.h"
#include "core/audioBuffer.h"
#include "core/pluginHost.h"
#include "core/channelManager.h"
#include "data.h"


namespace giada {
namespace m {
namespace model
{
Data::Data()
{
}


/* -------------------------------------------------------------------------- */


Data::Data(const Data& o)
{
printf("DATA: %p\n", (void*)this);

	for (const std::unique_ptr<Channel>& c : o.channels)
		channels.push_back(channelManager::create(*c));

	for (const std::unique_ptr<Plugin>& p : o.masterOutPlugins)
		masterOutPlugins.push_back(std::make_unique<Plugin>(*p));
	
	for (const std::unique_ptr<Plugin>& p : o.masterInPlugins)
		masterInPlugins.push_back(std::make_unique<Plugin>(*p));

	// TODO - actionMap
}


/* -------------------------------------------------------------------------- */


Data::~Data()
{
printf("~DATA: %p\n", (void*)this);
}


/* -------------------------------------------------------------------------- */


void Data::render(AudioBuffer& out, const AudioBuffer& in, AudioBuffer& inToOut)
{
	for (std::unique_ptr<Channel>& channel : channels)
		channel->prepareBuffer(clock::isRunning());

	if (clock::isRunning())
		for (Frame i=0; i<out.countFrames(); i++)
			parseEvents(i);

	for (std::unique_ptr<Channel>& channel : channels)
		channel->process(out, in, mixer::isChannelAudible(channel.get()), clock::isRunning());

#ifdef WITH_VST
	pluginHost::processStack(out,     pluginHost::StackType::MASTER_OUT, 0);
	pluginHost::processStack(inToOut, pluginHost::StackType::MASTER_IN, 0);
#endif
}


/* -------------------------------------------------------------------------- */


void Data::parseEvents(Frame f)
{
	mixer::FrameEvents fe;
	fe.frameLocal   = f;
	fe.frameGlobal  = clock::getCurrentFrame();
	fe.doQuantize   = clock::getQuantize() == 0 || !clock::quantoHasPassed();
	fe.onBar        = clock::isOnBar();
	fe.onFirstBeat  = clock::isOnFirstBeat();
	fe.quantoPassed = clock::quantoHasPassed();
	fe.actions      = recorder::getActionsOnFrame(clock::getCurrentFrame());

	for (std::unique_ptr<Channel>& channel : channels)
		channel->parseEvents(fe);   
}


/* -------------------------------------------------------------------------- */


Channel* Data::getChannel(const Channel* c) const
{
	for (const std::unique_ptr<Channel>& channel : channels)
		if (channel.get() == c)
			return channel.get();
	assert(false);
	return nullptr;
}

}}} // giada::m::model::
