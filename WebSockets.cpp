/*
 * =====================================================================================
 *
 *       Filename:  WebSockets.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/2022 09:57:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yair Gadelov (yg), yair.gadelov@gmail.com
 *        Company:  Israel
 *
 * =====================================================================================
 */
#include "WebSockets.h"
#include <string>
#include <string.h>
//https://github.com/warmcat/libwebsockets/blob/main/minimal-examples-lowlevel/ws-client/minimal-ws-client-rx/libwebsockets.org.cer
//static int interrupted, rx_seen, test;
//static struct lws *client_wsi;
//static  	unsigned char cs[256];




//	static int
int callback_dumb_increment(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	int n;
	WebSockets::Protocol * p  = 0;
	if ((WebSockets::Protocol*)lws_get_protocol(wsi))
		p  =  (WebSockets::Protocol*)lws_get_protocol(wsi)->user;
	switch (reason) {

		/* because we are protocols[0] ... */
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
				 in ? (char *)in : "(null)");
			p  =  (WebSockets::Protocol*)lws_get_protocol(wsi)->user;
		//	p->ClearWsi();
			break;

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lwsl_user("%s: established\n", __func__);
			p  =  (WebSockets::Protocol*)lws_get_protocol(wsi)->user;
			
		//	lws_callback_on_writable(wsi);
			p->setConnected(true);

			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE: 
			{
				//	WebSockets * WebSockets  =  (WebSockets*)lws_get_protocol(wsi)->user;

			//	std::string msg = ws->getMsg();
				char c[255];
				c[0] = 0;
			//	strncpy (c, msg.c_str(),msg.size());
			//	c[msg.size()] = 0;
				n = lws_write(wsi, (unsigned char*) c, strlen(c)  ,  LWS_WRITE_TEXT);
				// it is forbidden to uses directly with the pointer "(unsigned char*)userInfo->connection_string"
				// from some reson the following function couses memory leak and destroy the connection string 
				// I don't now why, maybe because the lws runs in c namespace and userInfo in c++ namespace 
				//		n = lws_write(wsi, (const char*)cs ,strlen( cs )  ,  LWS_WRITE_TEXT);
				//	n = lws_write(wsi, (unsigned char*) cs , strlen ( (const char*)cs)   ,  LWS_WRITE_TEXT);

				break;
			}

		case LWS_CALLBACK_CLIENT_RECEIVE:
			time(&p->last_update_time);
		//	printf("%s\n",in);
			lwsl_user("RX: %s\n", (const char *)in);
		//	rx_seen++;
		//	if (test && rx_seen == 10)
		//		interrupted = 1;
			break;

		case LWS_CALLBACK_CLIENT_CLOSED:
		//	p->ClearWsi();

			break;

		default:
			break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}






