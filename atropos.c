/* Mode: C;
* Atropos frederic Bourgeois http://numsys.eu 
*/


#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/dir.h>
/* local include */
#include "vrrpd.h"

#ifdef use_linux_libc5
#endif

typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#if defined(SIOCGPARAMS)  && SIOCGPARAMS != SIOCDEVPRIVATE+3
#error Changed definition for SIOCGPARAMS
#else
#define SIOCGPARAMS (SIOCDEVPRIVATE+3) 		/* Read operational parameters. */
#define SIOCSPARAMS (SIOCDEVPRIVATE+4) 		/* Set operational parameters. */
#endif
#define DATA_MAX 10000

char version1[10] = "0.80";
char buff[80];
char data[DATA_MAX];
char pidend[6] = ".pid";
char temp2[2] = "/";
int monitor;
int pid;
int ix = 0;
int max_mac = 6;
// Five loop with twice SIGNAL
int max_count = 5;
int nb;
int retval;
char globalstatedown[FILENAME_MAX+1];
char statedown[FILENAME_MAX+1];
char statetemp[18] = "/vrrpdstatedown_";
char statetemp2[FILENAME_MAX+1];
static char	PidDir[FILENAME_MAX+1];
char namepid[FILENAME_MAX+1]="vrrpd_";
char temp[FILENAME_MAX+1];

DIR *currentDir;
struct dirent *fichier;

const char shortopts[] = "aA:bB:fF:Mn:sS:hH:p:rRsvVw?";

struct option longopts[] = {
 /* { name  has_arg  *flag  val } */
	{"state", 0, 0, 's'},
	{"help", 0, 0, 'h'},
	{"backup", 0, 0, 'b'},
	{"reduce", 0, 0, 'r'},
	{"increase", 0, 0, 'i'},
	{"master", 0, 0, 'm'},
	{"version", 0, 0, 'v'},
	{"error", 0, 0, '?'},
	{ 0, 0, 0, 0 }
	};

char ligne[80];
int cpt = 0;
char buf[80];
int s, i;
FILE * filestate;
FILE *f;

unsigned int opt_a = 0,					/* Show-all-interfaces flag. */			/* Verbose flag. */
	state = 0,
	version = 0,
	backup = 0,
	master = 0,
	increase = 0,
	reduce = 0;

