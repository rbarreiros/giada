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


#ifndef G_DATA_H
#define G_DATA_H


#include <memory>
#include <vector>
#include "core/recorder.h"
#include "core/plugin.h"


namespace giada {
namespace m
{
class AudioBuffer;
class Channel;


namespace render
{
class Data
{
public:

    Data();
    Data(const Data& o);
    // TODO - Data(Data&& o);

    void render(AudioBuffer& out, const AudioBuffer& in, AudioBuffer& inToOut);

    recorder::ActionMap actions;
    std::vector<Channel*> channels;
    std::vector<std::unique_ptr<Plugin>> masterOutPlugins;
    std::vector<std::unique_ptr<Plugin>> masterInPlugins;

    Channel* getChannel(const Channel* c);

private:

    void parseEvents(Frame f);
};

}}} // giada::m::

#endif