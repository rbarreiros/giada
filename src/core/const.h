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
 * will be useful, but WITHOUT ANY WARRANTY without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */


#ifndef G_CONST_H
#define G_CONST_H


/* -- environment ----------------------------------------------------------- */
#if defined(_WIN32)
	#define G_OS_WINDOWS
#elif defined(__APPLE__)
	#define G_OS_MAC
#elif defined(__linux__)
	#define G_OS_LINUX
#endif

#ifndef BUILD_DATE
	#define BUILD_DATE __DATE__
#endif



/* -- version --------------------------------------------------------------- */
constexpr auto G_APP_NAME      = "Giada";
constexpr auto G_VERSION_STR   = "0.15.4";
constexpr int  G_VERSION_MAJOR = 0;
constexpr int  G_VERSION_MINOR = 15;
constexpr int  G_VERSION_PATCH = 4;

constexpr auto CONF_FILENAME = "giada.conf";

#ifdef G_OS_WINDOWS
	#define G_SLASH '\\'
	#define G_SLASH_STR "\\"
#else
	#define G_SLASH '/'
	#define G_SLASH_STR "/"
#endif


/* -- GUI ------------------------------------------------------------------- */
#define G_GUI_REFRESH_RATE   1000/24
#define G_GUI_PLUGIN_RATE    0.05  // refresh rate for plugin GUI
#define G_GUI_FONT_SIZE_BASE 12
#define G_GUI_INNER_MARGIN   4
#define G_GUI_OUTER_MARGIN   8
#define G_GUI_UNIT           20    // base unit for elements 
#define G_GUI_CHANNEL_H_1    G_GUI_UNIT
#define G_GUI_CHANNEL_H_2    G_GUI_UNIT * 2
#define G_GUI_CHANNEL_H_3    G_GUI_UNIT * 4
#define G_GUI_CHANNEL_H_4    G_GUI_UNIT * 6
#define G_GUI_ZOOM_FACTOR    2


#define G_COLOR_RED       fl_rgb_color(28,  32,  80)
#define G_COLOR_BLUE      fl_rgb_color(113, 31,  31)
#define G_COLOR_RED_ALERT fl_rgb_color(239, 75,  53)

#define G_COLOR_LIGHT_2  fl_rgb_color(200, 200, 200)
#define G_COLOR_LIGHT_1  fl_rgb_color(170, 170, 170)
#define G_COLOR_GREY_4   fl_rgb_color(78,  78,  78)
#define G_COLOR_GREY_3   fl_rgb_color(54,  54,  54)
#define G_COLOR_GREY_2   fl_rgb_color(37,  37,  37)
#define G_COLOR_GREY_1_5 fl_rgb_color(28,  28,  28)
#define G_COLOR_GREY_1   fl_rgb_color(25,  25,  25)
#define G_COLOR_BLACK    fl_rgb_color(0,   0,   0)



/* -- MIN/MAX values -------------------------------------------------------- */
constexpr float G_MIN_BPM          = 20.0f;
constexpr auto  G_MIN_BPM_STR      = "20.0";
constexpr float G_MAX_BPM          = 999.0f;
constexpr auto  G_MAX_BPM_STR      = "999.0";
constexpr int   G_MAX_BEATS        = 32;
constexpr int   G_MAX_BARS         = 32;
constexpr int   G_MAX_QUANTIZE     = 8;
constexpr float G_MIN_DB_SCALE     = 60.0f;
constexpr int   G_MIN_COLUMN_WIDTH = 140;
constexpr float G_MAX_BOOST_DB     = 20.0f;
constexpr float G_MIN_PITCH        = 0.1f;
constexpr float G_MAX_PITCH        = 4.0f;
constexpr int   G_MAX_GRID_VAL     = 64;
constexpr int   G_MIN_BUF_SIZE     = 8;
constexpr int   G_MAX_BUF_SIZE     = 4096;
constexpr int   G_MIN_GUI_WIDTH    = 816;
constexpr int   G_MIN_GUI_HEIGHT   = 510;
constexpr int   G_MAX_IO_CHANS     = 2;
constexpr int   G_MAX_VELOCITY     = 0x7F;
constexpr int   G_MAX_MIDI_CHANS   = 16;
constexpr int   G_MAX_POLYPHONY    = 32;



