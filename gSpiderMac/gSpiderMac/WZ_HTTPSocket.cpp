
#ifdef WIN32
#include "StdAfx.h"
#endif

#include <string>
#include "WZ_HTTPSocket.h"
#include "util.h"

#ifdef WIN32
#pragma comment(lib, "libeay32.lib") 
#pragma comment(lib, "ssleay32.lib")
#else
#endif


void openssl_test(char* host, int port, char* pagePath)
{
	char host_port[256];memset(host_port, 0, 256);
	sprintf(host_port, "%s:%d", host, port);

	CRYPTO_malloc_init(); // Initialize malloc, free, etc for OpenSSL's use
	SSL_library_init(); // Initialize OpenSSL's SSL libraries
	SSL_load_error_strings(); // Load SSL error strings
	ERR_load_BIO_strings(); // Load BIO error strings
	OpenSSL_add_all_algorithms(); // Load all available encryption algorithms

	// Set up a SSL_CTX object, which will tell our BIO object how to do its work
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_client_method());
	// Create a SSL object pointer, which our BIO object will provide.
	SSL* ssl;
	// Create our BIO object for SSL connections.
	BIO* bio = BIO_new_ssl_connect(ctx);
	// Failure?
	if (bio == NULL) {
		printf("Error creating BIO!\n");
		//ERR_print_errors_fp(stderr);
		// We need to free up the SSL_CTX before we leave.
		SSL_CTX_free(ctx);
		return;
	}
	// Makes ssl point to bio's SSL object.
	BIO_get_ssl(bio, &ssl);
	// Set the SSL to automatically retry on failure.
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	// We're connection to google.com on port 443.
	BIO_set_conn_hostname(bio, host_port);

	// Same as before, try to connect.
	if (BIO_do_connect(bio) <= 0) {
		printf("Failed to connect!");
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return;
	}

	// Now we need to do the SSL handshake, so we can communicate.
	if (BIO_do_handshake(bio) <= 0) {
		printf("Failed to do SSL handshake!");
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return;
	}

	//ShowCerts(ssl);system("pause");

	// Create a buffer for grabbing information from the page.
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	
	// Create our GET request.
	char req_buf[2048];memset(req_buf, 0, 2048);
	sprintf(req_buf, "GET %s HTTP/1.1\nHost:%s\nUser Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\nConnection: Close\n\n", pagePath, host);
	// BIO_puts sends a null-terminated string to the server. In this case it's our GET request.
	BIO_puts(bio, req_buf);
	// Loop while there's information to be read.
	while (1) {
		// BIO_read() reads data from the server into a buffer. It returns the number of characters read in.
		int x = BIO_read(bio, buf, sizeof(buf) - 1);
		// If we haven't read in anything, assume there's nothing more to be sent since we used Connection: Close.
		if (x == 0) {
			break;
		}
		// If BIO_read() returns a negative number, there was an error
		else if (x < 0) {
			// BIO_should_retry lets us know if we should keep trying to read data or not.
			if (!BIO_should_retry(bio)) {
				printf("\nRead Failed!\n");
				BIO_free_all(bio);
				SSL_CTX_free(ctx);         
				return;
			}
		}
		// We actually got some data, without errors!
		else {
			// Null-terminate our buffer, just in case
			buf[x] = 0;
			// Echo what the server sent to the screen
			std::string gbk;
			//ConvertUtf8ToGBK(gbk, buf);
			printf("%s", gbk.c_str());
			
		}
	}
	// Free up that BIO object we created.
	BIO_free_all(bio);
	// Remember, we also need to free up that SSL_CTX object!
	SSL_CTX_free(ctx);
	
	// Return.
	return;

}


