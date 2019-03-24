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
#include <string>
#include <FL/Fl_Scroll.H>
#include "core/conf.h"
#include "core/const.h"
#include "core/pluginHost.h"
#include "core/channel.h"
#include "utils/string.h"
#include "utils/gui.h"
#include "gui/elems/basics/boxtypes.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/statusButton.h"
#include "gui/elems/mainWindow/mainIO.h"
#include "gui/elems/mainWindow/keyboard/channel.h"
#include "gui/elems/plugin/pluginElement.h"
#include "pluginChooser.h"
#include "mainWindow.h"
#include "pluginList.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace v
{
gdPluginList::gdPluginList(m::pluginHost::StackType stackType, size_t chanIndex)
: gdWindow (468, 204), 
  stackType(stackType),
  chanIndex(chanIndex)
{
	if (m::conf::pluginListX)
		resize(m::conf::pluginListX, m::conf::pluginListY, w(), h());

	list = new Fl_Scroll(8, 8, 476, 188);
	list->type(Fl_Scroll::VERTICAL);
	list->scrollbar.color(G_COLOR_GREY_2);
	list->scrollbar.selection_color(G_COLOR_GREY_4);
	list->scrollbar.labelcolor(G_COLOR_LIGHT_1);
	list->scrollbar.slider(G_CUSTOM_BORDER_BOX);

	list->begin();
	rebuild();
	list->end();

	end();
	set_non_modal();

	/* TODO - awful stuff... we should subclass into gdPluginListChannel and
	gdPluginListMaster */

	if (stackType == m::pluginHost::StackType::MASTER_OUT)
		label("Master Out Plug-ins");
	else
	if (stackType == m::pluginHost::StackType::MASTER_IN)
		label("Master In Plug-ins");
	else {
		std::string l = "Channel " + u::string::iToString(chanIndex + 1) + " Plug-ins";
		copy_label(l.c_str());
	}

	u::gui::setFavicon(this);
	show();
}


/* -------------------------------------------------------------------------- */


gdPluginList::~gdPluginList()
{
	m::conf::pluginListX = x();
	m::conf::pluginListY = y();
}


/* -------------------------------------------------------------------------- */


void gdPluginList::cb_addPlugin(Fl_Widget* v, void* p) { ((gdPluginList*)p)->cb_addPlugin(); }


/* -------------------------------------------------------------------------- */


void gdPluginList::rebuild()
{
	m::pluginHost::Stack stack = m::pluginHost::getStack(stackType, chanIndex);

	/* Clear the previous list. */

	list->clear();
	list->scroll_to(0, 0);

	/* Add new plug-ins buttons, as many as the plugin in pluginHost::stack + 1, 
	then 'add new' button. */

	int i  = 0;
	int py = 0;
	for (const m::Plugin* plugin : stack.plugins) {
		py = (list->y() - list->yposition()) + (i * 24);
		list->add(new gePluginElement(*this, *plugin, list->x(), py, 800));
		i++;
	}

	py = py == 0 ? 90 : py = (list->y() - list->yposition()) + ((i + 1) * 24);

	addPlugin = new geButton(8, py, 452, 20, "-- add new plugin --");
	addPlugin->callback(cb_addPlugin, (void*)this);
	list->add(addPlugin);

	/* If num(plugins) > 7 make room for the side scrollbar. 
	Scrollbar.width = 20 + 4(margin) */

	if (i>7)
		size(492, h());
	else
		size(468, h());

	redraw();	
}


/* -------------------------------------------------------------------------- */


void gdPluginList::cb_addPlugin()
{
	int wx = m::conf::pluginChooserX;
	int wy = m::conf::pluginChooserY;
	int ww = m::conf::pluginChooserW;
	int wh = m::conf::pluginChooserH;
	u::gui::openSubWindow(G_MainWin, new v::gdPluginChooser(wx, wy, ww, wh, stackType, chanIndex), 
		WID_FX_CHOOSER);
}


/* -------------------------------------------------------------------------- */

#if 0
void gdPluginList::refreshList()
{


/*THESE THINGS MUST BE PERFORMED BY EACH COMPONENT IN THEIR REFRESH() METHODS */
/* 
	if (stack.type == pluginHost::StackType::MASTER_OUT) {
		G_MainWin->mainIO->setMasterFxOutFull(stack.plugins.size() > 0);
	}
	else
	if (stack.type == pluginHost::StackType::MASTER_IN) {
		G_MainWin->mainIO->setMasterFxInFull(stack.plugins.size() > 0);
	}
	else {
		ch->guiChannel->fx->status = stack.plugins.size();
		ch->guiChannel->fx->redraw();
	}
*/
}
#endif
}} // giada::v::


#endif // #ifdef WITH_VST
