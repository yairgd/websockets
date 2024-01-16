/*
 * =====================================================================================
 *
 *       Filename:  WebSockets.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/2022 09:57:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yair Gadelov (yg), yair.gadelov@gmail.com
 *        Company:  Israel
 *
 * =====================================================================================
 */

#ifndef WEB_SOCKETS
#define WEB_SOCKETS 
#include <functional>
#include <libwebsockets.h>
#include <string>
#include <signal.h>
#include <thread>
#include <future>
#include <time.h>
#include <vector>
#include <map>
#include <uuid/uuid.h>

#include <iostream>
typedef int  (*callback)(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int callback_dumb_increment(struct lws *wsi, enum lws_callback_reasons reason,
			    void *user, void *in, size_t len);
class WebSockets {

	public:
		class Protocol {
			public:
				Protocol() = default;
				Protocol(std::string name, lws_protocols *protocol, Protocol *user): m_name(name){
					*protocol = {strdup(&m_name[0]), callback_dumb_increment,0, 0, 0, user, 0}; 
					m_protocol = protocol;
					m_connected = false;
					last_update_time = 0;
					memcpy (p_name,&m_name[0],m_name.size());
					p_name[m_name.size()] = 0;
				}
				void ClearWsi() {
					client_wsi = 0;;
				}
				struct lws *getWsi() {
					return client_wsi;
				}
				~Protocol() {

				}
				void SetAddress(std::string address,std::string path, int port,
						std::function<bool (std::string json, std::string name)>   callback
					       ) {
					SetAddress(address, path, port, true, callback);
				}
				void SetAddress(std::string address,std::string path, int port, int ssl_connection,
						std::function<bool (std::string json, std::string name)>   callback
					       ) {
					m_address = address;
					m_path =path;
					memset(&i, 0, sizeof i); /* otherwise uninitialized garbage */
					i.port = port;
					i.address = strdup(&m_address[0]);
					i.path = strdup(&m_path[0]);
					i.host = i.address;
					i.origin = i.address;
					i.ssl_connection = ssl_connection;
					i.protocol = p_name; 
					i.pwsi = &client_wsi;
					m_callback = callback;
				}
				void Connect(struct lws_context *context) {
					i.context = context;					
					client_wsi = lws_client_connect_via_info(&i);
				}	
				int Write(const std::string & msg) {
					char c[8092];
					strncpy (c, msg.c_str(),msg.size());
					c[msg.size()] =0;
					return lws_write(client_wsi, (unsigned char*) c, strlen(c), LWS_WRITE_TEXT);
				}
				int Write(unsigned char* msg, size_t msg_length) {
					// why lws why
					size_t buf_length = LWS_PRE + msg_length;
					unsigned char buf[buf_length];
					unsigned char* buf_msg_ptr = buf+LWS_PRE;
					memcpy(buf_msg_ptr, msg, msg_length);
					return lws_write(client_wsi, buf_msg_ptr, msg_length, LWS_WRITE_BINARY);
				}
				void setConnected(bool c) {
					m_connected = c;
					//					std::cout<<c<<std::endl;
				}
				bool Connected() {
					return m_connected;
				}

				bool CallBack() {
					if (m_callback)
						return m_callback(m_msg, m_name);
					return true;
				}

				void ClearMsg() {
					m_msg.clear();
				}
				void MsgAppend(char *in, int len) {
					m_msg.append(in,len);
				}


			public:
				time_t last_update_time;
			private:
				std::string m_name;
				std::string m_address;				
				std::string m_path;
				struct lws_protocols *m_protocol;
				struct lws *client_wsi;
				lws_context *m_context;
				struct lws_client_connect_info i;
				bool m_connected = false;
				char p_name[16];
				std::function<bool (std::string json, std::string name)>   m_callback;
				std::string m_msg;
				std::string m_out_msg;




		};
	public:
		WebSockets() {
			for (int i =0; i < 256; i++) {
				struct lws_protocols protocol;		
				memset ( (void *)&protocol,0,sizeof (struct lws_protocols));
				protocols.push_back(protocol);
			}
		};

		WebSockets(const WebSockets & ws) {};

		~WebSockets() {
			if (context)
				lws_context_destroy(context);
		}
		void Connect(std::function<void(void)>   callback) {
			CreateContext();
			if (!context)
				throw WebSocketsException("cannot create context",-9);

			for ( auto& [key, p] : Protocols ) {
				p.Connect(context);

			}
			while (!Connected())   {
				RunStep();
				std::this_thread::yield();
			}
			callback();
		}
		void AddProtocol(std::string name, std::string address,std::string path, int port,
				 const std::function<bool (std::string json, std::string name)>  & callback = {}) {
			lws_protocols protocol;
			Protocol * p = &Protocols[name];
			*p =  Protocol(name, &protocols[idx++], p);
			p->SetAddress(address,path,port,callback);
		}
		void AddProtocol(std::string name, std::string address,std::string path, int port, int ssl_connection,
				 const std::function<bool (std::string json, std::string name)>  & callback = {}) {
			lws_protocols protocol;
			Protocol * p = &Protocols[name];
			*p =  Protocol(name, &protocols[idx++], p);
			p->SetAddress(address,path,port,ssl_connection,callback);
		}
		Protocol &operator [] (std::string name) {
			return Protocols[name];
		}
		/**
		 * Created  03/12/2022
		 * @brief   generate unique id
		 * @param   
		 * @return  
		 */
		std::string GetId() {
			uuid_t uuid;
			char id[64];
			memset (id,64,0);
			uuid_generate(uuid);
			uuid_unparse_lower(uuid, id);
			std::string s(id);
			return s;
		}

		void RunStep() {
			time_t rx_time;

			std::this_thread::yield();
			lws_service(context, 0);
			time(&rx_time);				
			for ( auto& [key, p] : Protocols ) {
				//	if  ( (p.Connected() && p.getWsi()) /*  || (p.last_update_time && rx_time-p.last_update_time>300)*/) {
				//		lws_close_reason(p.getWsi(), LWS_CLOSE_STATUS_NOSTATUS, NULL, 0);
				//		p.ClearWsi();
				//		p.Connect(context);
				//	}
			}



		}

		void Run() {
			time_t rx_time;
			isRunning = true;
			while (isRunning) {
				RunStep();
			}
		}

		void Stop() {
			isRunning = false;
		}

		bool Connected() {
			for ( auto& [key, p] : Protocols ) {
				if (!p.Connected())
					return false;
			}
			return true;
		}



		struct WebSocketsException : public std::exception {
			WebSocketsException(std::string msg, int id) {
				m_msg = msg;
				m_msg_id  = id;
			}
			const char * what () const throw ()
			{
				return m_msg.c_str();
			}
			std::string m_msg;
			int m_msg_id;
		};



	private:
		void CreateContext() {
			memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
			info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
			info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
			info.protocols = protocols.data();
			info.timeout_secs = 10;
			//	info.connect_timeout_secs = 30;
#if defined(LWS_WITH_MBEDTLS) || defined(USE_WOLFSSL)
			/*
			 * OpenSSL uses the system trust store.  mbedTLS has to be told which
			 * CA to trust explicitly.
			 */
			//	info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif

			/*
			 * since we know this lws context is only ever going to be used with
			 * one client wsis / fds / sockets at a time, let lws know it doesn't
			 * have to use the default allocations for fd tables up to ulimit -n.
			 * It will just allocate for 1 internal and 1 (+ 1 http2 nwsi) that we
			 * will use.
			 */
			info.fd_limit_per_thread = 1 + 1 + 1 + 5;

			context = lws_create_context(&info);
			if (!context) {
				lwsl_err("lws init failed\n");
				throw WebSocketsException("lws init failed",-9);

			}


			int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
				/* for LLL_ verbosity above NOTICE to be built into lws, lws
				 * must have been configured with -DCMAKE_BUILD_TYPE=DEBUG
				 * instead of =RELEASE */
				/* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
				/* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
				/* | LLL_DEBUG */;
			lws_set_log_level(logs, NULL);


		}

		struct lws_context_creation_info info;
		struct lws_context *context = 0 ;
		std::vector<struct lws_protocols> protocols;
		std::map<std::string,WebSockets::Protocol> Protocols;
		int idx = 0;
		bool isRunning = false;
};


#endif