/* -- kernel audio ---------------------------------------------------------- */
constexpr int G_SYS_API_NONE   = 0x00;  // 0000 0000
constexpr int G_SYS_API_JACK   = 0x01;  // 0000 0001
constexpr int G_SYS_API_ALSA   = 0x02;  // 0000 0010
constexpr int G_SYS_API_DS     = 0x04;  // 0000 0100
constexpr int G_SYS_API_ASIO   = 0x08;  // 0000 1000
constexpr int G_SYS_API_CORE   = 0x10;  // 0001 0000
constexpr int G_SYS_API_PULSE  = 0x20;  // 0010 0000
constexpr int G_SYS_API_WASAPI = 0x40;  // 0100 0000
constexpr int G_SYS_API_ANY    = 0x7F;  // 0111 1111



/* -- kernel midi ----------------------------------------------------------- */
constexpr int G_MIDI_API_JACK = 0x01;  // 0000 0001
constexpr int G_MIDI_API_ALSA = 0x02;  // 0000 0010



/* -- default system -------------------------------------------------------- */
#if defined(G_OS_LINUX)
	#define G_DEFAULT_SOUNDSYS	G_SYS_API_NONE
#elif defined(G_OS_WINDOWS)
	#define G_DEFAULT_SOUNDSYS 	G_SYS_API_DS
#elif defined(G_OS_MAC)
	#define G_DEFAULT_SOUNDSYS 	G_SYS_API_CORE
#endif

constexpr int   G_DEFAULT_SOUNDDEV_OUT      = 0;      // FIXME - please override with rtAudio::getDefaultDevice (or similar)
constexpr int   G_DEFAULT_SOUNDDEV_IN       = -1;     // no recording by default: input disabled
constexpr int   G_DEFAULT_MIDI_SYSTEM       = 0;
constexpr int   G_DEFAULT_MIDI_PORT_IN      = -1;
constexpr int   G_DEFAULT_MIDI_PORT_OUT     = -1;
constexpr int   G_DEFAULT_SAMPLERATE        = 44100;
constexpr int   G_DEFAULT_BUFSIZE           = 1024;
constexpr int   G_DEFAULT_BIT_DEPTH         = 32;     // float
constexpr float G_DEFAULT_VOL               = 1.0f;
constexpr float G_DEFAULT_PITCH             = 1.0f;
constexpr float G_DEFAULT_BOOST             = 1.0f;
constexpr float G_DEFAULT_OUT_VOL           = 1.0f;
constexpr float G_DEFAULT_IN_VOL            = 1.0f;
constexpr float G_DEFAULT_BPM               = 120.0f;
constexpr int   G_DEFAULT_BEATS             = 4;
constexpr int   G_DEFAULT_BARS              = 1;
constexpr int   G_DEFAULT_QUANTIZE          = 0;      // quantizer off
constexpr float G_DEFAULT_FADEOUT_STEP      = 0.01f;  // micro-fadeout speed
constexpr int   G_DEFAULT_COLUMN_WIDTH      = 380;
constexpr auto  G_DEFAULT_PATCH_NAME        = "(default patch)";
constexpr int   G_DEFAULT_MIDI_INPUT_UI_W   = 300;
constexpr int   G_DEFAULT_MIDI_INPUT_UI_H   = 350;
constexpr int   G_DEFAULT_ACTION_SIZE       = 8192;  // frames
constexpr int   G_DEFAULT_ZOOM_RATIO        = 128;
constexpr float G_DEFAULT_REC_TRIGGER_LEVEL = -10.0f;



/* -- responses and return codes -------------------------------------------- */
#define G_RES_ERR_PROCESSING    -6
#define G_RES_ERR_WRONG_DATA    -5
#define G_RES_ERR_NO_DATA       -4
#define G_RES_ERR_PATH_TOO_LONG -3
#define G_RES_ERR_IO            -2
#define G_RES_ERR_MEMORY        -1
#define G_RES_ERR                0
#define G_RES_OK                 1



/* -- log modes ------------------------------------------------------------- */
#define LOG_MODE_STDOUT 0x01
#define LOG_MODE_FILE   0x02
#define LOG_MODE_MUTE   0x04



/* -- unique IDs of mainWin's subwindows ------------------------------------ */
/* -- wid > 0 are reserved by gg_keyboard ----------------------------------- */
#define WID_BEATS         -1
#define WID_BPM           -2
#define WID_ABOUT         -3
#define WID_FILE_BROWSER  -4
#define WID_CONFIG        -5
#define WID_FX_LIST       -6
#define WID_ACTION_EDITOR -7
#define WID_SAMPLE_EDITOR -8
#define WID_FX            -9
#define WID_KEY_GRABBER   -10
#define WID_SAMPLE_NAME   -11



