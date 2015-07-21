#ifdef WIN32
#include "StdAfx.h"
#endif

#include "util.h"
#include <stdio.h>

#ifdef WIN32
void ConvertGBKToUtf8(std::string& amp, std::string strGBK)
{ 
	int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, NULL,0); 
	unsigned short * wszUtf8 = new unsigned short[len+1]; 
	memset(wszUtf8, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, (LPWSTR)wszUtf8, len); 
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
	char *szUtf8=new char[len + 1]; 
	memset(szUtf8, 0, len + 1); 
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL); 
	//strGBK = szUtf8; 
	amp=szUtf8;
	delete[] szUtf8; 
	delete[] wszUtf8; 
}

void ConvertUtf8ToGBK(std::string&amp, std::string strUtf8)
{ 
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, NULL,0); 
	unsigned short * wszGBK = new unsigned short[len+1]; 
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, (LPWSTR)wszGBK, len); 
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL,NULL); 
	//strUtf8 = szGBK; 
	amp=szGBK;
	delete[] szGBK; 
	delete[] wszGBK; 
}

wchar_t * UTF8ToUnicode(const char* str)
{
	int textlen;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
	memset(result, 0, (textlen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

char * UnicodeToANSI(const wchar_t* str)
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen + 1)*sizeof(char));
	memset(result, 0, sizeof(char)* (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

char * UTF8ToANSI(const char* str)
{
	wchar_t* tmp = UTF8ToUnicode(str);
	char* result = UnicodeToANSI(tmp);
	free(tmp);
	return result;
}

wchar_t * ANSIToUnicode(const char* str)
{
	int textlen;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
	memset(result, 0, (textlen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

#else

// ------------- on Mac OS or Linux platform --------------------
char* convert_gbk2utf8(char* in)
{
    if (in == NULL) {
        return NULL;
    }
    
    iconv_t converter = iconv_open("UTF-8", "GBK");
    if(converter == (iconv_t)(-1))
    {
        printf("Doesn't support convert GBK -> UTF-8! \n");
        return  NULL;
    }
    else
    {
        printf("Support GBK -> UTF8 !\n");
    }
    
    size_t insize;
    const char *input = in;//汉字随便加//应是const char *而不是char *
    
    insize = strlen(input);
    
    //存储转换结果的字符串
    size_t outsize = insize * 3+1;
    char *output = new char[outsize];
    char *output_old = output;//记录转换后的字符串的地址
    memset(output,0,outsize);
    
    //转换
    //size_t rc = iconv(conveter,(const char **)&input,&insize,&output_old,&outsize);
    size_t ret = iconv(converter, (char**)&input, &insize, &output_old, &outsize);
    if(ret==(size_t)-1){
        std::cout<<"-------------------converting failed"<<std::endl;
        return output;  // strange . failed here but output has transfered string.
    }
    
    //cout<<"output is : "<<output;
    //cout<<"  outsize="<<outsize<<endl;
    
    return output;
    iconv_close(converter);
}

#endif
// ------------- use standard C library to transfer -------------------

FILE* flog = NULL;

void sp_debug(const char* fmt,...)
{
    if (DEBUG) {
        va_list args;
        va_start(args, fmt);
        char msgbuf[DEBUG_MSG_BUF_SIZE];
        vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
        va_end(args);
        printf("%s", msgbuf);
        
        if (flog) {
            fprintf(flog, msgbuf);
            fflush(flog);
        }
    }
}

char* MD5String(char* inStr)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    
    MD5((unsigned char*)&inStr, strlen(inStr), (unsigned char*)&digest);

    char* mdString = new char[33];
    
    for(int i = 0; i < 16; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    mdString[33] = '\0';
    
    printf("\ndigest= %s\n", mdString);
    
    return mdString;
}

char* sha1String(char* inStr)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)inStr, strlen(inStr), hash);
    
    char* sha1String = new char[33];
    
    for(int i = 0; i < 16; i++)
        sprintf(&sha1String[i*2], "%02x", (unsigned int)hash[i]);
    sha1String[33] = '\0';
    
    printf("\nSHA1 = %s\n", sha1String);
    
    return sha1String;
}
