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


#ifndef GE_KEYBOARD_H
#define GE_KEYBOARD_H


#include <vector>
#include <FL/Fl_Scroll.H>
#include "core/const.h"
#include "core/channel.h"


class geButton;
class geColumn;
class geChannel;
class geSampleChannel;


namespace giada {
namespace v
{
class geKeyboard : public Fl_Scroll
{
public:

	geKeyboard(int X, int Y, int W, int H);

	int handle(int e) override;

	/* init
	Builds the initial setup of empty channels. */

	void init();

	/* rebuild
	Rebuilds this widget from scratch. Used when the model has changed. */

	void rebuild();

	/* refresh
	Refreshes each column's channel, called on each GUI cycle. */

	void refresh();

	/* addChannel
	Adds a new channel to geChannels. Used by callbacks and during patch loading. 
	Requires Channel (and not geChannel). If build is set to true, also generate 
	the corresponding column if column (index) does not exist yet. */

	geChannel* addChannel(int column, giada::m::Channel* ch, int size, bool build=false);

	/* addColumn
	 * add a new column to the top of the stack. */

	void addColumn(int width=380);

	/* deleteChannel
	 * delete a channel from geChannels<> where geChannel->ch == ch and remove
	 * it from the stack. */

	void deleteChannel(geChannel* gch);

	/* freeChannel
	 * free a channel from geChannels<> where geChannel->ch == ch. No channels
	 * are deleted */

	void freeChannel(geChannel* gch);

	/* updateChannel
	 * wrapper function to call gch->update(). */

	void updateChannel(geChannel* gch);

	/* organizeColumns
	 * reorganize columns layout by removing empty gaps. */

	void organizeColumns();

	/* getColumnByIndex
	 * return the column with index 'index', or nullptr if not found. */

	geColumn* getColumnByIndex(int index);

	/* getColumn
	 * return the column with from columns->at(i). */

	geColumn* getColumn(int i);

	/* clear
	 * delete all channels and groups. */

	void clear();

	/* setChannelWithActions
	 * add 'R' button if channel has actions, and set recorder to active. */

	void setChannelWithActions(geSampleChannel* gch);

	/* printChannelMessage
	 * given any output by glue_loadChannel, print the message on screen
	 * on a gdAlert subwindow. */

	void printChannelMessage(int res);

	/* getTotalColumns */

	unsigned getTotalColumns() { return columns.size(); }

	/* getColumnWidth
	 * return the width in pixel of i-th column. Warning: 'i' is the i-th column
	 * in the column array, NOT the index. */

	int getColumnWidth(int i);

	/* getChannel
	Given a chanIndex returns the UI channel it belongs to. */

	geChannel* getChannel(size_t chanIndex);

private:

	static const int COLUMN_GAP = 16;

	void emptyColumns();
	
	/* refreshColIndexes
	 * Recompute all column indexes in order to avoid any gaps between them.
	 * Indexes must always be contiguous! */

	void refreshColIndexes();

	static void cb_addColumn(Fl_Widget* v, void* p);
	void cb_addColumn(int width=G_DEFAULT_COLUMN_WIDTH);

	/* indexColumn
	 * the last index used for column. */

	static int indexColumn;

	geButton* addColumnBtn;

	std::vector<geColumn*> columns;

};
}} // giada::v::


#endif