/* -- patch signals --------------------------------------------------------- */
#define PATCH_UNREADABLE    0x01
#define PATCH_INVALID       0x02
#define PATCH_READ_OK       0x04
#define PATCH_WRONG_PLUGINS 0x08  // currently unused
#define PATCH_WRONG_SAMPLES 0x10  // currently unused



/* -- midimap signals ------------------------------------------------------- */
#define MIDIMAP_NOT_SPECIFIED 0x00
#define MIDIMAP_UNREADABLE    0x01
#define MIDIMAP_INVALID       0x02
#define MIDIMAP_READ_OK       0x04



/* -- MIDI signals -------------------------------------------------------------
Channel voices messages - controller (0xB0) is a special subset of this family:
it drives knobs, volume, faders and such. */

#define MIDI_CONTROLLER     0xB0 << 24
#define MIDI_ALL_NOTES_OFF (MIDI_CONTROLLER) | (0x7B << 16)

/* system common / real-time messages. Single bytes */

#define MIDI_SYSEX          0xF0
#define MIDI_MTC_QUARTER    0xF1
#define MIDI_POSITION_PTR   0xF2
#define MIDI_CLOCK          0xF8
#define MIDI_START          0xFA
#define MIDI_CONTINUE       0xFB
#define MIDI_STOP           0xFC
#define MIDI_EOX            0xF7  // end of sysex

/* Channels */

constexpr int G_MIDI_CHANS[G_MAX_MIDI_CHANS] = {
	0x00 << 24,  0x01 << 24,  0x02 << 24,  0x03 << 24,
	0x04 << 24,  0x05 << 24,  0x06 << 24,  0x07 << 24,
	0x08 << 24,  0x09 << 24,  0x0A << 24,  0x0B << 24,
	0x0C << 24,  0x0D << 24,  0x0E << 24,  0x0F << 24
};

/* midi sync constants */

#define MIDI_SYNC_NONE      0x00
#define MIDI_SYNC_CLOCK_M   0x01  // master
#define MIDI_SYNC_CLOCK_S   0x02  // slave
#define MIDI_SYNC_MTC_M     0x04  // master
#define MIDI_SYNC_MTC_S     0x08  // slave

/* JSON patch keys */