char * my_stristr(char* s,char* find)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0)
	{
		len = strlen(find);
		do
		{
			do
			{
				if ((sc = *s++) == 0)
					return (char *)0;
			} while (sc != c);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return s;
}
char* RemoveTag(char* html, char* startTag, char* endTag)
{
	char* startTagFound = NULL;
	char* endTagFound = NULL;

	//<!-- remove html comments--> (Marius Roibu)
	startTagFound = my_stristr(html,startTag);
	if(startTagFound)
		endTagFound = my_stristr(startTagFound,endTag);

	/* if startTagFound<endTagFound -> HTML is wrong */
	while((startTagFound!= NULL) && (endTagFound!= NULL) && startTagFound<endTagFound )
	{
		endTagFound += strlen(endTag);
		startTagFound[0] = 0;
		strcat(html, " ");
		strcat(html, endTagFound);
		startTagFound = my_stristr(html,startTag);
		endTagFound = my_stristr(html,endTag);
	}
	//end alteration

	return html;
}

const char HEAD_REQUEST_POST1[] =
{
	"POST %s HTTP/1.1\r\n"							// End hostname header from part1
	"Host:%s \r\n"
	"User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0) \r\n" // Specify user agent
	"Keep-Alive: 100\r\n"
	//"Connection: keep-alive\r\n"
	"Connection:close\r\n"
	"Content-Type: application/x-www-form-urlencoded\r\n" 		// Close connection after response
	"Content-Length: %d"		// content-length should less than post value,should not large than that , else server will block there.					
	"\r\n\r\n"
	"%s\r\n"
};
const char HEAD_REQUEST_POST2[] =
{
	"POST %s HTTP/1.1\r\n"							// End hostname header from part1
	"Host:%s \r\n"
	"User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0) \r\n" // Specify user agent
	"Accept-Language: zh-cn,zh;q=0.5\r\n"
	"Accept-Encoding: gzip,deflate\r\n"
	"Accept-Charset: gb2312,utf-8;\r\n"
	"Keep-Alive: 150\r\n"
	"Transfer-Encoding: chunked\r\n"
	"Connection: Keep-Alive\r\n"
	"Content-Type: multipart/form-data; boundary=%s\r\n"
	//"Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
	"Content-Length: %d\r\n"		// content-length should less than post value,should not large than that , else server will block there.					
	"\r\n"
	"%s"
	"\r\n"
};
/*
  class WZ_HTTTPSocket implementation
 
 
 */
WZ_HTTPSocket::WZ_HTTPSocket(void)
{
	init_chars();
}
WZ_HTTPSocket::WZ_HTTPSocket(int i)
{
	init_chars();
}
WZ_HTTPSocket::WZ_HTTPSocket(char* iden)
{
	init_chars();
	strncpy(identifier, iden, strlen(iden));

}

void WZ_HTTPSocket::init_chars()
{
	sock = NULL;
    html = NULL;
	memset(encodeType, 0, 32);
	total_bytes = 0;
	ctx = NULL;
	bio = NULL;
	ssl = NULL;
	isHTTPS = 0;
	flag_cleanALL = 0;
	logReport = false;
	fLog = NULL;
	names_num = 0;
	ptrConnectState = NULL;
	ptrReadSome = NULL;
	ptrReadAll = NULL;
	tag = 0;
    isPost = 0; // default GET
	memset(identifier, 0, 64);
	memset(session, 0, SESSION_STR_SIZE);
}

WZ_HTTPSocket::~WZ_HTTPSocket(void)
{
	if (NULL != fLog) fclose(fLog);

#ifdef WIN32
	closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    
	delete[] html;
	
	sock = NULL;
	ctx = NULL;
	bio = NULL;
	ssl = NULL;
	//printf("destroying WZ_HTTPSocket [ tag=%d ] object...\n", tag);
}

int WZ_HTTPSocket::cleanALL()
{
	flag_cleanALL = 1;
	while (flag_cleanALL == 1)
	{
		//Sleep(1);
	}

	return 2;
}
/*
 * return ip addr in char mode if success, else NULL
 */
char* WZ_HTTPSocket::init_sock(char* hostname, int port)
{
	if (sock != -1) {
#ifdef WIN32
		closesocket(sock);
#else
        close(sock);
#endif
		sock = NULL;
	}

#ifdef WIN32
	WSADATA wsadata;
	if((WSAStartup(MAKEWORD(1,1),&wsadata))!=0 || (LOBYTE(wsadata.wVersion )!=1 || HIBYTE(wsadata.wVersion)!=1))
	{
		return NULL; // failed
	}
#endif
	if(strlen(hostname)<3)
		return NULL;

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sock == -1)	return NULL;
    he = NULL;
	if((he = gethostbyname(hostname)) == 0)
		return NULL;

    // print information about this host:
    printf("Official name == %s\n", he->h_name);
    struct in_addr **addr_list;
    addr_list = (struct in_addr **)he->h_addr_list;
    char* ip1 = inet_ntoa(*addr_list[0]);
    printf("ip1= %s\n", ip1);
    
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = ((struct in_addr *)(he->h_addr))->s_addr;
	saddr.sin_port = htons((unsigned short int)port);
    
	return ip1;
}

