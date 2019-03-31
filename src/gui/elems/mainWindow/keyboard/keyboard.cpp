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


#include <cassert>
#include "core/model/model.h"
#include "core/sampleChannel.h"
#include "glue/transport.h"
#include "glue/io.h"
#include "utils/log.h"
#include "gui/dispatcher.h"
#include "gui/dialogs/warnings.h"
#include "gui/elems/basics/boxtypes.h"
#include "gui/elems/basics/resizerBar.h"
#include "column.h"
#include "sampleChannel.h"
#include "channelButton.h"
#include "keyboard.h"


namespace giada {
namespace v
{
int geKeyboard::indexGen = 0;


/* -------------------------------------------------------------------------- */


geKeyboard::geKeyboard(int X, int Y, int W, int H)
: Fl_Scroll          (X, Y, W, H),
	addColumnBtn    (nullptr),
	m_lastResizerBar(nullptr)
{
	color(G_COLOR_GREY_1);
	type(Fl_Scroll::BOTH_ALWAYS);
	scrollbar.color(G_COLOR_GREY_2);
	scrollbar.selection_color(G_COLOR_GREY_4);
	scrollbar.labelcolor(G_COLOR_LIGHT_1);
	scrollbar.slider(G_CUSTOM_BORDER_BOX);
	hscrollbar.color(G_COLOR_GREY_2);
	hscrollbar.selection_color(G_COLOR_GREY_4);
	hscrollbar.labelcolor(G_COLOR_LIGHT_1);
	hscrollbar.slider(G_CUSTOM_BORDER_BOX);

	addColumnBtn = new geButton(8, y(), 200, 20, "Add new column");
	addColumnBtn->callback(cb_addColumn, (void*) this);
	add(addColumnBtn);

	//init();
}


/* -------------------------------------------------------------------------- */


void geKeyboard::init()
{
	/* add 6 empty columns as init layout */

	cb_addColumn();
	cb_addColumn();
	cb_addColumn();
	cb_addColumn();
	cb_addColumn();
	cb_addColumn();
}


/* -------------------------------------------------------------------------- */


void geKeyboard::rebuild()
{
	const std::vector<std::unique_ptr<m::Channel>>& channels = m::model::get()->channels;

	/**/
	clear();
	m_lastResizerBar = nullptr;
	addColumnBtn = new geButton(8, y(), 200, 20, "Add new column");
	addColumnBtn->callback(cb_addColumn, (void*) this);
	add(addColumnBtn);
	/**/


	if (channels.size() == 0) {
		init();
	}
	else {
		for (const std::unique_ptr<m::Channel>& ch : channels) {
			printf("add channel to columnIndex=%ld\n", ch->columnIndex);
			geColumn* column = getColumn(ch->columnIndex);
			if (column == nullptr) { // Column not found
				column = cb_addColumn(G_DEFAULT_COLUMN_WIDTH, ch->columnIndex);
			}
			assert(column != nullptr);
			column->addChannel(ch.get(), G_GUI_CHANNEL_H_1);
		}
	}

}


/* -------------------------------------------------------------------------- */


geColumn* geKeyboard::getColumn(int index) const
{
	for (int i=0; i<children(); i++) {
		geColumn* col = dynamic_cast<geColumn*>(child(i));
		if (col != nullptr && col->getIndex() == index)
			return col;
	}
	return nullptr;
}


/* -------------------------------------------------------------------------- */


void geKeyboard::organizeColumns()
{
#if 0
	if (columns.size() == 0)
		return;

	/* Otherwise delete all empty columns. */

	for (size_t i=columns.size(); i-- > 0;) {
		if (columns.at(i)->isEmpty()) {
			Fl::delete_widget(columns.at(i));
			columns.erase(columns.begin() + i);
		}
	}

	/* Zero columns? Just add the "add column" button. Compact column and avoid 
	empty spaces otherwise. */

	if (columns.size() == 0)
		addColumnBtn->position(x() - xposition(), y());
	else {
		for (size_t i=0; i<columns.size(); i++) {
			int pos = i == 0 ? x() - xposition() : columns.at(i-1)->x() + columns.at(i-1)->w() + COLUMN_GAP;
			columns.at(i)->position(pos, y());
		}
		addColumnBtn->position(columns.back()->x() + columns.back()->w() + COLUMN_GAP, y());
	}

	refreshColIndexes();
	redraw();
#endif
}


/* -------------------------------------------------------------------------- */


void geKeyboard::cb_addColumn(Fl_Widget* v, void* p)
{
	((geKeyboard*)p)->cb_addColumn(G_DEFAULT_COLUMN_WIDTH);
}


/* -------------------------------------------------------------------------- */


void geKeyboard::refresh()
{
	/*
	for (geColumn* c : columns)
		c->refresh();*/
}


/* -------------------------------------------------------------------------- */


int geKeyboard::handle(int e)
{
	switch (e) {
		case FL_FOCUS:
		case FL_UNFOCUS: {
			return 1;               // Enables receiving Keyboard events
		}
		case FL_SHORTCUT:           // In case widget that isn't ours has focus
		case FL_KEYDOWN:            // Keyboard key pushed
		case FL_KEYUP: {            // Keyboard key released
			dispatcher::dispatchKey(e);
			return 1;
		}
	}
	return Fl_Group::handle(e);     // Assume the buttons won't handle the Keyboard events
}


/* -------------------------------------------------------------------------- */


void geKeyboard::printChannelMessage(int res)
{
	if      (res == G_RES_ERR_WRONG_DATA)
		gdAlert("Multichannel samples not supported.");
	else if (res == G_RES_ERR_IO)
		gdAlert("Unable to read this sample.");
	else if (res == G_RES_ERR_PATH_TOO_LONG)
		gdAlert("File path too long.");
	else if (res == G_RES_ERR_NO_DATA)
		gdAlert("No file specified.");
	else
		gdAlert("Unknown error.");
}


/* -------------------------------------------------------------------------- */


geColumn* geKeyboard::cb_addColumn(int width, int index)
{
	int colx = x() - xposition();  // Mind the x-scroll offset with xposition()

	/* If this is not the first column... */

	if (m_lastResizerBar != nullptr)
		colx = m_lastResizerBar->x() + m_lastResizerBar->w();

	/* Generate new index. If not passed in. */

	int newIndex; 
	if (index != -1) {
		newIndex = index;
		if (indexGen < newIndex) indexGen = newIndex;
	}
	else
		newIndex = indexGen++;

	/* Add a new column + a new resizer bar. */

	geColumn* column = new geColumn(colx, y(), width, h(), newIndex);
	m_lastResizerBar = new geResizerBar(colx + width, y(), COLUMN_GAP, h(), G_MIN_COLUMN_WIDTH, geResizerBar::HORIZONTAL);
	add(column);
	add(m_lastResizerBar);

	/* And then shift the "add column" button on the rightmost edge. */

	addColumnBtn->position(colx + width + COLUMN_GAP, y());

	redraw();

	return column;
}


/* -------------------------------------------------------------------------- */


void geKeyboard::addColumn(int width)
{
	cb_addColumn(width);
}


/* -------------------------------------------------------------------------- */


void geKeyboard::refreshColIndexes()
{
	/*
	for (unsigned i=0; i<columns.size(); i++)
		columns.at(i)->setIndex(i);*/
}


/* -------------------------------------------------------------------------- */


geChannel* geKeyboard::getChannel(size_t chanIndex)
{
#if 0
	/* TODO - temporary, raw, naive and laughable. To be changed soon! */
	for (geColumn* column : columns)
		for (int i=1; i<column->children(); i++) {
			geChannel* gch = static_cast<geChannel*>(column->child(i));
			if (gch->ch->index == chanIndex)
				return gch;
		}
	assert(false);
	return nullptr;
#endif
}

}} // giada::v::