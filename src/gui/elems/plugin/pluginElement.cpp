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
#include "core/graphics.h"
#include "core/pluginHost.h"
#include "core/plugin.h"
#include "core/channel.h"
#include "utils/gui.h"
#include "utils/log.h"
#include "glue/plugin.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/choice.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/pluginList.h"
#include "gui/dialogs/pluginWindowGUI.h"
#include "gui/dialogs/pluginWindow.h"
#include "pluginElement.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace v
{
gePluginElement::gePluginElement(const m::Plugin& p, m::pluginHost::StackInfo info, int X, int Y, int W)
: Fl_Group   (X, Y, W, 20), 
  m_plugin   (p),
  m_stackInfo(info)
{
	begin();
	button    = new geButton(8, y(), 220, 20);
	program   = new geChoice(button->x()+button->w()+4, y(), 132, 20);
	bypass    = new geButton(program->x()+program->w()+4, y(), 20, 20);
	shiftUp   = new geButton(bypass->x()+bypass->w()+4, y(), 20, 20, "", fxShiftUpOff_xpm, fxShiftUpOn_xpm);
	shiftDown = new geButton(shiftUp->x()+shiftUp->w()+4, y(), 20, 20, "", fxShiftDownOff_xpm, fxShiftDownOn_xpm);
	remove    = new geButton(shiftDown->x()+shiftDown->w()+4, y(), 20, 20, "", fxRemoveOff_xpm, fxRemoveOn_xpm);
	end();

	button->copy_label(m_plugin.getName().c_str());
	button->callback(cb_openPluginWindow, (void*)this);

	program->callback(cb_setProgram, (void*)this);

	for (int i=0; i<m_plugin.getNumPrograms(); i++)
		program->add(u::gui::removeFltkChars(m_plugin.getProgramName(i)).c_str());

	if (program->size() == 0) {
		program->add("-- no programs --\0");
		program->deactivate();
	}
	else
		program->value(m_plugin.getCurrentProgram());

	bypass->callback(cb_setBypass, (void*)this);
	bypass->type(FL_TOGGLE_BUTTON);
	bypass->value(m_plugin.isBypassed() ? 0 : 1);

	shiftUp->callback(cb_shiftUp, (void*)this);
	shiftDown->callback(cb_shiftDown, (void*)this);
	remove->callback(cb_removePlugin, (void*)this);
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_removePlugin    (Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_removePlugin(); }
void gePluginElement::cb_openPluginWindow(Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_openPluginWindow(); }
void gePluginElement::cb_setBypass       (Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_setBypass(); }
void gePluginElement::cb_shiftUp         (Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_shiftUp(); }
void gePluginElement::cb_shiftDown       (Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_shiftDown(); }
void gePluginElement::cb_setProgram      (Fl_Widget* v, void* p) { ((gePluginElement*)p)->cb_setProgram(); }


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_shiftUp()
{
	c::plugin::swapPlugins(m_plugin.index, m_plugin.index - 1, m_stackInfo);
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_shiftDown()
{
	c::plugin::swapPlugins(m_plugin.index, m_plugin.index + 1, m_stackInfo);
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_removePlugin()
{
	/* Any subwindow linked to the plugin must be destroyed first. The 
	pluginWindow has id = id_plugin + 1, because id=0 is reserved for the parent 
	window 'add plugin'.*/
	
	static_cast<gdWindow*>(window())->delSubWindow(m_plugin.getId() + 1);
	c::plugin::freePlugin(m_plugin.index, m_stackInfo);
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_openPluginWindow()
{
	/* The new pluginWindow has id = id_plugin + 1, because id=0 is reserved for 
	the parent window 'add plugin'. */

	int pwid = m_plugin.getId() + 1;

	gdWindow* parent = static_cast<gdWindow*>(window());
	gdWindow* child  = parent->getChild(pwid);

	if (child != nullptr) {
		child->show();  // Raise it to top
	}
	else {
		if (m_plugin.hasEditor())
			child = new gdPluginWindowGUI(m_plugin);
		else 
			child = new gdPluginWindow(m_plugin, m_stackInfo);
		child->setId(pwid);
		parent->addSubWindow(child);
	}
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_setBypass()
{
	assert(false); 
	// TODO - pluginHost::bypassPlugin
	// m_plugin->toggleBypass();
}


/* -------------------------------------------------------------------------- */


void gePluginElement::cb_setProgram()
{
	c::plugin::setProgram(m_plugin.index, program->value(), m_stackInfo);
}
}} // giada::v::


#endif // #ifdef WITH_VST
