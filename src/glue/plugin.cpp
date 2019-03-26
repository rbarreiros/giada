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
#include <FL/Fl.H>
#include "core/pluginManager.h"
#include "core/pluginHost.h"
#include "core/mixer.h"
#include "core/plugin.h"
#include "core/channel.h"
#include "core/const.h"
#include "core/conf.h"
#include "utils/gui.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/pluginWindow.h"
#include "gui/dialogs/pluginList.h"
#include "gui/dialogs/warnings.h"
#include "gui/dialogs/config.h"
#include "gui/dialogs/browser/browserDir.h"
#include "plugin.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace c     {
namespace plugin 
{
namespace
{
void updatePluginEditor_(size_t pluginIndex, m::pluginHost::StackInfo info, 
	bool gui)
{
	const m::Plugin* p = m::pluginHost::getPluginByIndex(pluginIndex, info);

	if (p->hasEditor())
		return;

	/* Get the parent window first: the plug-in list. Then, if it exists, get
	the child window - the actual pluginWindow. */

	v::gdPluginList* parent = static_cast<v::gdPluginList*>(u::gui::getSubwindow(G_MainWin, WID_FX_LIST));
	if (parent == nullptr)
		return;
	v::gdPluginWindow* child = static_cast<v::gdPluginWindow*>(u::gui::getSubwindow(parent, p->getId() + 1));
	if (child == nullptr) 
		return;
	
	if (!gui) Fl::lock();
	child->updateParameters(!gui);
	if (!gui) Fl::unlock();
}
} // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


void addPlugin(int pluginListIndex, m::pluginHost::StackInfo info)
{
	if (pluginListIndex >= m::pluginManager::countAvailablePlugins())
		return;
	std::unique_ptr<m::Plugin> p = m::pluginManager::makePlugin(pluginListIndex);
	if (p != nullptr)
		m::pluginHost::addPlugin(std::move(p), info);
}


/* -------------------------------------------------------------------------- */


void swapPlugins(size_t index1, size_t index2, m::pluginHost::StackInfo info)
{
	m::pluginHost::swapPlugin(index1, index2, info);
}


/* -------------------------------------------------------------------------- */


void freePlugin(size_t pluginIndex, m::pluginHost::StackInfo info)
{
	m::pluginHost::freePlugin(pluginIndex, info);
}


/* -------------------------------------------------------------------------- */


void setProgram(size_t pluginIndex, int programIndex, m::pluginHost::StackInfo info)
{
	m::pluginHost::setPluginProgram(pluginIndex, programIndex, info); 
	updatePluginEditor_(pluginIndex, info, true); 
}


/* -------------------------------------------------------------------------- */


void setParameter(size_t pluginIndex, int paramIndex, float value, 
    m::pluginHost::StackInfo info, bool gui)
{
	m::pluginHost::setPluginParameter(pluginIndex, paramIndex, value, info); 
	updatePluginEditor_(pluginIndex, info, gui); 
}


/* -------------------------------------------------------------------------- */


void toggleBypass(size_t pluginIndex, m::pluginHost::StackInfo info)
{
	m::pluginHost::toggleBypass(pluginIndex, info);
}


/* -------------------------------------------------------------------------- */


void setPluginPathCb(void* data)
{
	gdBrowserDir* browser = (gdBrowserDir*) data;

	if (browser->getCurrentPath() == "") {
		gdAlert("Invalid path.");
		return;
	}

	if (!m::conf::pluginPath.empty() && m::conf::pluginPath.back() != ';')
		m::conf::pluginPath += ";";
	m::conf::pluginPath += browser->getCurrentPath();

	browser->do_callback();

	gdConfig* configWin = static_cast<gdConfig*>(u::gui::getSubwindow(G_MainWin, WID_CONFIG));
	configWin->refreshVstPath();
}

}}}; // giada::c::plugin::


#endif