// if cached , use this function
int WZ_HTTPSocket::init_ip_sock(char* ipaddr, int port)
{
    
    if (sock != -1) {
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        sock = NULL;
    }
    
#ifdef WIN32
    WSADATA wsadata;
    if((WSAStartup(MAKEWORD(1,1),&wsadata))!=0 || (LOBYTE(wsadata.wVersion )!=1 || HIBYTE(wsadata.wVersion)!=1))
    {
        return NULL; // failed
    }
#endif
    if(strlen(ipaddr)<9) // for -> 192.168.1.1, 10.10.0.1
        return NULL;
    
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (sock == -1)	return NULL;
    
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(ipaddr);
    saddr.sin_port = htons((unsigned short int)port);
    
    return 0;
}

void WZ_HTTPSocket::getALLData(char* html, int len)
{
	// do nothing , let extends class to
    printf("call default virtual getALLData func\n");
}

size_t WZ_HTTPSocket::RecvBytes(void)
{
	int recvdbytes = 0;

    sp_debug("reading data....\n");
    
	html = new char[MAXPACKETSIZE];
    if (!html) {
        return -1;
    }
	memset(html, 0, MAXPACKETSIZE);
	
	if (isHTTPS == 0)
	{
        
        fd_set read_flags,write_flags; // you know what these are
        struct timeval waitd;
        char inbuff[4096];      // Buffer to read incoming data into
        int err;
        
        int wait_times = 0;
        
        while(1) {
            waitd.tv_sec = 0;     // Make select wait up to 1 second for data
            waitd.tv_usec = 1000;    // and 0 milliseconds.
            FD_ZERO(&read_flags); // Zero the flags ready for using
            FD_ZERO(&write_flags);
            FD_SET(sock, &read_flags);
            //if(strlen(outbuff)!=0) FD_SET(sock, &write_flags);
            err = select(sock+1, &read_flags, NULL, (fd_set*)0,&waitd);
            if(err < 0) continue;
            if(FD_ISSET(sock, &read_flags)) { //Socket ready for reading
                FD_CLR(sock, &read_flags);
                memset(&inbuff,0,sizeof(inbuff));
                ssize_t nread = read(sock, inbuff, sizeof(inbuff)-1);
                if (nread <= 0) {
                    close(sock);
                    printf("\nSOCK :======= END (%ld)======\n", total_bytes);
                    break;
                }
                else {
                    //printf("%s",inbuff);
                    if (ptrReadSome != NULL) {
                        ptrReadSome(this, inbuff, nread);
                    }
                    memcpy(html+total_bytes, inbuff, nread);
                    total_bytes += nread;
                }
                wait_times = 0;
            }else{
                wait_times++;
                //printf("%d\r", wait_times);
                if (wait_times > 500000) {
                    printf("time out error , > 500000. retry...\n");
                    return -5;
                }
            }
            
            //            if(FD_ISSET(sock, &write_flags)) { //Socket ready for writing
            //                FD_CLR(sock, &write_flags);
            //                write(sock,outbuff,strlen(outbuff));
            //                memset(&outbuff,0,sizeof(outbuff));
            //            }
            // now the loop repeats over again
        }// while
        
        if (ptrReadAll != NULL)
        {
            ptrReadAll(this, html, total_bytes);
        }
        return total_bytes;
	}
    
    // https link
	else{
		// Loop while there's information to be read.
		int x = 0;
		size_t recvdBytes = 0;
		while (1) {
			// BIO_read() reads data from the server into a buffer. It returns the number of characters read in.
			x = BIO_read(bio, html+recvdBytes, MAXPACKETSIZE);
			// If we haven't read in anything, assume there's nothing more to be sent since we used Connection: Close.
			if (x == 0) {
				break;
			}
			// If BIO_read() returns a negative number, there was an error
			else if (x < 0) {
				// BIO_should_retry lets us know if we should keep trying to read data or not.
				if (!BIO_should_retry(bio)) {
					printf("\nRead Failed!\n");
					BIO_free_all(bio);
					SSL_CTX_free(ctx);         
					return -3;
				}
			}
			// We actually got some data, without errors!
			else {
				// Null-terminate our buffer, just in case
				recvdBytes += x;
			}
		}
		if (ptrReadAll != NULL)
		{
			ptrReadAll(this, html, recvdBytes);
		}
		total_bytes = recvdBytes;
		return total_bytes;
	}
	
	//getALLData(html, recvdbytes);
	return recvdbytes;
}

