/*
 *  Design : For a HTTPSocket . Users shoulg only care for GET and POST methods. 
 *	others should be encapsulated into these two functions.
 *
 *	Modify Date : 2010-8-16
 *	By wzhao
 *
 *

   Modify DAte 2014-5-10 
   By wzhao



   Use How to -------------------

   parseURL,
   then connect_to_host()
   then send Request or Post

   then readBytes()
 */

#pragma once
#ifdef WIN32
#include "winsock2.h"
#else
#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#endif

#include "stdio.h"
#pragma   warning(disable:4996)
#pragma   warning(disable:4101)
#pragma	  warning(disable:4267)
#pragma	  warning(disable:4312)
#pragma	  warning(disable:4244)

#define			FIRSTTIMEOUT			0      /* MSeconds */
#define			TIMEOUTs				300      /* MSeconds */
#define			MAXPACKETSIZE			900000		/* max packet buf size */
#define			MAXURLSIZE				5000
#define			VERSIONINFO				"1.0"
#define         USER_AGENT              "Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)"
#define			REQ_LEN					8192*2
#define			PAGE_DIR_LEN			1024
#define			MIN(a,b)				(a<b)?a:b
#define			SESSION_STR_SIZE		130*3

#include <string>
#include <vector>
/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* OpenSSL headers */
#ifdef WIN32
#include <openssl/bio.h> // BIO objects for I/O
#include <openssl/ssl.h> // SSL and SSL_CTX for SSL connections
#include <openssl/err.h> // Error reporting
#else
#include    <openssl/x509.h>
#include    <openssl/x509v3.h>
#include    <openssl/pem.h>
#include    <openssl/ssl.h>
#endif

/*
   cookie class , for all threads using,
   it is not thread-safe, so need to instance outside threads start , as global

  外部初始化 before invoke main  
  const CookieManager* CookieManager::m_instance = new CookieManager; 
 */
class CookieManager  
{  
public:
	char	host[64];
	char	cookie_buf[1024];
private:  
	static CookieManager* m_pInstance;   
public:
	CookieManager(){};
	~CookieManager(){};
	static CookieManager* getInstance();

	void	parseCookie(const char* str);
	char*	getCookie(char* key);
	bool	setCookie(char* key, char* val);
}; 


/***************************************************************
 * httpSocket class
 *
 ***************************************************************/

// call back to tell connect ok or not
typedef void(*connectStatePtr)(void* sender, size_t);
// call back function, read one time and call back,
typedef void(*readBytesPtr)(void* sender, char*, size_t);
/**
 * callback function, read all finished and back.
 * NOTE !! object sender should not be delete in this function . cause other
 * procedure is still need to be execute after this call.
 */
typedef void(*readALLPtr)(void* sender, char*, size_t);
 

class WZ_HTTPSocket
{
public:
	WZ_HTTPSocket(void);
	WZ_HTTPSocket(int tag);
	WZ_HTTPSocket(char* iden);
	virtual ~WZ_HTTPSocket(void);
	void init_chars();
public:
	connectStatePtr ptrConnectState;
	readBytesPtr	ptrReadSome;
	readALLPtr		ptrReadAll;

#ifdef WIN32
	SOCKET	sock;
#else
    int     sock;
#endif
    
	int		tag;
	int		depth; // in which depth
    int     isPost;

	size_t  total_bytes;
	char	identifier[64];// string tag
	int		isHTTPS;
	struct hostent *he;
	struct sockaddr_in saddr;
	char head_packet[REQ_LEN];
	//char	html[MAXPACKETSIZE];
	bool    flag_cleanALL; // for release class , when in recving loop if delete got , should break first and release

	SSL_CTX* ctx;
	// Create a SSL object pointer, which our BIO object will provide.
	SSL* ssl;
	// Create our BIO object for SSL connections.
	BIO* bio;

	char*	html;									// pointer to html all
	char	encodeType[32];							// UTF-8 or GBK, etc
	char	host[512];
	int		port;
	char	page_dir[PAGE_DIR_LEN];					// path to page with argvs
	int		header_len;
	char*	html_header;
	int		html_body_len;							
	char*	html_body;								// html contents except the header

	bool	logReport;								// if output log
	FILE*	fLog;
	int		names_num;
	std::vector<std::string> nicknameList;			// other nicknames in page
	char	cur_nickname[64];						// the user nickname of BBS
	char	session[SESSION_STR_SIZE];				// save the session string

	int		ret_status;		// header info ,status , 200, 301, etc
	char	ret_encode[32];	// encoding type, in content-type

public:
	bool    parseURL(char* url);
	bool    redirectURL(char* url);
    int     init_ip_sock(char* ipaddr, int port=80); // use cached ip
	char*   init_sock(char* hostname, int port=80);  // use new dns ip
	int     connect_wait(void);
	void    do_get();
	void    do_post(char* postStr);
	size_t  RecvBytes(void);
	int     setnonblock(int n_timeout=0);

	// append post value into string for later use, utf8 is default
	void    addPostValue(char* key, char* vStr);
	// https related
	int ssl_init();
	int ssl_connect_wait();
	int ssl_get(); // return read data size
	int ssl_post(); // return read data size
	void ssl_shutdown();

	int cleanALL();
	// helper functions

	int SendGETRequest(char* url);
	int SendPOSTRequest(char* url, char* postValuesStr);
	int SendPOSTRequest(char* url, char* postValuesStr, char* boundary);
	int SendPackets(char* buf);

	virtual void getALLData(char* html, int len);

	void setLogReport();
	bool replaceTag(char* inStr, char* tag, char* replacedTag, char* replacedStr);
	char* change2lower(char* s,bool set_return_to_blank=false);
	int UnHtml(char* html, char* text, int maxout);
	int br2return(char* html, char* back_txt); // change <br> to \n for print out

	int splitHTMLHeader(void); // return html body start pointer
	int parseHeaderInfo(); // return HTTP Status number, ex:200
	
	// HTML parser
	
};
