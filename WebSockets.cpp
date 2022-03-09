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
		case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
			printf ("LWS_CALLBACK_CLIENT_RECEIVE_PONG");
			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE: 
			{
			//	WebSockets * WebSockets  =  (WebSockets*)lws_get_protocol(wsi)->user;
			//	std::string msg = ws->getMsg();
			//	char c[255];
			//	c[0] = 0;
			//	strncpy (c, msg.c_str(),msg.size());
			//	c[msg.size()] = 0;
			//	n = lws_write(wsi, (unsigned char*) c, strlen(c)  ,  LWS_WRITE_TEXT);
			
				break;
			}

		case LWS_CALLBACK_CLIENT_RECEIVE:
			time(&p->last_update_time);

			if (lws_is_first_fragment(wsi) ) {
				p->ClearMsg();

			}

			p->MsgAppend((char*)in,len);
			if (lws_is_final_fragment(wsi)) {	
				bool res = p->CallBack(); 
				if (res == false) {
					p->ClearWsi() ;
					return -1;
				}
			};
			break;

		case LWS_CALLBACK_CLIENT_CLOSED:
			lwsl_user("%s: close connection\n", __func__);
			p->ClearWsi();

			break;

		default:
			break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}






