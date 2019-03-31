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
class geResizerBar;


namespace giada {
namespace v
{
class geChannel;
class geSampleChannel;

class geKeyboard : public Fl_Scroll
{
public:

	geKeyboard(int X, int Y, int W, int H);

	int handle(int e) override;

	/* rebuild
	Rebuilds this widget from scratch. Used when the model has changed. */

	void rebuild();

	/* refresh
	Refreshes each column's channel, called on each GUI cycle. */

	void refresh();

	/* addColumn
	 * add a new column to the top of the stack. */

	void addColumn(int width=380);

	/* organizeColumns
	 * reorganize columns layout by removing empty gaps. */

	void organizeColumns();

	/* printChannelMessage
	 * given any output by glue_loadChannel, print the message on screen
	 * on a gdAlert subwindow. */

	void printChannelMessage(int res);

	/* getTotalColumns */

	//unsigned getTotalColumns() { return columns.size(); }

	/* getChannel
	Given a chanIndex returns the UI channel it belongs to. */

	geChannel* getChannel(size_t chanIndex);

private:

	static const int COLUMN_GAP = 20;

	/* indexColumn
	The last index used for column. */

	static int indexGen;
	
	/* init
	Builds the initial setup of empty channels. */

	void init();

	/* refreshColIndexes
	 * Recompute all column indexes in order to avoid any gaps between them.
	 * Indexes must always be contiguous! */

	void refreshColIndexes();

	static void cb_addColumn(Fl_Widget* v, void* p);
	geColumn* cb_addColumn(int width=G_DEFAULT_COLUMN_WIDTH, int index=-1);

	geColumn* getColumn(int index) const;

	geButton* addColumnBtn;
	
	geResizerBar* m_lastResizerBar;
};
}} // giada::v::


#endif
