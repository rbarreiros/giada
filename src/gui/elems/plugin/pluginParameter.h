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


#ifndef GE_PLUGIN_PARAMETER_H
#define GE_PLUGIN_PARAMETER_H


#include <FL/Fl_Group.H>


class geBox;
class geSlider;


namespace giada {
namespace v
{
class gePluginParameter : public Fl_Group
{
public:

	gePluginParameter(int paramIndex, const m::Plugin& p, int x, int y, int w, 
		int labelWidth);

	void update(bool changeSlider);

private:

	static const int VALUE_WIDTH = 100;

	int m_paramIndex;
	const m::Plugin& m_plugin;

	geBox*    m_label;
	geSlider* m_slider;
	geBox*    m_value;

	static void cb_setValue(Fl_Widget* v, void* p);
	void cb_setValue();
};
}} // giada::v::


#endif

#endif // #ifdef WITH_VST