int WZ_HTTPSocket::setnonblock(int n_timeout)
{
	
#ifdef WIN32
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char*) &n_timeout, sizeof(int));
#else
		struct timeval timeout;

		timeout.tv_sec  = 1;
		timeout.tv_usec = n_timeout;

		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char*) &timeout, sizeof(timeout)))
			return 0;
#endif
		return 1;
	
}

int WZ_HTTPSocket::SendPackets(char* buf)
{
	return send(sock, buf, strlen(buf), 0);
}

int WZ_HTTPSocket::SendGETRequest(char* page_dir)
{
	
	memset(head_packet, 0, REQ_LEN);
	
	sprintf(head_packet,"GET %s HTTP/1.0\r\nAccept: */*\r\nHost: %s\r\nUser-Agent: %s \r\nConnection: Close\r\n\r\n", page_dir, host, USER_AGENT);
	
	return SendPackets(head_packet);
	
}
int WZ_HTTPSocket::SendPOSTRequest(char* page_dir, char* postValuesStr)
{
	memset(head_packet, 0, REQ_LEN);
    int postLen = 0;
    if (postValuesStr) {
        postLen = (int)strlen(postValuesStr);
    }
	
	sprintf(head_packet, HEAD_REQUEST_POST1, page_dir, host, postLen, postValuesStr);

	if (logReport)
	{
		fwrite(head_packet, strlen(head_packet), 1, fLog);
		fflush(fLog);
	}
	
	return SendPackets(head_packet);

}
int WZ_HTTPSocket::SendPOSTRequest(char* page_dir, char* postValuesStr, char* boundary)
{
	memset(head_packet, 0, REQ_LEN);
    int postLen = 0;
    if (postValuesStr) {
        postLen = (int)strlen(postValuesStr);
    }
	sprintf(head_packet, HEAD_REQUEST_POST2, page_dir, host, boundary, postLen, postValuesStr);
	if (logReport)
	{
		fwrite(head_packet, strlen(head_packet), 1, fLog);
		fflush(fLog);
	}

	return SendPackets(head_packet);

}
bool WZ_HTTPSocket::parseURL(char* startURL)
{
	if(startURL == NULL)
		return false;

	char* p = startURL;
	if (strncmp("http://", p, 7) == 0)
	{
		isHTTPS = 0;
		memset(page_dir, 0, PAGE_DIR_LEN);

		char* p2 = p + 7;
		while(*p2 != '/' && *p2 != '\0') p2++;

		char* xn = new char[p2-p-6];
        if (!xn) {
            return false;
        }
		// IMPORTANT !!!! if here not set zero into array, later the delete xn will rise an error in Debug mode.
		// the length should be p2-p-7, but the end \0 will need one char. so p2-p-6 will be applyed.
		memset(xn, 0, p2-p-6);
		memcpy(xn, p+7, p2-p-7);
		char* pTmp = NULL;
		pTmp = strstr(xn, ":");
		if (pTmp != NULL)
		{
			memcpy(host, xn, pTmp-xn);
			host[pTmp-xn] = '\0';
			port = atoi(pTmp+1);
		}else{
			strcpy(host, xn);
			port = 80;
		}
		
		delete []xn; //why can not delete ? Now it can be delete because all xn has been cleared with zero and \0 with end.
		
		printf("Host=%s, port=%d\n", host, port);
		char* p3 = p2;
		while(*p3 != '\0') p3++;
		memcpy(page_dir, p2, p3-p2);
		// 若没带/结尾，自动加上。
		if(strncmp("/", page_dir,1) != 0) 
		{
			memset(page_dir, '/', 1);
		}
		
		return true;
	}
	else if(strncmp("https://", p, 8) == 0){
		isHTTPS = 1;  // mark https
		memset(page_dir, 0, PAGE_DIR_LEN);

		char* p2 = p + 8;
		while(*p2 != '/' && *p2 != '\0') p2++;

		char* xn = new char[p2-p-7];
		// IMPORTANT !!!! if here not set zero into array, later the delete xn will rise an error in Debug mode.
		// the length should be p2-p-7, but the end \0 will need one char. so p2-p-6 will be applyed.
		memset(xn, 0, p2-p-7);
		memcpy(xn, p+8, p2-p-8);
		char* pTmp = NULL;
		pTmp = strstr(xn, ":");
		if (pTmp != NULL)
		{
			memcpy(host, xn, pTmp-xn);
			host[pTmp-xn] = '\0';
			port = atoi(pTmp+1);
		}else{
			strcpy(host, xn);
			port = 443;
		}

		delete []xn; //why can not delete ? Now it can be delete because all xn has been cleared with zero and \0 with end.

		printf("Host=%s, port=%d\n", host, port);
		char* p3 = p2;
		while(*p3 != '\0') p3++;
		memcpy(page_dir, p2, p3-p2);
		// 若没带/结尾，自动加上。
		if(strncmp("/", page_dir,1) != 0) 
		{
			memset(page_dir, '/', 1);
		}
		//openssl_test(host, port, page_dir);
		return true;
	}
	return false;
}

