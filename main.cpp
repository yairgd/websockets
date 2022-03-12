/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2022 06:29:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yair Gadelov (yg), yair.gadelov@gmail.com
 *        Company:  Israel
 *
 * =====================================================================================
 */

//g++  WebSockets.cpp -o WebSockets  -lwebsockets -lpthread -DDEMO
#include "WebSockets.h"
#include <iostream>
#include <string>

// https://libwebsockets.org/git/libwebsockets/tree/minimal-examples/ws-client/minimal-ws-client-ping/minimal-ws-client-ping.c?id=7c6e3a8aeb89f989ff4ccb4605736c38220789c5  ping/[poing + timer example
int main(int argc, const char **argv)
{
	WebSockets ws;

	auto func = [](std::string json, std::string protol_name) ->bool {
		std::cout<<protol_name<<":          "<<json<<std::endl;
		return true;
	};

	auto runAfterConnection = [&ws](void) ->void {
		ws["binance"].Write("{\"method\": \"SUBSCRIBE\",\"params\":[\"btcusdt@trade\"],\"id\": 1}");
		ws["binance"].Write("{\"method\": \"SUBSCRIBE\",\"params\":[\"ltcusdt@trade\"],\"id\": 2}");
		ws["binance2"].Write("{\"method\": \"SUBSCRIBE\",\"params\":[\"xrpusdt@trade\"],\"id\": 3}");
		ws["bitstamp"].Write("{ \"event\": \"bts:subscribe\",\"data\": { \"channel\": \"diff_order_book_btcusd\"}}");
		ws["bitstamp2"].Write("{ \"event\": \"bts:subscribe\",\"data\": { \"channel\": \"diff_order_book_ltcusd\"}}");
	};

	ws.AddProtocol("bitstamp","ws.bitstamp.net","/",443,func);				
	ws.AddProtocol("binance","stream.binance.com","/ws",9443,func);	
	ws.AddProtocol("binance2","stream.binance.com","/ws",9443,func);	
	ws.AddProtocol("bitstamp2","ws.bitstamp.net","/",443,func);				

	ws.Connect(runAfterConnection);		
	ws.Run();

}
