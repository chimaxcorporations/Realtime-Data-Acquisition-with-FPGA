#include <stdio.h>
#include <string.h>

#include "lwipopts.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "xgpio.h"
//#include "xil_printf.h"
#define pmodADC XPAR_PMODADC_0_S00_AXI_BASEADDR
#define	http_port  80

#define webHTTP_OK	"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"   	// Standard GET response
// Format of the dynamic page (Header)
#define webHTML_START "<html> <head> </head> <BODY onLoad=\"window.setTimeout(&quot;location.href='index.html'&quot;,1000)\" bgcolor=\"#FFFFFF\" text=\"#2477E6\">\r\n"

#define webHTML_END "\r\n</pre> \r\n</font></BODY> </html>"

#define RECV_BUF_SIZE 2048  				// http request size can be a max of RECV_BUF_SIZE
char recv_buf[RECV_BUF_SIZE];				// Http Request Receiver Buffer

#define webMAX_PAGE_SIZE	2048   			// Size of "dynamic WEB page"
char  cDynamicPage[webMAX_PAGE_SIZE];
XGpio LED, SW;
u16 *pmodAdd = (u16*) pmodADC;
u16 adcdata;
u16 readSW = 0x00;
float Temps[120];
//****************************************************************************************************
void ADC_to_Buffer  (float t)
{
               for (int i=1; i<120; i++)
                              Temps[i-1] = Temps[i];
               Temps[120-1] = t;
}

void AddLine_SVG (void)
{
char hstr[32];
               strcat (cDynamicPage, "<svg height=500 width=500>" );

               strcat (cDynamicPage, "<polyline points='");
               for (int i=0; i<120; i++)
               {
                    sprintf (hstr, "%d,%d ", i*4, 200- (int)(Temps[i]*4) );
                    strcat (cDynamicPage, hstr);
               }

    strcat (cDynamicPage, "' style=fill:none;stroke:black;stroke-width:2 />");
               strcat (cDynamicPage, "</svg>" );
}

void AddPageHitCounter (void)
{
static int ulPageHits = 0;
char cPageHits[32];
	ulPageHits++;
	sprintf( cPageHits, "Page Hits= %d \r\n", ulPageHits );
	strcat( cDynamicPage, cPageHits );				// Concatenate to the PageBuffer
	strcat ( cDynamicPage,"<br>");
}

void readSwitch(void)
{
	char swval[32];
	readSW = XGpio_DiscreteRead(&SW, 1);
	sprintf( swval, "Switch= %d \r\n", readSW );
	strcat( cDynamicPage, swval );
	strcat ( cDynamicPage,"<br>");

}
void readReg(void )
{
	char val[32];
	float adcBuf;
		adcdata  = *(pmodAdd)&0xFFF;
		XGpio_DiscreteWrite(&LED, 1, adcdata);
		sprintf( val, "ADC VALUE= %d \r\n", adcdata );
		strcat( cDynamicPage, val );
		strcat ( cDynamicPage,"<br>");
		adcBuf = (float)adcdata;
		ADC_to_Buffer (adcBuf);
	    xil_printf("ADC Value %i \r\n",adcdata);
//	vTaskDelete(NULL);
}
void AddTaskList (void)
{
	strcat ( cDynamicPage, "<p><pre>Task          State  Priority  Stack    #<br>");
	strcat ( cDynamicPage, "*************************************************<br>" );
	vTaskList( (char* ) cDynamicPage + strlen( cDynamicPage ) );  // Then the list of tasks and their status...
//	vTaskGetRunTimeStats( ( signed portCHAR * ) cDynamicPage + strlen( cDynamicPage ) );
}

static void prvweb_ParseHTMLRequest (int sd, char* RecString, int RecLen)
{

		// Is this a GET?  We don't handle anything else
		if( (RecLen > 1 ) && ( !strncmp( RecString, "GET", 3) ) )
		{
			write (sd, webHTTP_OK, (u16_t) strlen(webHTTP_OK) );  	// Write out the HTTP OK header

			strcpy( cDynamicPage, webHTML_START );  				// Generate the dynamic page... First the page header
			AddPageHitCounter ();
			readReg(); // add adc value
			readSwitch(); // read switch status
			AddLine_SVG ();
//			AddTaskList ();

			strcat( cDynamicPage, webHTML_END );  						//  Finally the page footer
			write (sd, cDynamicPage, (u16_t)strlen(cDynamicPage) );   	// Write out the dynamically generated page
		}
}


void process_http_request(void *p)
{
int sd = (int)p;
int read_len;

	read_len = read(sd, recv_buf, RECV_BUF_SIZE);  // read in the request

	if (read_len > 0)
		prvweb_ParseHTMLRequest (sd, recv_buf, read_len);

	close(sd);
	vTaskDelete(NULL);
	return;
}




// http server
void Task_WEBServer (void *p)
{
int sock, new_sd;
struct sockaddr_in address, remote;
int size;

	// create a TCP socket
	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		vTaskDelete(NULL);
		return;
	}

	// bind to port 80 for any incoming addr
	address.sin_family = AF_INET;
	address.sin_port = htons(http_port);
	address.sin_addr.s_addr = INADDR_ANY;
	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
	{
		vTaskDelete(NULL);
		return;
	}

	lwip_listen(sock, 0);    // Set to listen for incoming connections

	size = sizeof(remote);
	while (1)
	{
		new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size);
		// spawn a separate handler for each request
		sys_thread_new ("http", process_http_request, (void*)new_sd, 2048, DEFAULT_THREAD_PRIO);
	}
	return;
}