int WZ_HTTPSocket::ssl_init()
{
	char host_port[256];memset(host_port, 0, 256);
	sprintf(host_port, "%s:%d", host, port);

	CRYPTO_malloc_init(); // Initialize malloc, free, etc for OpenSSL's use
	SSL_library_init(); // Initialize OpenSSL's SSL libraries
	SSL_load_error_strings(); // Load SSL error strings
	ERR_load_BIO_strings(); // Load BIO error strings
	OpenSSL_add_all_algorithms(); // Load all available encryption algorithms

	// Set up a SSL_CTX object, which will tell our BIO object how to do its work
	ctx = SSL_CTX_new(SSLv23_client_method());
	// Create a SSL object pointer, which our BIO object will provide.
	// Create our BIO object for SSL connections.
	bio = BIO_new_ssl_connect(ctx);
	// Failure?
	if (bio == NULL) {
		printf("Error creating BIO!\n");
		//ERR_print_errors_fp(stderr);
		// We need to free up the SSL_CTX before we leave.
		SSL_CTX_free(ctx);
		return -1;
	}
	// Makes ssl point to bio's SSL object.
	BIO_get_ssl(bio, &ssl);
	// Set the SSL to automatically retry on failure.
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	// We're connection to google.com on port 443.
	BIO_set_conn_hostname(bio, host_port);
	return 0;
	// left to connect....
}

int WZ_HTTPSocket::ssl_connect_wait()
{
	if (BIO_do_connect(bio) <= 0) {
		printf("Failed to connect %s!\n", host);
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return -1;  // connect error
	}

	// Now we need to do the SSL handshake, so we can communicate.
	if (BIO_do_handshake(bio) <= 0) {
		printf("Failed to do SSL handshake!");
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return -2;  // ssl handshake error
	}

	if(ptrConnectState != NULL) 
		ptrConnectState(this, 1);

	return 0;
}
int WZ_HTTPSocket::ssl_get()
{
	//ShowCerts(ssl);system("pause");

	// Create a buffer for grabbing information from the page.
	
	printf("%s,%s", host, page_dir);
	// Create our GET request.
	char req_buf[1024];memset(req_buf, 0, 1024);
	sprintf(req_buf, "GET %s HTTP/1.1\nHost:%s\nUser Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\nConnection: Close\n\n", page_dir, host);
	// BIO_puts sends a null-terminated string to the server. In this case it's our GET request.
	BIO_puts(bio, req_buf);

	return 0;
	//// Loop while there's information to be read.
	//int total = 0;
	//int x = 0;
	//int recvdBytes = 0;
	//while (1) {
	//	// BIO_read() reads data from the server into a buffer. It returns the number of characters read in.
	//	x = BIO_read(bio, html+recvdBytes, MAXPACKETSIZE);
	//	// If we haven't read in anything, assume there's nothing more to be sent since we used Connection: Close.
	//	if (x == 0) {
	//		break;
	//	}
	//	// If BIO_read() returns a negative number, there was an error
	//	else if (x < 0) {
	//		// BIO_should_retry lets us know if we should keep trying to read data or not.
	//		if (!BIO_should_retry(bio)) {
	//			printf("\nRead Failed!\n");
	//			BIO_free_all(bio);
	//			SSL_CTX_free(ctx);         
	//			return -3;
	//		}
	//	}
	//	// We actually got some data, without errors!
	//	else {
	//		// Null-terminate our buffer, just in case
	//		recvdBytes += x;
	//	}
	//}
	//ptrReadAll(this, html, recvdBytes);

	//return recvdBytes;
}