void fonctinfo() {
		FILE * child_process = popen("ps -e |grep vrrpd | wc -l", "r");
		fgets(buf, sizeof(buf), child_process);
		pclose(child_process);
            	printf("############################################################\n\n");
		if (child_process == NULL) 
			fprintf(stdout,"Could not open pipe:\n");	
		if ( !strncmp(buf, "0", 1 )) {
			fprintf(stdout, "\n");	
			fprintf(stdout,"VRRPD RUN: 0\n");
			exit(0);	
		}
	  	snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );

                if( NULL == ( currentDir = opendir(PidDir))) {
                	perror( "opendir()" );
                } else {
                	while( NULL != ( fichier = readdir( currentDir ))) {
                        	if(!strncmp(namepid, fichier->d_name, 6) ){
                                	strcpy(statetemp2,PidDir);
                                        strcat(statetemp2,temp2);
                                        strcat(statetemp2,fichier->d_name);
                                        	if ((f = fopen(statetemp2, "rb")) != NULL){
                                                	fgets(buff, sizeof(buff), f);
                                                        fclose(f);
                                                        int pid = atoi(buff);
                                                        kill(pid,SIGUSR1);
							char vrrp_tmp[FILENAME_MAX]="/tmp/";
							sleep(1);
						        snprintf(temp, sizeof(temp), "vrrpstate%d", pid);
        						strcat(vrrp_tmp, temp);
        						if ((f = fopen(vrrp_tmp, "r")) != NULL){
       								while (fgets(data, DATA_MAX, f) != NULL) 
        							{
            								printf("%s", data);
        							}
							 } else {
                               						fprintf(stdout, "VRRP PID %d File %s STATE NOT FOUND\n", pid, vrrp_tmp);
								}
						 }
                                                        statetemp2[0]='\0';
							printf("\n");
            						printf("############################################################\n\n");
                                                 }
                       }
		}

               // End Loop // 
		FILE *filestate = popen("ps -fC vrrpd", "r");
		if (filestate != NULL){
			while (fgets (ligne, sizeof ligne, filestate) != NULL){
	        		cpt++;
	       			printf ("%s",ligne);
			}
			fclose (filestate);
			}
		else{
			printf ("Erreur d'ouverture du fichier\n");
 			}


	for (ix=0; ix < max_mac; ix++) {

		struct  ifreq  devea;
		int ether = ix;
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0)
		{
			perror("socket");
			exit(1);
		}
		
		sprintf(devea.ifr_name, "eth%d", ether);
		if (ioctl(s, SIOCGIFHWADDR, &devea) == 0)
		{
			printf("\n");
			printf("eth%d Current MAC is: ",ether);
			for (i = 0; i < max_mac; i++)
			{
				if (i < (max_mac - 1))
					printf("%2.2x:", i[devea.ifr_hwaddr.sa_data] & 0xff);
				if (i == (max_mac - 1))
					printf("%2.2x", i[devea.ifr_hwaddr.sa_data] & 0xff);
			}
			printf("\n");
		}		
	}
	printf("\n");
	fprintf(stdout, "Be careful, Atropos doesn't show virtual mac address of vlan interface");
	printf("\n");
	fprintf(stdout, "Take a look at syslog for more informations");
	printf("\n");
 }

static void signal_end( int nosig )
{
	fprintf(stdout,"\n");
	fprintf(stdout,"Don't use \"CTRL+C\"\n");
	signal( SIGINT, signal_end );
	return;
}


