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


#ifndef GE_STACKER_H
#define GE_STACKER_H


#include <cstdio>
#include <FL/Fl_Group.H>
#include "core/const.h"


namespace giada {
namespace v
{
class geStacker : public Fl_Group
{
public:

	enum class Dir { HORIZONTAL, VERICAL };

	geStacker(int x, int y, int w, int h, Dir);

	template<typename T>
	T stack(T w)
	{
		int newx = x();
		int newy = y();

		if (children() > 1) {  // TODO - why is there always a first child on empty groups?
			if (m_dir == Dir::HORIZONTAL)
				newx = m_prevx + m_prevw + G_GUI_INNER_MARGIN;
			else
				newy = m_prevy + m_prevh + G_GUI_INNER_MARGIN; 
		}   

		m_prevx = newx;
		m_prevy = newy;
		m_prevw = w->w();
		m_prevh = w->h();

		w->position(newx, newy); 
		Fl_Group::add(w); 

		return w;        
	}

private:

	Dir m_dir;
	int m_prevx;
	int m_prevy;
	int m_prevw;
	int m_prevh;
};
}} // giada::v::


#endif