int WZ_HTTPSocket::ssl_post()
{
	return 0;
}
void WZ_HTTPSocket::ssl_shutdown()
{
	return;
	// Free up that BIO object we created.
	BIO_free_all(bio);
	// Remember, we also need to free up that SSL_CTX object!
	SSL_CTX_free(ctx);
}

int WZ_HTTPSocket::connect_wait(void)
{
	if (isHTTPS == 0)
	{
#ifdef WIN32
		if (connect(sock, (LPSOCKADDR) &saddr, sizeof(saddr)) == SOCKET_ERROR)
		{
			printf("[ERROR] connect to %s error\n", host);
			closesocket(sock);
			return -1;
		}
		
#else
        if (connect(sock, (struct sockaddr *) &saddr, sizeof(struct sockaddr))<0)
		{
			printf("[ERROR] connect to %s error\n", host);
			close(sock);
			return -1;
		}
#endif
	}else{
		//openssl_test(ps->host, 443, ps->page_dir);
		ssl_init();
		ssl_connect_wait();
	}
	
	if(ptrConnectState != NULL) 
		ptrConnectState(this, 1);

	return 0;
}

void WZ_HTTPSocket::do_get()
{
	if (isHTTPS == 0)
	{
		int n = SendGETRequest(page_dir);
        printf("Request sent %d\n", n);
	}
	else ssl_get();

	if(ptrConnectState != NULL) 
		ptrConnectState(this, 2);
}

void WZ_HTTPSocket::do_post(char* postStr)
{
	if (isHTTPS == 0)
	{
		SendPOSTRequest(page_dir, postStr);
	}
	else ssl_post();

	if(ptrConnectState != NULL) 
		ptrConnectState(this, 2);
}


void WZ_HTTPSocket::setLogReport()
{
	fLog = fopen("log.txt", "w+");
	logReport = true;
}

int WZ_HTTPSocket::parseHeaderInfo()
{
	char* p = NULL;
	char* p2 = NULL;
	ret_status = 0;

	char* buf = new char[header_len+1];
	memset(buf, 0, header_len+1);
	strncpy(buf, html_header, header_len);

	p = strstr(buf, " ");
	if (p != NULL)
	{
		 p2 = strstr(p+1, " ");
		 if (p2 != NULL)
		 {
			 *p2 = '\0';
		 }

        ret_status = atoi(p);
        sp_debug("http status=%d\n", ret_status);
        
	}

    switch (ret_status) {
        case 200:
        {
            char* pContentType = strstr(p2+1, "Content-Type");
            if (pContentType)
            {
                char* pContentTypeEnd = strstr(pContentType+1, "\r\n");
                if (pContentTypeEnd)
                {
                    *pContentTypeEnd = '\0';
                    
                    char* pCharSet = strstr(pContentType, "charset=");
                    if (pCharSet)
                    {
                        strncpy(encodeType, pCharSet+8, strlen(pCharSet + 8));
                    }
                    
                }
            }
        }
            break;
            
        case 404:
        {
            
        }
            break;
            
        default:
            break;
    }
	
	delete [] buf;

	return ret_status;
}

int WZ_HTTPSocket::splitHTMLHeader(void)
{
	if (html == NULL) return NULL;

	html_header = html;

    size_t len = strlen(html);
	char* p = NULL;
	p = strstr(html, "\r\n\r\n");
	if (p != NULL)
	{
		header_len = (p-html);
		html_body = p+4;
		html_body_len = len - header_len - 4;

		*p = '\0'; // split, left html=header

		ret_status = parseHeaderInfo();

		return ret_status;
	}
	return 0;
}

/* change html code to lowercase, only those bwtween '<' and '>' will be changed. 
 * also ,if set_return_to_black is true ,then '\n' '\t' will change into black
 *
 */
