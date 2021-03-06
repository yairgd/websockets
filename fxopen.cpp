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
#include <iostream>
#include <chrono>
#include <uuid/uuid.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <json/json.h>



//npm install wscat
//wscat -c  wss://ws.bitstamp.net/


static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}



int main(int argc, const char **argv)
{
	WebSockets ws;
	std::string web_api_key = "<web_api_key>";
	std::string secret = "<secret>";
	std::string web_api_id = "<web_api_id>";

	auto base64_encode = [] (unsigned char const* bytes_to_encode, unsigned int in_len)->std::string  {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		while (in_len--) {
			char_array_3[i++] =  *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for(i = 0; (i <4) ; i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}
		if (i)
		{
			for(j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while((i++ < 3))
				ret += '=';

		}
		return ret;
	};

	auto createSign = [base64_encode](std::time_t timestamp, std::string web_api_id, std::string web_api_key, std::string api_secret)->std::string {
		//unsigned char* result;
		unsigned int len = 2000;

		std::string data_to_sign = "";
		data_to_sign.append(std::to_string(timestamp));
		data_to_sign.append(web_api_id);
		data_to_sign.append(web_api_key);
		const unsigned char *sign_data =  HMAC(EVP_sha256(),(unsigned char*)&api_secret[0], api_secret.size(),(unsigned char*)&data_to_sign[0] ,data_to_sign.size(), NULL, NULL);
		return 	base64_encode (sign_data,32);
	};
	auto func = [](std::string json, std::string protocl_name) ->bool {

		std::cout<<protocl_name<<":   "<<json<<std::endl;
		return true;
	};

	auto runAfterConnection = [&ws,&createSign,&web_api_key,&secret,&web_api_id](void) ->void {
		// find time stamp
		auto unix_timestamp = std::chrono::seconds(std::time(NULL));
		auto unix_timestamp_x_1000 = std::chrono::milliseconds(unix_timestamp).count();


		// create uuid
		uuid_t uuid;
		char id[64];
		memset (id,64,0);
		uuid_generate(uuid);
		uuid_unparse_lower(uuid, id);

		// sign the data
		std::string sig = createSign(unix_timestamp_x_1000, web_api_id,web_api_key,secret);

		// create json 
		Json::Value root;
		root["Id"] = id;
		root["Request"] = "Login";
		root["Params"]["AuthType"] = "HMAC";
		root["Params"]["WebApiId"] = web_api_id;
		root["Params"]["WebApiKey"] = web_api_key;
		root["Params"]["Timestamp"] = unix_timestamp_x_1000;
		root["Params"]["Signature"] = sig;
		root["Params"]["DeviceId"] = "trade_app";
		root["Params"]["AppSessionId"] = "123";

		// strinfy the json
		std::ostringstream message;
		Json::StreamWriterBuilder wbuilder;
		wbuilder["commentStyle"] = "None";
		wbuilder["indentation"] = ""; //The JSON document is written in a single line
		std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());
		writer->write(root,&message );

		// send login command
		ws["fxopen_feed"].Write(message.str());


		ws["fxopen_feed"].Write("{\"Id\": \"A03DF33D-E520-483B-892F-73C671127BC2\",\"Request\": \"FeedSubscribe\",\"Params\": {\"Subscribe\": [{\"Symbol\": \"EURUSD\",\"BookDepth\": 3}, {\"Symbol\": \"USDJPY\",\"BookDepth\": 1}]}}");
	};
	ws.AddProtocol("fxopen_feed","marginalttlivewebapi.fxopen.net","/",3000,func);	


	ws.Connect(runAfterConnection);		
	ws.Run();

}