int main(int argc, char **argv)
{
	signal( SIGINT, signal_end );
	int c, errflag = 0;
	while ((c = getopt_long(argc, argv, shortopts, longopts, 0)) != EOF)
			switch (c) {
			case 's': state++;  break;
			case 'v': version++;  break;
			case 'b': backup++;  break;
			case 'r': reduce++;  break;
			case 'i': increase++;  break;
			case 'm': master++;  break;
			case '?': errflag++; break;
			}

	if (state) {
		fonctinfo();
		return 0;	
		};

	if (version) {
		fprintf(stdout,"Atropos %s\n", version1);
		return 0;	
		};


        if (reduce) {
                snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );
                fprintf(stdout,"Please wait\n");

		sleep (1);
                if( NULL == ( currentDir = opendir(PidDir))) {
                	perror( "opendir()" );
                        } else {
                        	while( NULL != ( fichier = readdir( currentDir ))) {
                        		if(!strncmp(namepid, fichier->d_name, 6) ){
                                        	strcpy(statetemp2,PidDir);
                                                strcat(statetemp2,temp2);
                                                strcat(statetemp2,fichier->d_name);
                                                if ((f = fopen(statetemp2, "rb")) != NULL){
                                                       	fgets(buff, sizeof(buff), f);
                                                       	fclose(f);
                                                       	int pid = atoi(buff);
                                                        kill(pid,SIGTTIN);
                                                	statetemp2[0]='\0';
                                                }
                                        }
                       }
		}

		fprintf(stdout,"DONE\n");	
                return 0;
        };

        if (increase) {
                snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );
                fprintf(stdout,"Please wait\n");

                sleep (1);
                if( NULL == ( currentDir = opendir(PidDir))) {
                        perror( "opendir()" );
                        } else {
                                while( NULL != ( fichier = readdir( currentDir ))) {
                                        if(!strncmp(namepid, fichier->d_name, 6) ){
                                                strcpy(statetemp2,PidDir);
                                                strcat(statetemp2,temp2);
                                                strcat(statetemp2,fichier->d_name);
                                                if ((f = fopen(statetemp2, "rb")) != NULL){
                                                        fgets(buff, sizeof(buff), f);
                                                        fclose(f);
                                                        int pid = atoi(buff);
                                                        kill(pid,SIGTTOU);
                                                        statetemp2[0]='\0';
                                                }
                                        }
                       }
                }

                fprintf(stdout,"DONE\n");
                return 0;
        };


	if (backup) {
		snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );
		fprintf(stdout,"VRRPD BACKUP\n");
		fprintf(stdout,"Please wait\n");
		fprintf(stdout,"Don't use \"CTRL+C\"\n");
		for (ix = 0; ix <= max_count; ix++) {
				fprintf(stderr, "\n");
				fprintf(stderr,"Stage: %d|%d:  ", ix, max_count);
				sleep (1);
				fprintf(stderr,"\b");
				fprintf(stderr,"|");
                       		if( NULL == ( currentDir = opendir(PidDir))) {
                                	perror( "opendir()" );
                                	} else {
                                	while( NULL != ( fichier = readdir( currentDir ))) {
                                        	if(!strncmp(namepid, fichier->d_name, 6) ){
                                                	strcpy(statetemp2,PidDir);
                                                	strcat(statetemp2,temp2);
                                                	strcat(statetemp2,fichier->d_name);
                                                	if ((f = fopen(statetemp2, "rb")) != NULL){
                                                        	fgets(buff, sizeof(buff), f);
                                                        	fclose(f);
                                                        	int pid = atoi(buff);
                                                               	kill(pid,SIGUSR2);
                                                		statetemp2[0]='\0';
                                                	}	
                                        	}
                                	}
				}
				sleep(1);
                                fprintf(stderr,"\b");
				fprintf(stderr,"/");
				sleep (1);
                                fprintf(stderr,"\b");
				fprintf(stderr,"-");
				sleep (1);
				fprintf(stderr,"\b");
				fprintf(stderr,"DONE");
                                if( NULL == ( currentDir = opendir(PidDir))) {
                                        perror( "opendir()" );
                                        } else {
                                        while( NULL != ( fichier = readdir( currentDir ))) {
                                                if(!strncmp(namepid, fichier->d_name, 6) ){
                                                        strcpy(statetemp2,PidDir);
                                                        strcat(statetemp2,temp2);
                                                        strcat(statetemp2,fichier->d_name);
                                                        if ((f = fopen(statetemp2, "rb")) != NULL){
                                                                fgets(buff, sizeof(buff), f);
                                                                fclose(f);
                                                                int pid = atoi(buff);
                                                                kill(pid,SIGUSR2);
                                                        	statetemp2[0]='\0';
                                                        }
                                                }
                                        }
                                }

		}

		fprintf(stdout,"\n");	
		fprintf(stdout,"OK \n");
		fonctinfo();	
		return 0;
	}

	else {
		fprintf(stdout,"\n");
		fprintf(stdout,"Atropos %s frederic Bourgeois http://numsys.eu\n", version1);
		fprintf(stdout,"\n");
		fprintf(stdout,"atropos --backup 		Be backup (caution: Don't use with priority !)\n");
		fprintf(stdout,"atropos --reduce 		Reduce priority dynamically priority -10\n");
		fprintf(stdout,"				If vrrpd run with -z : Set the priority after SIGTTIN (not decrement as default)\n");
		fprintf(stdout,"atropos --increase 		Increase priority dynamically +10 \n");
		fprintf(stdout,"				If vrrpd run with -x : Set the priority after SIGTTOU (not increment as default)\n");
		fprintf(stdout,"atropos --help			This Page\n");
		fprintf(stdout,"atropos --state			Status \n");
		fprintf(stdout,"atropos --version		version \n");
		fprintf(stdout,"It requires to be run as root \n");	
		return 0;
		}

}