char* WZ_HTTPSocket::change2lower(char* s,bool set_return_to_blank)
{
	if (!s) return NULL;
	char* t = s;
	bool start = false;
	while(*t != '\0'){
		if (!start && *t == '<') start = true;
		if (start && *t == '>') start = false;

		if (start) *t = tolower(*t);

		if (set_return_to_blank && (*t == '\n' || *t == '\t')) *t = ' ';
		t++;
	}
	return s;
}
// replacedStr 外部传入指针，空间由外部调用控制，申请。这里不做检查

bool WZ_HTTPSocket::replaceTag(char* inStr, char* srcTag, char* destTag, char* replacedStr)
{
	bool success = false;

	size_t srcTagLen = strlen(srcTag);
	size_t dstTagLen = strlen(destTag);

	char* pStart = inStr;
	char* p = NULL;
	char* lastpStart = NULL;
	p = strstr(pStart, srcTag);
	
	while ( p != NULL)
	{
		// save reservd str first
		strncpy(replacedStr, pStart, (p-pStart));
		replacedStr += (p-pStart);

		// save destTag, here = replace
		strncpy(replacedStr, destTag, dstTagLen);
		replacedStr += dstTagLen;

		// adjust pointer
		pStart = p + srcTagLen;
		p = strstr(pStart, srcTag);
		lastpStart = pStart;
	}

	if (strlen(lastpStart) >0)
	{
		strcpy(replacedStr, lastpStart);
	}

	success = true;
	return success;
}
/* UnHtml
* html -> text <- 
* html = "<p align="left"><font face="Arial" size="2">TesT123</font></p>"
* text => TesT123
*/
int WZ_HTTPSocket::UnHtml(char* html, char* text,int maxout)
{
	int i, m, x=0, pOpen=0;
	unsigned char curC;

	memset(text,0,maxout);

	RemoveTag(html,"<!--","-->");
	RemoveTag(html,"<script","</script>");
	RemoveTag(html,"<style","</style>");

	m=MIN((signed)strlen(html),maxout);


	for(i=0;i<m;i++)
	{
		curC=html[i];

		if(curC=='<')
		{
			pOpen=1;

			/* "a<br>b"  => "a b";
			"a <br>b" => "a b" */
			if(x && text[x-1]!=' ')
				text[x++]=' ';
		}
		else
			if(curC=='>')
				pOpen=0;

		if(pOpen==0 && curC!='>')
		{
			/*                     RemoveShit                  */
			if( /*!( (curC>=32 && curC<=126) || (curC>=192 && curC<=255) ) ||*/ curC=='\''  || curC=='\"'  || curC=='\\')
				curC=' ';

			if(x && text[x-1]==' ' && curC==' ')	//if x>0 and last char is space and current char is space -> don't add this char
				continue;
			else
				text[x++]=curC;
		}
	}

	return x;
}
/*
 * this function  change <br> to '\n' for print.
 * suppose back_txt has cleared.
 */
int WZ_HTTPSocket::br2return(char* html, char* back_txt)
{
	
	char* pStart = html;
	char* pf = strstr(html, "<br>");
	//char* pf_old = NULL;
	int offset = 0;
	int step = pf - pStart;
	const char* nn = "\r\n";

	while(pf != NULL){
		memcpy(back_txt+offset, pStart, step);
		memcpy(back_txt+offset+step, nn, 2);

		pStart = pStart + step + 4;
		offset = offset + step+2;
		pf = strstr(pStart, "<br>");
		if(pf != NULL){
			step = pf - pStart;
			//pf_old = pf;
		}
	}

	memcpy(back_txt+offset, pStart, strlen(pStart));
	return 0;
}


/************************************************************************/
/* Class CookieManager  implementation                                  
/************************************************************************/

CookieManager* CookieManager::m_pInstance = NULL;

CookieManager* CookieManager::getInstance()  
{  
	return m_pInstance ? m_pInstance:(m_pInstance = new CookieManager);
}

void	CookieManager::parseCookie(const char* str)
{
	//printf("Cookie Get:%s\n", str);
}

char*	CookieManager::getCookie(char* key)
{
	return NULL;
}

bool	CookieManager::setCookie(char* key, char* val)
{
	return false;
}