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


#include "model.h"


namespace giada {
namespace m {
namespace model
{
std::atomic<bool> changed(false);
std::shared_ptr<Data> data_ = std::make_shared<Data>();


/* -------------------------------------------------------------------------- */


const std::shared_ptr<Data> get()
{
    return std::atomic_load(&data_);
}


/* -------------------------------------------------------------------------- */


std::shared_ptr<Data> clone()
{
    return std::make_shared<Data>(*data_);
}


/* -------------------------------------------------------------------------- */


void swap(std::shared_ptr<Data> newData)
{
    std::shared_ptr<Data> oldData = std::atomic_load(&data_);
    while (!std::atomic_compare_exchange_weak(&data_, &oldData, newData));
    changed.store(true);
}

}}} // giada::m::model::
