/*
    SHT15 temp broadcaster
    author: yiling.cao@gmail.com,
    date:   2015-01-26
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define NETWORK_SEND 1
#define DEBUG 1
#define READ_UINTERVAL 5000000 //5 second
#define TEMP_FILE "/sys/devices/platform/sht15/temp1_input"
#define TEMP_ERR_FILE "/sys/devices/platform/sht15/temp1_fault"
#define HUMI_FILE "/sys/devices/platform/sht15/humidity1_input"
#define HUMI_ERR_FILE "/sys/devices/platform/sht15/humidity1_fault"


int init_spi(); 
static void pabort(const char *);
int read_spi();
int net_broadcast(char *);

/*global*/
int sock;                         /* Socket */
struct sockaddr_in broadcastAddr; /* Broadcast address */
char *broadcastIP="224.0.0.1";                /* IP broadcast address */
unsigned short broadcastPort=2222;     /* Server port */
int broadcastPermission;          /* Socket opt to set permission to broadcast */


int main(int argc, char **argv){
	printf("SHT15 boardcaster to %s:%d\n", broadcastIP, broadcastPort);        
	read_sht15();	
	exit(0);
}

static void pabort(const char* msg){
	perror(msg);
	exit(1);
}

void error_msg(char* str){
	printf("%s\n", str);
}

int net_broadcast(char* send_string){
	unsigned int send_string_len;       /* Length of string to broadcast */

	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		error_msg("socket() failed");
		return -1;
	}

	/* Set socket to allow broadcast */
	broadcastPermission = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0){
		error_msg("setsockopt() failed");
		return -2;
	}

	/* Construct local address structure */
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
	broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
	broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
	broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */

	send_string_len = strlen(send_string);  /* Find length of sendString */
	/* Broadcast send_string in datagram to clients every 3 seconds*/
	if (sendto(sock, send_string, send_string_len, 0, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) != send_string_len){
		error_msg("sendto() sent a different number of bytes than expected");
		return -3;
	}
	return 0;
}


/*read spi value and return 3 bytes.
  the function will always stay in loop unless any change of 3 bytes occurs.
*/
int read_sht15(){
	char buff[255];
	float temp=0;
	float humi=0;

	FILE * temp_file;// = fopen( TEMP_FILE, "r" );
	FILE * humi_file;// = fopen( HUMI_FILE, "r" );

	while(1){
		
		if(1){
			temp_file = fopen( TEMP_FILE, "r" );
			fscanf(temp_file, "%s", buff);
			temp = atoi(buff)/1000.0;
			fclose(temp_file);

			humi_file = fopen( HUMI_FILE, "r" );
			fscanf(humi_file, "%s", buff);
			humi = atoi(buff)/1000.0;
			fclose(humi_file);

			sprintf(buff, "TEMP:%.2f,HUMI:%.2f\n", temp, humi);
			printf(buff, "%s\n", buff);
		}else{
			sprintf(buff, "TEMP:ERR,HUMI:ERR");
		}

#if NETWORK_SEND
		net_broadcast(buff);
#endif
		usleep(READ_UINTERVAL);
    	}
	return 0;
}
