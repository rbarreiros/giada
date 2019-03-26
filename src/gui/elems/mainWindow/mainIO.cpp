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


#include "core/const.h"
#include "core/graphics.h"
#include "core/mixer.h"
#include "core/pluginHost.h"
#include "glue/main.h"
#include "utils/gui.h"
#include "gui/elems/soundMeter.h"
#include "gui/elems/basics/statusButton.h"
#include "gui/elems/basics/dial.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/pluginList.h"
#include "mainIO.h"


extern gdMainWindow* G_MainWin;


namespace giada {
namespace v
{
geMainIO::geMainIO(int x, int y)
	: Fl_Group(x, y, 396, 20)
{
	begin();

#if defined(WITH_VST)
	masterFxIn  = new geStatusButton(x, y, 20, 20, fxOff_xpm, fxOn_xpm);
	inVol       = new geDial        (masterFxIn->x()+masterFxIn->w()+4, y, 20, 20);
	inMeter     = new geSoundMeter  (inVol->x()+inVol->w()+4, y+4, 140, 12);
	inToOut     = new geButton      (inMeter->x()+inMeter->w()+4, y+4, 12, 12, "", inputToOutputOff_xpm, inputToOutputOn_xpm);
	outMeter    = new geSoundMeter  (inToOut->x()+inToOut->w()+4, y+4, 140, 12);
	outVol      = new geDial        (outMeter->x()+outMeter->w()+4, y, 20, 20);
	masterFxOut = new geStatusButton(outVol->x()+outVol->w()+4, y, 20, 20, fxOff_xpm, fxOn_xpm);
#else
	inVol       = new geDial      (x+62, y, 20, 20);
	inMeter     = new geSoundMeter(inVol->x()+inVol->w()+4, y+5, 140, 12);
	outMeter    = new geSoundMeter(inMeter->x()+inMeter->w()+4, y+5, 140, 12);
	outVol      = new geDial      (outMeter->x()+outMeter->w()+4, y, 20, 20);
#endif

	end();

	resizable(nullptr);   // don't resize any widget

	outVol->callback(cb_outVol, (void*)this);
	outVol->value(m::mixer::outVol.load());
	inVol->callback(cb_inVol, (void*)this);
	inVol->value(m::mixer::inVol.load());

#ifdef WITH_VST
	masterFxOut->callback(cb_masterFxOut, (void*)this);
	masterFxIn->callback(cb_masterFxIn, (void*)this);
	inToOut->callback(cb_inToOut, (void*)this);
	inToOut->type(FL_TOGGLE_BUTTON);
#endif
}


/* -------------------------------------------------------------------------- */


void geMainIO::cb_outVol     (Fl_Widget *v, void *p)  	{ ((geMainIO*)p)->cb_outVol(); }
void geMainIO::cb_inVol      (Fl_Widget *v, void *p)  	{ ((geMainIO*)p)->cb_inVol(); }
#ifdef WITH_VST
void geMainIO::cb_masterFxOut(Fl_Widget *v, void *p)    { ((geMainIO*)p)->cb_masterFxOut(); }
void geMainIO::cb_masterFxIn (Fl_Widget *v, void *p)    { ((geMainIO*)p)->cb_masterFxIn(); }
void geMainIO::cb_inToOut    (Fl_Widget *v, void *p)    { ((geMainIO*)p)->cb_inToOut(); }
#endif


/* -------------------------------------------------------------------------- */


void geMainIO::cb_outVol()
{
	c::main::setOutVol(outVol->value());
}


/* -------------------------------------------------------------------------- */


void geMainIO::cb_inVol()
{
	c::main::setInVol(inVol->value());
}


/* -------------------------------------------------------------------------- */


#ifdef WITH_VST

void geMainIO::cb_masterFxOut()
{
//	m::pluginHost::Stack stack = m::pluginHost::getStack(m::pluginHost::StackType::MASTER_OUT);
//	u::gui::openSubWindow(G_MainWin, new gdPluginList(stack.type, stack.chanIndex), WID_FX_LIST);
}


void geMainIO::cb_masterFxIn()
{
//	m::pluginHost::Stack stack = m::pluginHost::getStack(m::pluginHost::StackType::MASTER_IN);
//	u::gui::openSubWindow(G_MainWin, new gdPluginList(stack.type, stack.chanIndex), WID_FX_LIST);
}


void geMainIO::cb_inToOut()
{
	m::mixer::inToOut = inToOut->value();
}

#endif


/* -------------------------------------------------------------------------- */


void geMainIO::setOutVol(float v)
{
  outVol->value(v);
}


void geMainIO::setInVol(float v)
{
  inVol->value(v);
}


/* -------------------------------------------------------------------------- */


#ifdef WITH_VST

void geMainIO::setMasterFxOutFull(bool v)
{
  masterFxOut->status = v;
  masterFxOut->redraw();
}


void geMainIO::setMasterFxInFull(bool v)
{
  masterFxIn->status = v;
  masterFxIn->redraw();
}

#endif


/* -------------------------------------------------------------------------- */


void geMainIO::refresh()
{
	outMeter->mixerPeak = m::mixer::peakOut.load();
	inMeter->mixerPeak  = m::mixer::peakIn.load();
	outMeter->redraw();
	inMeter->redraw();
}

}} // giada::v::