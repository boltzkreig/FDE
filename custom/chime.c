/*
 * BUILD: gcc -o chime chime.c -lespeak-ng -O3
 * A simple imitation of MacOS clock's time readout.
 * Tells time every 15 minutes; change interval if needed.
 * Run as a deamon in '.xinitrc' or equivalent of you WM.
 * Written By: Boltzkreig
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <espeak-ng/speak_lib.h>

espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
char *path = NULL;
void* user_data;
unsigned int *identifier;
const unsigned int interval = 15 * 60;

int main() {
	time_t current_time;
	time(&current_time);

	int hour              = 0,   minute = 0;
	struct tm *timeinfo;
	char text[50];

	int buflength         = 500, options       = 0;
	unsigned int position = 0,   position_type = 0, end_position = 0, flags = espeakCHARS_AUTO;
	espeak_Initialize(output, buflength, path, options);

	espeak_VOICE voice;
	memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
	const char *langNativeString = "en"; // Set voice by properties
	voice.languages              = langNativeString;
	voice.name                   = "UK";
	voice.variant                = 4;
	voice.gender                 = 1;
	espeak_SetVoiceByProperties(&voice);

	/* SIMPLER
	char voicename[] = {"English"};
	espeak_SetVoiceByName(voicename);
	*/

	sleep(5); // Allow xserver to startup

	while(getenv("DISPLAY") != NULL){
		sleep(interval - ((long)current_time%interval));

		time(&current_time);
		timeinfo = localtime( &current_time);
		hour   = timeinfo->tm_hour;
		minute = timeinfo->tm_min;
		if (minute == 0){
			sprintf(text, "Its %d hours.", hour);
		}else if (minute == 15){
			sprintf(text, "Its quarter past %d hours.", hour);
		}else if (minute == 30){
			sprintf(text, "Its half past %d hours.", hour);
		}else if (minute == 45){
			sprintf(text, "Its quarter to %d hours.", (hour + 1) % 24);
		}else{
			//In case there is some unexpected delay
			strftime(text,sizeof(text),"Its %M minutes past %H hours.",timeinfo);
		}

		espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
	}
	return 0;
}
