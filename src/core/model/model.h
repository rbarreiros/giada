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


#ifndef G_RENDER_MODEL_H
#define G_RENDER_MODEL_H


#include <memory>
#include <atomic>
#include "data.h"


namespace giada {
namespace m {
namespace model
{
const std::shared_ptr<Data> get();

/* clone
Returns a copy of the current data. Call this when you want to modify the 
current render data model. */

std::shared_ptr<Data> clone();

/* swap
Atomically swaps current data with the new one provided. */

void swap(std::shared_ptr<Data> data);

/* changed
Marks if the model has changed and requires UI update. */

extern std::atomic<bool> changed;


// TODO extern FIFO<Message, 1024> midiQueue;


}}} // giada::m::model::


#endif