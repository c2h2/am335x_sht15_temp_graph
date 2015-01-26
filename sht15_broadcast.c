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
#include <sys/types.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <net/if.h>

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
static void lplib_command_first_ethernet_card_data_get(char **ip, char **mac);


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


static void lplib_command_first_ethernet_card_data_get(char **ip, char **mac) {
    struct ifaddrs *ifap, *ifa;
    const char *name = NULL;
    struct sockaddr_ll *s;
    struct sockaddr_in *sa;
    unsigned int i, len;
    char macaddr[32] = {0};
    char ipaddr[20] = {0};
    int gotip = 0, gotmac = 0;
    getifaddrs(&ifap);
    for(ifa=ifap;ifa!=NULL;ifa=ifa->ifa_next)
    {
        if(ifa->ifa_addr!=NULL && (ifa->ifa_flags & IFF_UP))
        {
            if(name!=NULL && strcmp(ifa->ifa_name, name)!=0)
            {
                continue;
            }
            if(strncmp(ifa->ifa_name, "eth", 3)!=0 &&
                strncmp(ifa->ifa_name, "p", 1)!=0)
            {
                continue;
            }
            if(name==NULL)
            {
                name = ifa->ifa_name;
            }
            if(ifa->ifa_addr->sa_family==AF_PACKET)
            {
                s = (struct sockaddr_ll*)ifa->ifa_addr;
                len = 0;
                for(i=0;i<6;i++)
                {
                    len+=snprintf(macaddr+len, 31-len, "%02X", s->sll_addr[i]);
                }
                gotmac = 1;
                if(mac!=NULL && *mac==NULL)
                {
                    *mac = strdup(macaddr);
                }
            }
            else if(ifa->ifa_addr->sa_family==AF_INET)
            {
                sa = (struct sockaddr_in *)ifa->ifa_addr;
                memset(ipaddr, 0, 20);
                inet_ntop(AF_INET, &sa->sin_addr, ipaddr, 19);
                gotip = 1;
                if(ip!=NULL && *ip==NULL)
                {
                    *ip = strdup(ipaddr);
                }
            }
        }
        if(gotip && gotmac)
        {
            break;
        }
    }
    freeifaddrs(ifap);
    if(ip!=NULL && *ip==NULL)
    {
        *ip = strdup("0.0.0.0");
    }
    if(mac!=NULL && *mac==NULL)
    {
        *mac = strdup("000000000000");
    }
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
	FILE * temp_err_file;
	int err=0;
	//get mac address
        char *mac = NULL;
        char *ip = NULL;
        lplib_command_first_ethernet_card_data_get(&ip, &mac);
	


	while(1){
		temp_err_file = fopen( TEMP_ERR_FILE, "r");
		fscanf(temp_err_file, "%s", buff);
		fclose(temp_err_file);

		if (buff[0]=='0'){
			temp_file = fopen( TEMP_FILE, "r" );
			fscanf(temp_file, "%s", buff);
			temp = atoi(buff)/1000.0;
			fclose(temp_file);

			humi_file = fopen( HUMI_FILE, "r" );
			fscanf(humi_file, "%s", buff);
			humi = atoi(buff)/1000.0;
			fclose(humi_file);

			sprintf(buff, "TEMP:%.2f,HUMI:%.2f,MAC:%s,IP:%s\n", temp, humi, mac, ip);
		}else{
			sprintf(buff, "TEMP:ERR,HUMI:ERR,MAC:%s,IP:%s\n", mac, ip);
		}

		printf(buff, "%s\n", buff);

#if NETWORK_SEND
		net_broadcast(buff);
#endif
		usleep(READ_UINTERVAL);
    	}
	return 0;
}
