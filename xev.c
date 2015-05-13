/*
 * Author: IteratedLateralus ( http://github.com/iteratedlateralus )
 */

#include "sdl/playwave.h"
#include "sendmail.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <curl/curl.h>
#define SMTP_TEMPLATE_FILE "template.txt"

extern char* optarg;
Display* dpy;
Window w;
char trigger_type[10];    
char* device = NULL;
time_t grace_time;
time_t trigger_grace_time = 0;
int sound_off_count = 1;
char http_get_url[256];
int sound_off_threshhold = 20;
int sound_off_counter = 0;
int max_children = 10;
int child_count = 0;
int http_counter = 0;
int http_threshhold = 5;
int first_hit = 0; 
static char* smtp_host = NULL;
static char* smtp_user = NULL;
static char* smtp_to = NULL;
static char* smtp_from = NULL;
static char* smtp_password = NULL;
static char* smtp_message = NULL;
static uint16_t smtp_port = 25;

void usage(){
    printf("Usage:\n");
    printf("======\n");
    printf(" ./tripwire -t <type> [options]\n");
    printf("OPTIONS\n");
    printf("=======\n");
    printf(" -h             Print this help screen\n");
    printf(" -t <type>      Trigger type. One of: http,sms,sendmail,none\n");
    printf(" -d             Read from stdin\n");
    printf(" -D <file/dev>  Read from file or dev device\n");
    printf(" HTTP type\n");
    printf(" -u <url>       URL to hit\n");
    printf(" SMTP type\n");
    printf(" -H <host>      SMTP host\n");
    printf(" -F <from>      SMTP from\n");
    printf(" -P <password>  SMTP password\n");
    printf(" -p <port>      SMTP port (default: 25)\n");
    printf(" -o <to>        SMTP destination\n");
    printf(" \n");
    printf("OTHER OPTIONS\n");
    printf(" -x <width>     Width of the capture window\n");
    printf(" -y <height>    Height of the capture window\n");
    printf(" -s <count>     How many times to trigger alarm.wav\n");
    printf(" -T <thresh>    Soundoff threshhold\n");
    printf("EXAMPLES\n");
    printf("========\n");
    printf(" Using a window as input, calling a URL as the trigger type: \n");
    printf(" $ ./xev -t http -u http://example.com/script.php\n");
    printf("\n");
    printf(" Using /dev/input mice as input: \n");
    printf(" $ sudo cat /dev/input/mice | ./xev -d -t http -u http://example.com/script.php\n");
    printf("\n");
    printf(" Reading directly from /dev/input/mice (may require sudo)\n");
    printf(" $ sudo ./xev -d -D /dev/input/mice -t http -u http://example.com/script.php\n");
    printf("\n");
}

int grace_time_reached(){
    return time(NULL) >= grace_time;
}
int trigger_grace_time_reached(){
    return time(NULL) >= trigger_grace_time;
}
void trigger_sendmail(){
    if( smtp_message ){
        free(smtp_message);
        smtp_message = NULL;
    }
    smtp_message = (char*)malloc(1000);
    if( build_smtp_message(SMTP_TEMPLATE_FILE,smtp_message,1000) < 0 ){
        strcpy(smtp_message,"Tripwire Alert");
    }
    smtp_send_message(smtp_host,smtp_port,smtp_user,smtp_password,smtp_from,smtp_message,strlen(smtp_message),&smtp_to);
}

void trigger_http_get(){
    CURL* ch = curl_easy_init();
    if( ch == NULL ){
        fprintf(stderr,"Invalid curl handle returned\n");
        return;
    }
    curl_easy_setopt(ch,CURLOPT_URL,http_get_url);
    curl_easy_perform(ch);
    curl_easy_cleanup(ch);
}