constexpr auto PATCH_KEY_HEADER                       = "header";
constexpr auto PATCH_KEY_VERSION                      = "version";
constexpr auto PATCH_KEY_VERSION_MAJOR                = "version_major";
constexpr auto PATCH_KEY_VERSION_MINOR                = "version_minor";
constexpr auto PATCH_KEY_VERSION_PATCH                = "version_patch";
constexpr auto PATCH_KEY_NAME                         = "name";
constexpr auto PATCH_KEY_BPM                          = "bpm";
constexpr auto PATCH_KEY_BARS                         = "bars";
constexpr auto PATCH_KEY_BEATS                        = "beats";
constexpr auto PATCH_KEY_QUANTIZE                     = "quantize";
constexpr auto PATCH_KEY_MASTER_VOL_IN                = "master_vol_in";
constexpr auto PATCH_KEY_MASTER_VOL_OUT               = "master_vol_out";
constexpr auto PATCH_KEY_METRONOME                    = "metronome";
constexpr auto PATCH_KEY_LAST_TAKE_ID                 = "last_take_id";
constexpr auto PATCH_KEY_SAMPLERATE                   = "samplerate";
constexpr auto PATCH_KEY_COLUMNS                      = "columns";
constexpr auto PATCH_KEY_MASTER_OUT_PLUGINS           = "master_out_plugins";
constexpr auto PATCH_KEY_MASTER_IN_PLUGINS            = "master_in_plugins";
constexpr auto PATCH_KEY_CHANNELS                     = "channels";
constexpr auto PATCH_KEY_CHANNEL_TYPE                 = "type";
constexpr auto PATCH_KEY_CHANNEL_INDEX                = "index";
constexpr auto PATCH_KEY_CHANNEL_SIZE                 = "size";
constexpr auto PATCH_KEY_CHANNEL_NAME                 = "name";
constexpr auto PATCH_KEY_CHANNEL_COLUMN               = "column";
constexpr auto PATCH_KEY_CHANNEL_MUTE                 = "mute";
constexpr auto PATCH_KEY_CHANNEL_SOLO                 = "solo";
constexpr auto PATCH_KEY_CHANNEL_VOLUME               = "volume";
constexpr auto PATCH_KEY_CHANNEL_PAN                  = "pan";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN              = "midi_in";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_VELO_AS_VOL  = "midi_in_velo_as_vol";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_KEYPRESS     = "midi_in_keypress";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_KEYREL       = "midi_in_keyrel";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_KILL         = "midi_in_kill";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_ARM          = "midi_in_arm";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_VOLUME       = "midi_in_volume";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_MUTE         = "midi_in_mute";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_FILTER       = "midi_in_filter";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_SOLO         = "midi_in_solo";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT_L           = "midi_out_l";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT_L_PLAYING   = "midi_out_l_playing";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT_L_MUTE      = "midi_out_l_mute";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT_L_SOLO      = "midi_out_l_solo";
constexpr auto PATCH_KEY_CHANNEL_SAMPLE_PATH          = "sample_path";
constexpr auto PATCH_KEY_CHANNEL_KEY                  = "key";
constexpr auto PATCH_KEY_CHANNEL_MODE                 = "mode";
constexpr auto PATCH_KEY_CHANNEL_BEGIN                = "begin";
constexpr auto PATCH_KEY_CHANNEL_END                  = "end";
constexpr auto PATCH_KEY_CHANNEL_BOOST                = "boost";
constexpr auto PATCH_KEY_CHANNEL_READ_ACTIONS         = "rec_active";  // TODO update string key in 1.0
constexpr auto PATCH_KEY_CHANNEL_PITCH                = "pitch";
constexpr auto PATCH_KEY_CHANNEL_INPUT_MONITOR        = "input_monitor";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_READ_ACTIONS = "midi_in_read_actions";
constexpr auto PATCH_KEY_CHANNEL_MIDI_IN_PITCH        = "midi_in_pitch";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT             = "midi_out";
constexpr auto PATCH_KEY_CHANNEL_MIDI_OUT_CHAN        = "midi_out_chan";
constexpr auto PATCH_KEY_CHANNEL_PLUGINS              = "plugins";
constexpr auto PATCH_KEY_CHANNEL_ACTIONS              = "actions";
constexpr auto PATCH_KEY_CHANNEL_ARMED                = "armed";
constexpr auto PATCH_KEY_ACTION_TYPE                  = "type";
constexpr auto PATCH_KEY_ACTION_FRAME                 = "frame";
constexpr auto PATCH_KEY_ACTION_F_VALUE               = "f_value";
constexpr auto PATCH_KEY_ACTION_I_VALUE               = "i_value";
constexpr auto PATCH_KEY_PLUGIN_PATH                  = "path";
constexpr auto PATCH_KEY_PLUGIN_BYPASS                = "bypass";
constexpr auto PATCH_KEY_PLUGIN_PARAMS                = "params";
constexpr auto PATCH_KEY_PLUGIN_MIDI_IN_PARAMS        = "midi_in_params";
constexpr auto PATCH_KEY_COLUMN_INDEX                 = "index";
constexpr auto PATCH_KEY_COLUMN_WIDTH                 = "width";
constexpr auto PATCH_KEY_COLUMN_CHANNELS              = "channels";
constexpr auto G_PATCH_KEY_ACTION_ID                  = "id";
constexpr auto G_PATCH_KEY_ACTION_CHANNEL             = "channel";     
constexpr auto G_PATCH_KEY_ACTION_FRAME               = "frame";   
constexpr auto G_PATCH_KEY_ACTION_EVENT               = "event";   
constexpr auto G_PATCH_KEY_ACTION_PREV                = "prev";  
constexpr auto G_PATCH_KEY_ACTION_NEXT                = "next";

/* JSON config keys */

