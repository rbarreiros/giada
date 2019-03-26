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


#include <thread>
#include <atomic>
#include <ctime>
#ifdef __APPLE__
	#include <pwd.h>
#endif
#if defined(__linux__) && defined(WITH_VST)
	#include <X11/Xlib.h> // For XInitThreads
#endif
#include <FL/Fl.H>
#include "gui/updater.h"
#include "utils/log.h"
#include "utils/fs.h"
#include "utils/time.h"
#include "utils/gui.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/warnings.h"
#include "glue/main.h"
#include "core/mixer.h"
#include "core/wave.h"
#include "core/const.h"
#include "core/clock.h"
#include "core/channel.h"
#include "core/mixerHandler.h"
#include "core/patch.h"
#include "core/conf.h"
#include "core/pluginManager.h"
#include "core/pluginHost.h"
#include "core/recorder.h"
#include "core/recManager.h"
#include "core/midiMapConf.h"
#include "core/kernelMidi.h"
#include "core/kernelAudio.h"
#include "init.h"


extern std::atomic<bool> G_quit;
extern gdMainWindow*     G_MainWin;


namespace giada {
namespace m {
namespace init
{
namespace
{
void initConf_()
{
	if (!conf::read())
		gu_log("[init] Can't read configuration file! Using default values\n");
	
	patch::init();
	midimap::init();
	midimap::setDefault();
	
	if (!gu_logInit(conf::logMode))
		gu_log("[init] log init failed! Using default stdout\n");

	if (midimap::read(conf::midiMapPath) != MIDIMAP_READ_OK)
		gu_log("[init] MIDI map read failed!\n");
}


/* -------------------------------------------------------------------------- */


void initAudio_()
{
	kernelAudio::openDevice();
	clock::init(conf::samplerate, conf::midiTCfps);
	mixer::init(clock::getFramesInLoop(), kernelAudio::getRealBufSize());
	recorder::init(&mixer::mutex);
	recManager::init(&mixer::mutex);

#ifdef WITH_VST

	pluginManager::init(conf::samplerate, kernelAudio::getRealBufSize());
	pluginManager::sortPlugins(static_cast<pluginManager::SortMethod>(conf::pluginSortMethod));
	pluginHost::init(kernelAudio::getRealBufSize());

#endif

	if (!kernelAudio::getStatus())
		return;

	kernelAudio::startStream();
}


/* -------------------------------------------------------------------------- */


void initMIDI_()
{
	kernelMidi::setApi(conf::midiSystem);
	kernelMidi::openOutDevice(conf::midiPortOut);
	kernelMidi::openInDevice(conf::midiPortIn);	
}


/* -------------------------------------------------------------------------- */


void initGUI_(int argc, char** argv)
{
	/* This enables the FLTK lock and start the runtime multithreading support. */

	Fl::lock();

	/* This is of paramount importance on Linux with VST enabled, otherwise many
	plug-ins go nuts and crash hard. It seems that some plug-ins or our Juce-based
	PluginHost use Xlib concurrently. */
	
#if defined(__linux__) && defined(WITH_VST)
	XInitThreads();
#endif

	G_MainWin = new gdMainWindow(G_MIN_GUI_WIDTH, G_MIN_GUI_HEIGHT, "", argc, argv);
	G_MainWin->resize(conf::mainWindowX, conf::mainWindowY, conf::mainWindowW,
		conf::mainWindowH);

	u::gui::updateMainWinLabel(patch::name == "" ? G_DEFAULT_PATCH_NAME : patch::name);

	if (!kernelAudio::getStatus())
		gdAlert("Your soundcard isn't configured correctly.\n"
			"Check the configuration and restart Giada.");

	u::gui::updateControls();

	Fl::add_timeout(G_GUI_REFRESH_RATE, v::updater::update, nullptr);
}


/* -------------------------------------------------------------------------- */


void shutdownAudio_()
{
#ifdef WITH_VST

	pluginHost::close();
	gu_log("[init] PluginHost cleaned up\n");

#endif

	if (kernelAudio::getStatus()) {
		kernelAudio::closeDevice();
		gu_log("[init] KernelAudio closed\n");
		mixer::close();
		gu_log("[init] Mixer closed\n");
	}
}


/* -------------------------------------------------------------------------- */


void shutdownGUI_()
{
	u::gui::closeAllSubwindows();

	gu_log("[init] All subwindows and UI thread closed\n");
}
} // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


void startup(int argc, char** argv)
{
	time_t t;
	time (&t);
	gu_log("[init] Giada %s - %s", G_VERSION_STR, ctime(&t));

	initConf_();
	initAudio_();
	initMIDI_();
	initGUI_(argc, argv);
}


/* -------------------------------------------------------------------------- */


void closeMainWindow()
{
	if (!gdConfirmWin("Warning", "Quit Giada: are you sure?"))
		return;

	G_MainWin->hide();
	delete G_MainWin;
}


/* -------------------------------------------------------------------------- */


void shutdown()
{
	G_quit.store(true);

	shutdownGUI_();

	if (!conf::write())
		gu_log("[init] error while saving configuration file!\n");
	else
		gu_log("[init] configuration saved\n");

	shutdownAudio_();

	gu_log("[init] Giada %s closed\n\n", G_VERSION_STR);
	gu_logClose();
}
}}} // giada::m::init