void motionNotify (){

    if( sound_off_counter++ >= sound_off_threshhold ){
        if( strcmp(trigger_type,"sendmail") == 0 ){
            trigger_sendmail();
        }else if( strcmp(trigger_type,"sms") == 0 ){
            printf("SMS stub\n");
        }else if( strcmp(trigger_type,"http") == 0 ){
            if( http_counter++ >= http_threshhold || first_hit == 0 ){
                printf("HTTP stub\n");
                printf("Hitting URL: %s\n",http_get_url);
                trigger_http_get();
                http_counter = 0;
                first_hit++;
            }
        }else if( strcmp(trigger_type,"none") == 0 ){
            printf(".");
        }
        play_wave((const char*)"alarm.wav");
        trigger_grace_time = time(NULL) + 20;
        sound_off_counter = 0;

    }
}


int main(int argc,char** argv){
    XEvent e;
    int s,x,y,misc;
    int stdin_read = 0;
    char c;
    FILE* fp = NULL;
    grace_time = time(NULL) + 10;
    
    while((c=getopt(argc,argv,"t:do:D:H:F:P:i:p:hg:x:y:u:T:m:")) != -1){
        switch(c){
            case 't':
                strncpy(trigger_type,optarg,10);
                break;
            case 'd':
                stdin_read = 1;
                break;
            case 'D':
                device = (char*)malloc(strlen(optarg));
                strncpy(device,optarg,strlen(optarg));
                break;
            case 'x':
                x = atoi(optarg);
                break;
            case 'y':
                y = atoi(optarg);
                break;
            case 's':
                sound_off_count = atoi(optarg);
                break;
            case 'u':
                strncpy(http_get_url,optarg,256);
                break;
            case 'T':
                sound_off_threshhold = atoi(optarg);
                break;
            case 'm':
                max_children = atoi(optarg);
                break;
            case 'H':
                smtp_host = (char*)malloc(strlen(optarg));
                strncpy(smtp_host,optarg,strlen(optarg));
                break;
            case 'F':
                smtp_from = (char*)malloc(strlen(optarg));
                strncpy(smtp_from,optarg,strlen(optarg));   
                break;
            case 'P':
                smtp_password = (char*)malloc(strlen(optarg));
                strncpy(smtp_password,optarg,strlen(optarg));
                break;
            case 'p':
                smtp_port = atoi(optarg);
                break;
            case 'o':
                smtp_to = (char*)malloc(strlen(optarg));
                strncpy(smtp_to,optarg,strlen(optarg));
                break;
            case 'h':
                usage();
                return 0;
            case '?':
                fprintf(stderr,"Unknown option encountered\n");
                return -1;
        }
    }

    if( stdin_read ){
        if( device ){
            fp = fopen(device,"rb");
            if( !fp ){
                fprintf(stderr,"Unable to open device: %s\n",device);
                free(device);
                return -1;
            }else{
                while(1){
                    if( fread(&c,sizeof(char),1,fp) ){
                        motionNotify();
                    }
                } 
            }
        }else{
                while(1){
                    misc = read(0,&c,sizeof(char));
                    if( misc ){
                        motionNotify();
                    }
                }
        }
    }else{
            dpy = XOpenDisplay(NULL);
            if( dpy == NULL ){
                fprintf(stderr,"Invalid display\n");
                return 0;
            }


            s = DefaultScreen(dpy);
            w = XCreateSimpleWindow(dpy, RootWindow(dpy,s),
                10,10,x,y,1,
                BlackPixel(dpy,s),
                WhitePixel(dpy,s)
            );
            
            XSelectInput( dpy, w, ExposureMask | KeyPressMask | PointerMotionMask );
            XMapWindow( dpy, w );
            while(1){
                XNextEvent(dpy,&e);
                if( e.type == MotionNotify ){
                    motionNotify();
                }
            }
    }
    if( device )
        free(device);
    if( smtp_host)
        free(smtp_host);
    if( smtp_user )
        free(smtp_user);
    if( smtp_password )
        free(smtp_password);
    return 0;
}
