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


#include "../../glue/plugin.h"
#include "../../utils/gui.h"
#include "../../core/channel.h"
#include "../../core/conf.h"
#include "../../core/pluginManager.h"
#include "../../core/pluginHost.h"
#include "../elems/plugin/pluginBrowser.h"
#include "../elems/basics/button.h"
#include "../elems/basics/choice.h"
#include "../elems/basics/box.h"
#include "pluginChooser.h"


using namespace giada;
using namespace giada::m;
using namespace giada::c;


gdPluginChooser::gdPluginChooser(int X, int Y, int W, int H, pluginHost::StackType t, Channel* ch)
  : gdWindow(X, Y, W, H, "Available plugins"), ch(ch), stackType(t)
{
  /* top area */
  Fl_Group *group_top = new Fl_Group(8, 8, w()-16, 20);
  sortMethod = new geChoice(group_top->x() + 45, group_top->y(), 100, 20, "Sort by");
    geBox *b1 = new geBox(sortMethod->x()+sortMethod->w(), group_top->y(), 100, 20); 	// spacer window border <-> menu
  group_top->resizable(b1);
  group_top->end();

  /* center browser */
  browser = new gePluginBrowser(8, 36, w()-16, h()-70);

  /* ok/cancel buttons */
  Fl_Group *group_btn = new Fl_Group(8, browser->y()+browser->h()+8, w()-16, h()-browser->h()-16);
    geBox *b2 = new geBox(8, browser->y()+browser->h(), 100, 20); 	// spacer window border <-> buttons
    addBtn = new geButton(w()-88, group_btn->y(), 80, 20, "Add");
    cancelBtn = new geButton(addBtn->x()-88, group_btn->y(), 80, 20, "Cancel");
  group_btn->resizable(b2);
  group_btn->end();

  end();

  sortMethod->add("Name");
  sortMethod->add("Category");
  sortMethod->add("Manufacturer");
  sortMethod->callback(cb_sort, (void*) this);
  sortMethod->value(conf::pluginSortMethod);

  addBtn->callback(cb_add, (void*) this);
  addBtn->shortcut(FL_Enter);
  cancelBtn->callback(cb_close, (void*) this);

  resizable(browser);
	u::gui::setFavicon(this);
  show();
}


/* -------------------------------------------------------------------------- */


gdPluginChooser::~gdPluginChooser()
{
  conf::pluginChooserX = x();
  conf::pluginChooserY = y();
  conf::pluginChooserW = w();
  conf::pluginChooserH = h();
  conf::pluginSortMethod = sortMethod->value();
}


/* -------------------------------------------------------------------------- */


void gdPluginChooser::cb_close(Fl_Widget* v, void* p) { ((gdPluginChooser*)p)->cb_close(); }
void gdPluginChooser::cb_add(Fl_Widget* v, void* p)   { ((gdPluginChooser*)p)->cb_add(); }
void gdPluginChooser::cb_sort(Fl_Widget* v, void* p)  { ((gdPluginChooser*)p)->cb_sort(); }


/* -------------------------------------------------------------------------- */


void gdPluginChooser::cb_close()
{
	do_callback();
}


/* -------------------------------------------------------------------------- */


void gdPluginChooser::cb_sort()
{
	pluginManager::sortPlugins(static_cast<pluginManager::SortMethod>(sortMethod->value()));
  browser->refresh();
}


/* -------------------------------------------------------------------------- */


void gdPluginChooser::cb_add()
{
  int index = browser->value() - 3; // subtract header lines
  if (index < 0)
    return;
  plugin::addPlugin(ch, index, stackType);
  do_callback();
}

#endif // #ifdef WITH_VST
