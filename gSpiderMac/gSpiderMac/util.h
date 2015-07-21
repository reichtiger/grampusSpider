#pragma once

#include <iostream> 

#ifdef WIN32
void ConvertGBKToUtf8(std::string& amp, std::string strGBK);
void ConvertUtf8ToGBK(std::string&amp, std::string strUtf8);
wchar_t * UTF8ToUnicode(const char* str);
char * UnicodeToANSI(const wchar_t* str);
char * UTF8ToANSI(const char* str);
char * UnicodeToANSI(const wchar_t* str);
wchar_t * ANSIToUnicode(const char* str);
#else
#include <iconv.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

// ---- from GBK to UTF8 
char* convert_gbk2utf8(char* in);
#endif

#define         DEBUG                       1
#define     DEBUG_MSG_BUF_SIZE              1024

void sp_debug(const char* fmt,...);

char* MD5String(char* inStr);
char* sha1String(char* inStr);