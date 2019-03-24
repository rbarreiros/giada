#include "midiChannel.h"
#include "pluginHost.h"
#include "kernelMidi.h"
#include "const.h"
#include "action.h"
#include "midiChannelProc.h"
#include "mixerHandler.h"

namespace giada {
namespace m {
namespace midiChannelProc
{
namespace
{
void onFirstBeat_(MidiChannel* ch)
{
	if (ch->status == ChannelStatus::ENDING) {
		ch->status = ChannelStatus::OFF;
		ch->sendMidiLstatus();
	}
	else
	if (ch->status == ChannelStatus::WAIT) {
		ch->status = ChannelStatus::PLAY;
		ch->sendMidiLstatus();
	}
}
}; // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


void parseEvents(MidiChannel* ch, mixer::FrameEvents fe)
{
	if (fe.onFirstBeat)
		onFirstBeat_(ch);
	for (const Action* action : fe.actions)
		if (action->channel == ch->index)
			ch->sendMidi(action, fe.frameLocal);
}


/* -------------------------------------------------------------------------- */


void process(MidiChannel* ch, AudioBuffer& out, const AudioBuffer& in, bool audible)
{
#ifdef WITH_VST
	pluginHost::processStack(ch->buffer, pluginHost::StackType::CHANNEL, ch);
#endif

	/* Process the plugin stack first, then quit if the channel is muted/soloed. 
	This way there's no risk of cutting midi event pairs such as note-on and 
	note-off while triggering a mute/solo. */

	/* TODO - this is meaningful only if WITH_VST is defined */
	if (audible)
		for (int i=0; i<out.countFrames(); i++)
			for (int j=0; j<out.countChannels(); j++)
				out[i][j] += ch->buffer[i][j] * ch->volume;	
}


/* -------------------------------------------------------------------------- */


void start(MidiChannel* ch)
{
	switch (ch->status) {
		case ChannelStatus::PLAY:
			ch->status = ChannelStatus::ENDING;
			ch->sendMidiLstatus();
			break;

		case ChannelStatus::ENDING:
		case ChannelStatus::WAIT:
			ch->status = ChannelStatus::OFF;
			ch->sendMidiLstatus();
			break;

		case ChannelStatus::OFF:
			ch->status = ChannelStatus::WAIT;
			ch->sendMidiLstatus();
			break;

		default: break;
	}	
}


/* -------------------------------------------------------------------------- */


void kill(MidiChannel* ch, int localFrame)
{
	if (ch->isPlaying()) {
		if (ch->midiOut)
			kernelMidi::send(MIDI_ALL_NOTES_OFF);
#ifdef WITH_VST
		ch->addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
#endif
	}
	ch->status = ChannelStatus::OFF;
	ch->sendMidiLstatus();
}


/* -------------------------------------------------------------------------- */


void rewindBySeq(MidiChannel* ch)
{
	if (ch->midiOut)
		kernelMidi::send(MIDI_ALL_NOTES_OFF);
#ifdef WITH_VST
		ch->addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
#endif	
}


/* -------------------------------------------------------------------------- */


void setMute(MidiChannel* ch, bool v)
{
	ch->mute.store(v);
	if (v) {
		if (ch->midiOut)
			kernelMidi::send(MIDI_ALL_NOTES_OFF);
	#ifdef WITH_VST
			ch->addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
	#endif		
	}

	// This is for processing playing_inaudible
	ch->sendMidiLstatus();	

	ch->sendMidiLmute();
}


/* -------------------------------------------------------------------------- */


void setSolo(MidiChannel* ch, bool v)
{
	ch->solo.store(v);
	m::mh::updateSoloCount();

	// This is for processing playing_inaudible
	for (Channel* channel : mixer::channels)		
		channel->sendMidiLstatus();

	ch->sendMidiLsolo();
}


/* -------------------------------------------------------------------------- */


void stopBySeq(MidiChannel* ch)
{
	kill(ch, 0);
}
}}};
