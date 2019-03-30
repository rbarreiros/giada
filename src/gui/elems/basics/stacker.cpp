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


#include "stacker.h"


namespace giada {
namespace v
{
geStacker::geStacker(int x, int y, int w, int h, Dir dir)
: Fl_Group(x, y, w, h),
  m_dir   (dir),
  m_prevx (x),
  m_prevy (y),
  m_prevw (0),
  m_prevh (0)
{
}


/* -------------------------------------------------------------------------- */

/*
Fl_Widget* geStacker::stack(Fl_Widget* w)
{
    int newx = x();
    int newy = y();

    if (children() > 0) {
        if (m_dir == Dir::HORIZONTAL)
            newx = child(children() - 1)->x() + child(children() - 1)->w() + 1;
        else
            newy = child(children() - 1)->y() + child(children() - 1)->h() + 1; 
    }   
printf("children: %d --- %d %d\n", children(), newx, newy);
    Fl_Group::add(w); 
    w->position(newx, newy); 
    return w;        
}*/

}} // giada::v::