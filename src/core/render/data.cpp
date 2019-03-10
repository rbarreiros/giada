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
#include "data.h"


namespace giada {
namespace m {
namespace render
{
Data::Data()
{
}


/* -------------------------------------------------------------------------- */


Data::Data(const Data& o)
: channels(o.channels)
{
	for (const std::unique_ptr<Plugin>& p : o.masterOutPlugins)
		masterOutPlugins.push_back(std::make_unique<Plugin>(*p));
	
	for (const std::unique_ptr<Plugin>& p : o.masterInPlugins)
		masterInPlugins.push_back(std::make_unique<Plugin>(*p));
}


/* -------------------------------------------------------------------------- */


Data::~Data()
{
	puts("~Data");
}


/* -------------------------------------------------------------------------- */


void Data::render(AudioBuffer& out, const AudioBuffer& in, AudioBuffer& inToOut)
{
	for (Channel* channel : channels)
		channel->prepareBuffer(clock::isRunning());

	if (clock::isRunning())
		for (Frame i=0; i<out.countFrames(); i++)
			parseEvents(i);

	for (Channel* channel : channels)
		channel->process(out, in, mixer::isChannelAudible(channel), clock::isRunning());

#ifdef WITH_VST
	pluginHost::processStack(out,     pluginHost::StackType::MASTER_OUT);
	pluginHost::processStack(inToOut, pluginHost::StackType::MASTER_IN);
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

	for (Channel* channel : channels)
		channel->parseEvents(fe);   
}


/* -------------------------------------------------------------------------- */


Channel* Data::getChannel(const Channel* c)
{
	for (Channel* channel : channels)
		if (channel->index == c->index)
			return channel;
	assert(false);
	return nullptr;
}

}}} // giada::m::render::