constexpr auto CONF_KEY_HEADER                   = "header";
constexpr auto CONF_KEY_LOG_MODE                 = "log_mode";
constexpr auto CONF_KEY_SOUND_SYSTEM             = "sound_system";
constexpr auto CONF_KEY_SOUND_DEVICE_IN          = "sound_device_in";
constexpr auto CONF_KEY_SOUND_DEVICE_OUT         = "sound_device_out";
constexpr auto CONF_KEY_CHANNELS_IN              = "channels_in";
constexpr auto CONF_KEY_CHANNELS_OUT             = "channels_out";
constexpr auto CONF_KEY_SAMPLERATE               = "samplerate";
constexpr auto CONF_KEY_BUFFER_SIZE              = "buffer_size";
constexpr auto CONF_KEY_DELAY_COMPENSATION       = "delay_compensation";
constexpr auto CONF_KEY_LIMIT_OUTPUT             = "limit_output";
constexpr auto CONF_KEY_RESAMPLE_QUALITY         = "resample_quality";
constexpr auto CONF_KEY_MIDI_SYSTEM              = "midi_system";
constexpr auto CONF_KEY_MIDI_PORT_OUT            = "midi_port_out";
constexpr auto CONF_KEY_MIDI_PORT_IN             = "midi_port_in";
constexpr auto CONF_KEY_MIDIMAP_PATH             = "midimap_path";
constexpr auto CONF_KEY_LAST_MIDIMAP             = "last_midimap";
constexpr auto CONF_KEY_MIDI_SYNC                = "midi_sync";
constexpr auto CONF_KEY_MIDI_TC_FPS              = "midi_tc_fps";
constexpr auto CONF_KEY_MIDI_IN                  = "midi_in";
constexpr auto CONF_KEY_MIDI_IN_FILTER           = "midi_in_filter";
constexpr auto CONF_KEY_MIDI_IN_REWIND           = "midi_in_rewind";
constexpr auto CONF_KEY_MIDI_IN_START_STOP       = "midi_in_start_stop";
constexpr auto CONF_KEY_MIDI_IN_ACTION_REC       = "midi_in_action_rec";
constexpr auto CONF_KEY_MIDI_IN_INPUT_REC        = "midi_in_input_rec";
constexpr auto CONF_KEY_MIDI_IN_METRONOME        = "midi_in_metronome";
constexpr auto CONF_KEY_MIDI_IN_VOLUME_IN        = "midi_in_volume_in";
constexpr auto CONF_KEY_MIDI_IN_VOLUME_OUT       = "midi_in_volume_out";
constexpr auto CONF_KEY_MIDI_IN_BEAT_DOUBLE      = "midi_in_beat_doble";
constexpr auto CONF_KEY_MIDI_IN_BEAT_HALF        = "midi_in_beat_half";
constexpr auto CONF_KEY_RECS_STOP_ON_CHAN_HALT   = "recs_stop_on_chan_halt";
constexpr auto CONF_KEY_CHANS_STOP_ON_SEQ_HALT   = "chans_stop_on_seq_halt";
constexpr auto CONF_KEY_TREAT_RECS_AS_LOOPS      = "treat_recs_as_loops";
constexpr auto CONF_KEY_INPUT_MONITOR_DEFAULT_ON = "input_monitor_default_on";
constexpr auto CONF_KEY_PLUGINS_PATH             = "plugins_path";
constexpr auto CONF_KEY_PATCHES_PATH             = "patches_path";
constexpr auto CONF_KEY_SAMPLES_PATH             = "samples_path";
constexpr auto CONF_KEY_MAIN_WINDOW_X            = "main_window_x";
constexpr auto CONF_KEY_MAIN_WINDOW_Y            = "main_window_y";
constexpr auto CONF_KEY_MAIN_WINDOW_W            = "main_window_w";
constexpr auto CONF_KEY_MAIN_WINDOW_H            = "main_window_h";
constexpr auto CONF_KEY_BROWSER_X                = "browser_x";
constexpr auto CONF_KEY_BROWSER_Y                = "browser_y";
constexpr auto CONF_KEY_BROWSER_W                = "browser_w";
constexpr auto CONF_KEY_BROWSER_H                = "browser_h";
constexpr auto CONF_KEY_BROWSER_POSITION         = "browser_position";
constexpr auto CONF_KEY_BROWSER_LAST_PATH        = "browser_last_path";
constexpr auto CONF_KEY_BROWSER_LAST_VALUE       = "browser_last_value";
constexpr auto CONF_KEY_ACTION_EDITOR_X          = "action_editor_x";
constexpr auto CONF_KEY_ACTION_EDITOR_Y          = "action_editor_y";
constexpr auto CONF_KEY_ACTION_EDITOR_W          = "action_editor_w";
constexpr auto CONF_KEY_ACTION_EDITOR_H          = "action_editor_h";
constexpr auto CONF_KEY_ACTION_EDITOR_ZOOM       = "action_editor_zoom";
constexpr auto CONF_KEY_ACTION_EDITOR_GRID_VAL   = "action_editor_grid_val";
constexpr auto CONF_KEY_ACTION_EDITOR_GRID_ON    = "action_editor_grid_on";
constexpr auto CONF_KEY_SAMPLE_EDITOR_X          = "sample_editor_x";
constexpr auto CONF_KEY_SAMPLE_EDITOR_Y          = "sample_editor_y";
constexpr auto CONF_KEY_SAMPLE_EDITOR_W          = "sample_editor_w";
constexpr auto CONF_KEY_SAMPLE_EDITOR_H          = "sample_editor_h";
constexpr auto CONF_KEY_SAMPLE_EDITOR_GRID_VAL   = "sample_editor_grid_val";
constexpr auto CONF_KEY_SAMPLE_EDITOR_GRID_ON    = "sample_editor_grid_on";
constexpr auto CONF_KEY_PIANO_ROLL_Y             = "piano_roll_y";
constexpr auto CONF_KEY_PIANO_ROLL_H             = "piano_roll_h";
constexpr auto CONF_KEY_SAMPLE_ACTION_EDITOR_H   = "sample_action_editor_h";
constexpr auto CONF_KEY_VELOCITY_EDITOR_H        = "velocity_editor_h";
constexpr auto CONF_KEY_ENVELOPE_EDITOR_H        = "envelope_editor_h";
constexpr auto CONF_KEY_PLUGIN_LIST_X            = "plugin_list_x";
constexpr auto CONF_KEY_PLUGIN_LIST_Y            = "plugin_list_y";
constexpr auto CONF_KEY_CONFIG_X                 = "config_x";
constexpr auto CONF_KEY_CONFIG_Y                 = "config_y";
constexpr auto CONF_KEY_BPM_X                    = "bpm_x";
constexpr auto CONF_KEY_BPM_Y                    = "bpm_y";
constexpr auto CONF_KEY_BEATS_X                  = "beats_x";
constexpr auto CONF_KEY_BEATS_Y                  = "beats_y";
constexpr auto CONF_KEY_ABOUT_X                  = "about_x";
constexpr auto CONF_KEY_ABOUT_Y                  = "about_y";
constexpr auto CONF_KEY_NAME_X                   = "name_x";
constexpr auto CONF_KEY_NAME_Y                   = "name_y";
constexpr auto CONF_KEY_PLUGIN_CHOOSER_X         = "plugin_chooser_x";
constexpr auto CONF_KEY_PLUGIN_CHOOSER_Y         = "plugin_chooser_y";
constexpr auto CONF_KEY_PLUGIN_CHOOSER_W         = "plugin_chooser_w";
constexpr auto CONF_KEY_PLUGIN_CHOOSER_H         = "plugin_chooser_h";
constexpr auto CONF_KEY_MIDI_INPUT_X             = "midi_input_x";
constexpr auto CONF_KEY_MIDI_INPUT_Y             = "midi_input_y";
constexpr auto CONF_KEY_MIDI_INPUT_W             = "midi_input_w";
constexpr auto CONF_KEY_MIDI_INPUT_H             = "midi_input_h";
constexpr auto CONF_KEY_PLUGIN_SORT_METHOD       = "plugin_sort_method";
constexpr auto CONF_KEY_REC_TRIGGER_MODE         = "rec_trigger_mode";
constexpr auto CONF_KEY_REC_TRIGGER_LEVEL        = "rec_trigger_level";

/* JSON midimaps keys */

constexpr auto MIDIMAP_KEY_BRAND             = "brand";
constexpr auto MIDIMAP_KEY_DEVICE            = "device";
constexpr auto MIDIMAP_KEY_INIT_COMMANDS     = "init_commands";
constexpr auto MIDIMAP_KEY_MUTE_ON           = "mute_on";
constexpr auto MIDIMAP_KEY_MUTE_OFF          = "mute_off";
constexpr auto MIDIMAP_KEY_SOLO_ON           = "solo_on";
constexpr auto MIDIMAP_KEY_SOLO_OFF          = "solo_off";
constexpr auto MIDIMAP_KEY_WAITING           = "waiting";
constexpr auto MIDIMAP_KEY_PLAYING           = "playing";
constexpr auto MIDIMAP_KEY_PLAYING_INAUDIBLE = "playing_inaudible";
constexpr auto MIDIMAP_KEY_STOPPING          = "stopping";
constexpr auto MIDIMAP_KEY_STOPPED           = "stopped";
constexpr auto MIDIMAP_KEY_CHANNEL           = "channel";
constexpr auto MIDIMAP_KEY_MESSAGE           = "message";

#endif
