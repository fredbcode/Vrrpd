/* Mode: C;
* Atropos frederic Bourgeois http://numsys.eu 
*/


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
#ifdef use_linux_libc5
#endif

/* local include */
#include "vrrpd.h"


typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#if defined(SIOCGPARAMS)  && SIOCGPARAMS != SIOCDEVPRIVATE+3
#error Changed definition for SIOCGPARAMS
#else
#define SIOCGPARAMS (SIOCDEVPRIVATE+3) 		/* Read operational parameters. */
#define SIOCSPARAMS (SIOCDEVPRIVATE+4) 		/* Set operational parameters. */
#endif
char version1[10] = "0.60";
char buff[80];
int ix =9;
char pidend[6] = ".pid";
char temp2[2] = "/";
int monitor;
int pid;
int ix2;
int ix4 = 6;
int ix3;
int max = 8;
int nb;
int retval;
char globalstatedown[FILENAME_MAX+1];
char statedown[FILENAME_MAX+1];
char statetemp[18] = "/vrrpdstatedown_";
char statetemp2[FILENAME_MAX+1];
static char	PidDir[FILENAME_MAX+1];
char namepid[FILENAME_MAX+1]="vrrpd_";

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
		if (child_process == NULL) {
			fprintf(stdout,"Could not open pipe:\n");	
		}
		if ( !strncmp(buf, "0", 1 )) {
			fprintf(stdout, "\n");	
			fprintf(stdout,"VRRPD RUN: 0\n");
			exit(0);	
		}
 		fprintf(stdout, "\n");	

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
							sleep(1);
							if ((f = fopen("/tmp/.vrrpstate", "r")) != NULL) {
                						fgets(buff, sizeof(buff), f);
                        					int state = atoi(buff);
                        					fclose(f);
								if (state != 3){
                               						fprintf(stdout, "VRRP PID %d STATE: STATE BACKUP\n", pid);
								} else {
 									fprintf(stdout, "VRRPD STATE %d: STATE MASTER\n", pid);
									} 
							 } else {
                               						fprintf(stdout, "VRRP PID %d File /tmp/.vrrpstate STATE NOT FOUND\n", pid);
								}
						 }
                                                        statetemp2[0]='\0';
                                                 }
                       }
		}
                
		fprintf(stdout, "\n");
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


	for (ix3=0; ix3 <= ix4; ix3++) {

		struct  ifreq  devea;
		int ether = ix3;
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
			for (i = 0; i < 6; i++)
			{
				printf("%2.2x ", i[devea.ifr_hwaddr.sa_data] & 0xff);
				
			}
		}		
	}
	printf("\n");
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


main(int argc, char **argv)
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
		return;	
		};

	if (version) {
		fprintf(stdout,"Atropos %s\n", version1);
		return;	
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
                return;
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
                return;
        };


	if (backup) {
		snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );
		fprintf(stdout,"VRRPD BACKUP\n");
		fprintf(stdout,"Please wait\n");
		fprintf(stdout,"Don't use \"CTRL+C\"\n");
		for (ix3=0; ix3 <= ix4; ix3++) {
				fprintf(stderr,"\\");
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
		fprintf(stdout,"DONE\n");	
		return;
	}

	else {
		fprintf(stdout,"\n");
		fprintf(stdout,"Atropos %s frederic Bourgeois http://numsys.eu\n", version1);
		fprintf(stdout,"\n");
		fprintf(stdout,"atropos --backup 		Be backup (caution: Don't use with priority !)\n");
		fprintf(stdout,"atropos --reduce 		Reduce priority dynamically priority -10 \n");
		fprintf(stdout,"atropos --increase 		Increase priority dynamically +10 \n");
		fprintf(stdout,"atropos --help			This Page\n");
		fprintf(stdout,"atropos --state			Status \n");
		fprintf(stdout,"atropos --version		version \n");
		fprintf(stdout,"It requires to be run as root \n");	
		}

}
