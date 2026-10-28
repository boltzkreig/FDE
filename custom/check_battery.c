/*
 * BUILD: gcc check_battery_.c -o check_battery -Wall `pkg-config --cflags --libs libnotify` -O3 -DBEEP -lportaudio (Optional - Sawtooth Beep)
 * A Battery Checking Programme with notifications
   in case of Low, High and Full Battery charge
 * NOTE: Check that BAT_PATH & SUP_PATH are accurate for you computer.
 * If X-server is available then it uses `notify-send` else optionally `Wall` status.
 * It uses a bi-tonal Sawtooth beep.
 * Add any argument to start in verbose mode
 * Run as a deamon in '.xinitrc' or equivalent of you WM.
 * Written By: Boltzkreig
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libnotify/notify.h>

#define MIN_CHARGE 45
#define MAX_CHARGE 80
#define LIMIT 3
#define INTERVAL 30
#define BAT_PATH "/sys/class/power_supply/BAT0/capacity"
#define SUP_PATH "/sys/class/power_supply/AC/online"

#ifdef BEEP

#include "portaudio.h"
#define VOLUME        (0.5)
#define NUM_SECONDS   (0.41421356)
#define SAMPLE_RATE   (44100)

typedef struct
{
    float phase;
    int frame_count;
} paTestData;

static int patestCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData ) {
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer;
    (void) timeInfo;
    (void) statusFlags;

    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->phase;
        data->frame_count++;
        if (data->frame_count < SAMPLE_RATE * NUM_SECONDS * 0.333 ){
            data->phase += 0.002f;
        }else{
            data->phase += 0.02f;
        }
        if( data->phase >= 1.0f * VOLUME) data->phase = -1.0f * VOLUME;
    }
    return 0;
}

static paTestData data;
int play_audio(){
    PaStream *stream;
    PaError err;

    data.phase = 0.0;
    data.frame_count = 0;
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    err = Pa_OpenDefaultStream( &stream,
            0,          /* no input channels */
            1,          /* mono output */
            paFloat32,  /* 32 bit floating point output */
            SAMPLE_RATE,
            256,        /* frames per buffer */
            patestCallback,
            &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    /* Sleep for several seconds. */
    Pa_Sleep(NUM_SECONDS*1000);

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    Pa_Terminate();
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}

#endif

char wall[256];
char title[8] = "BATTERY";
NotifyNotification *notif;

int read_sysfs(char *file){
    FILE *fp;
    char buff[8];
    fp = fopen(file,"r");
    if(!fp) return -1;
    if(!fgets(buff,sizeof(buff),fp)) return -1;
    fclose(fp);
    return atoi(buff);
}

void showNotify(int charge, char *message, int *counter,int *notifyVisible){
    if(*counter){
        notify_notification_update(notif,title,message,NULL);
    }else {
        notif = notify_notification_new(title,message,NULL);
    }
    notify_notification_set_urgency(notif, NOTIFY_URGENCY_CRITICAL);
    notify_notification_show(notif, NULL);
    *notifyVisible = 1;
    (*counter)++;
#ifdef BEEP
    play_audio();
#endif
    if(getenv("DISPLAY") == NULL && getenv("WAYLAND_DISPLAY") == NULL){
        snprintf(wall, sizeof(wall),
                "echo \"%s(%d%%): \\\"%s\\\". Reminder Counter: %d\" | wall",
                title, charge, message, *counter);
        system(wall);
    }
}

void ignore(int *multiplier, int *notifyVisible){
    *multiplier = 10;
    if(*notifyVisible){
        notify_notification_close(notif,NULL);
        g_object_unref(notif);
        *notifyVisible = 0;
    }
}

int main(int argc,char **argv){
    (void) argv;     // escape unused variable errors
    int multiplier = 1;
    int notifyVisible = 0;
    int counter = 0;
    int charge = -1;
    int isPluggedIn = -1;
    char message[48];
    if (argc > 1 ){
        printf( "Visual Representation:\n"
                " 0                  (-)  +L                   -L  (+)  +L             95   100\n"
                " | - - - - - - - - - | - | - - - - - - - - - - | - | - | - - - - - - - | - |\n"
                " >->->->->->->->->->->->->->->->->->->->->->-> w w w w w >->->->->->-> w w w\n"
                " W W W W W W W W W W W W <-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<\n"
                " W = Inf Warnings: w = 3 Warnings: +L = Upper Limit: -L = Lower Limit \n");
    }

    if(!notify_init("dunst")) return 1;

    while(1){
        charge = read_sysfs(BAT_PATH);
        isPluggedIn = read_sysfs(SUP_PATH);

        // Full charge: triggering above 95%: shown 3 times
        if ( charge > 95 && isPluggedIn ){
            if (counter < 3){
                snprintf(message,sizeof(message),
                        "Fully Charged #%d",counter+1);
                showNotify(charge,message,&counter,&notifyVisible);
            } else {
                ignore(&multiplier, &notifyVisible);
            }
        }
        // Max charge: shown 3 times: assumes attempt to full charge after that
        else if ( counter < 3 && charge < MAX_CHARGE+LIMIT && charge > MAX_CHARGE-LIMIT && isPluggedIn ){
            snprintf(message,sizeof(message),
                    "Max Charge Reached: Unplug Charger #%d",counter+1);
            showNotify(charge,message,&counter,&notifyVisible);
        }
        // Min charge: shown until plugged-in
        else if ( charge < MIN_CHARGE+LIMIT && !isPluggedIn ){
            snprintf(message,sizeof(message),
                    "Min Charge Reached: Plug Charger #%d",counter+1);
            showNotify(charge,message,&counter,&notifyVisible);
        }
        else {
            counter = 0;
            ignore(&multiplier, &notifyVisible);
        }

        if(argc > 1) printf("The battery is at %d%% and %scharging.\n",charge,isPluggedIn? "":"dis");
        sleep(INTERVAL * multiplier);
        multiplier = 1;
    }
    return 0;
}
