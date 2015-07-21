//
//  main.cpp
//
//  Created by zhaohu on 14-9-28.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

// NOTE !!!!!!

// change `LLVM 5.1 - Language C++` settings option .

// to -> libstdc++, standard lib

// use  ../v8libs_3.26.33/mac/debug, or release.

/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <v8.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <string.h>

#include <assert.h>
#include "minicfg.h"
#include <wchar.h>
#include "GSTask.h"
#include "WZ_HTTPSocket.h"
#include "HTMLTree.h"
#include "util.h"
#include "testWrap.h"
#include "AjaxWrap.h"
#include "WindowWrap.h"
#include "DomWrap.h"
#include "spiderActions.h"
#include "NavigatorWrap.h"
#include "LocationWrap.h"

using namespace std;
using namespace v8;
using namespace gSpider;


/**
 * global vars
 */

char    gProto[16];
char    gHost[64];
char*   gPath = NULL;
int     gPort = 80;

int     gClick_pages = 0;
int     gCurPage = 0;

EVENT_OBJS* eventList = NULL;

//vector<GSTask> tasklist;
//vector<GSTask>::iterator itorTask;

vector<HTMLBranch*> createdJSNodes;
vector<HTMLBranch*>::iterator createdNode;

minicfg* dns_cache = NULL;
/**
 *  function declarationsß
 *
 */

int txt_chrome_task(GSTask* task);
wstring read_js_content(char* jsfile);
char* get_html(char* url, int& encodeType, int isJS, int& status);

/**
 *  functino implements
 */
void read_some(void* sender, char* str, size_t n)
{
    printf("%s", str);
    if (n <= 0) {
        // exit(99);
    }
}

void read_all_finished(void* sender, char* str, size_t n)
{
	printf("read_all_finished:::::::::::::: back socket bytes = %ld bytes. \n\n", n);
    //printf("%s\n", str);
    if (n <= 0) {
       // exit(99);
    }
}

// read js from cache , if
char* get_js_from_cache(char* url, int& encodeType, int isJS, int& status)
{
    // get MD5 str
    
    char filename[128];
    memset(filename, 0, 128);
    char* js_cache_md5 = sha1String(url);
    strcat(filename, js_cache_md5);
    strcat(filename, ".js");
    FILE* jsf = NULL;
    jsf = fopen(filename, "r+");
    if (jsf != NULL) {
        printf("++++++++++++++++++++++++++ cache js found \n");
        fclose(jsf);
        wstring ww = read_js_content(filename);
        return w2m(ww.c_str());
        
    }else{
        fclose(jsf);

    retryit:
        
        char* jsHTML = get_html(url, encodeType, isJS, status);
        if (!jsHTML) {
            goto retryit;
        }
        jsf = fopen(filename, "w+");
        fwrite(jsHTML, 1, strlen(jsHTML), jsf);fclose(jsf);
        return jsHTML ;
    }
}

char* get_html(char* url, int& encodeType, int isJS, int& status)
{
    
    WZ_HTTPSocket* hs = new WZ_HTTPSocket(0);
	if (!hs->parseURL(url))
        return NULL;
    
    if (isJS == 0) {
        if (hs->isHTTPS == 1) {
            strcat(gHost, "https://");
        }else{
            strcat(gHost, "http://");
        }
        strcat(gHost, hs->host);
    }
    
    
	hs->ptrReadAll = read_all_finished;
	//hs->ptrReadSome = read_some;
    int trytimes = 0;
  
tryagain:
    
    close(hs->sock);
    char* cached_ip = dns_cache->getValue(hs->host);
    if (cached_ip) {
        hs->init_ip_sock(cached_ip);
    }else{
        // update dns cache , if exist , use it
        char* ipaddr = hs->init_sock(hs->host, hs->port);
        if (ipaddr == NULL)
        {
            sp_debug("init sock error\n");
            return NULL;
        }
        
        dns_cache->saveValue(hs->host, ipaddr);
    }
    
    
	if (hs->connect_wait() <0){
		printf("connect failed\n");
		return NULL;
	}
    
	hs->do_get();
	if (hs->RecvBytes()>0)
	{
        status = hs->splitHTMLHeader();
        //printf("html head==============\n%s\n", hs->html);
        //printf("\nhtml body len === %d, %s\n", hs->html_body_len, hs->encodeType);
        if (strcmp(hs->encodeType, "gbk")==0) {
            encodeType = 0;
        }
        else if(strcmp(hs->encodeType, "UTF-8") == 0){
            encodeType = 1;
        }
		
        char* html = new char[hs->html_body_len+1];
        if (html) {
            strncpy(html, hs->html_body, hs->html_body_len);
            html[hs->html_body_len] = '\0';
            
            delete hs;
            return html;
        }
    }else{
        trytimes++;
        if (trytimes < 3) {
            goto tryagain;
        }
        sp_debug("tried more than 3 times error.\n");
        
    }
    
    delete hs;
    return NULL;
}

// -----------------------------------------------------------------------------

int links = 0;

wstring getJSFromBranch(HTMLBranch* curBr)
{
    wstring retStr;
    wchar_t* js_link = curBr->getAttribute((wchar_t*)L"src");
    wchar_t* async_js_link = curBr->getAttribute((wchar_t*)L"async");
    
    char* js = NULL;
    
    if (js_link
        || async_js_link
        ) {
        char* jslink2;
        if (js_link) {
            jslink2 = w2m(js_link);
        }else{
            jslink2 = w2m(async_js_link);
        }
        
        if (jslink2) {
            printf("jsLink =========================== %s\n", jslink2);
            links++;
            int et, status;
            
            try {
                if (strstr(jslink2, "http://") == jslink2) {
                    js = get_js_from_cache(jslink2, et, 1, status);
                }else if (strstr(jslink2, "//") == jslink2){
                    char newURL[512];memset(newURL, 0, 512);
                    strcat(newURL, "http:");
                    strcat(newURL, jslink2);
                    printf("newURL2 ===== %s\n", newURL);
                    js = get_js_from_cache(newURL, et, 1, status);
                    
                }else{
                    char newURL[512];memset(newURL, 0, 512);
                    strcat(newURL, gHost);
                    //strcat(newURL, "http://search.jd.com");
                    strcat(newURL, "/");
                    strcat(newURL, jslink2);
                    printf("newURL ===== %s\n", newURL);
                    js = get_js_from_cache(newURL, et, 1, status);
                }
                
                if ( (js != NULL) && (strcmp(js, "") !=0) ) {
                    retStr += m2w(js);
                    delete [] js;
                }
                
                
            } catch (...) {
                printf("getting JS file error : %s\n", jslink2);
                exit(98);
            }
            
            //retStr += L"  \n\n";
            
        }
        
        //delete jslink2;
        
    }
    
    
    if (curBr->pFirstChild) {
        
        
        retStr += curBr->pFirstChild->pLeafValue;
        retStr += L"  \n\n";
        
    }

    return retStr;
}
/**
 *  load JS in HTML Dom , both src links content ,and script tag's content.
 *  all together
 *
 */
wstring loadPageJS(wstring page_js_str, vector<HTMLBranch*>& jsLinks, HTMLTree* dom_tree, Handle<Context> context)
{
    //FILE* ff = fopen("alljs.js", "w+");
    
    if (!dom_tree) {
        return page_js_str;
    }
    
    dom_tree->getBranchesByTagName((wchar_t*)L"script", jsLinks);
    
    for (int i = 0; i < jsLinks.size(); i++) {
        
        HTMLBranch* curBr = jsLinks[i];
        
        wstring one_js = getJSFromBranch(curBr);
        
        char* add_js = w2m(one_js.c_str());
        if (add_js != NULL) {
            runJSCode(context->GetIsolate(), add_js);
            
            page_js_str += one_js;
            
        }
        
    }
    
    //fclose(ff);
    printf("++++++++++++++++++++ %d js links\n", links );
    return page_js_str;
}

/**
 *  read content JS code from a file, buf size = 2048
 */
wstring read_js_content(char* jsfile)
{
    FILE* f = fopen(jsfile, "r");
    if (f == NULL) return L"";
    
    char buf[2048];memset(buf, 0 , 2048);
    fread(buf, 2048, 1, f);
    fclose(f);
    
    wchar_t* cont = m2w(buf);
    return cont;
}

Handle<ObjectTemplate> install_globals(Isolate* isolate)
{
    
    Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);
    
    v8::Handle<v8::FunctionTemplate> log_ft = v8::FunctionTemplate::New(isolate, gSpider::log_Callback);
    log_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsLog", log_ft);
    
    // save link to array for next chrome visit, only one link once
    v8::Handle<v8::FunctionTemplate> savelink_ft = v8::FunctionTemplate::New(isolate, gSpider::add_link_Callback);
    savelink_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsLink", savelink_ft);
    
    // save content to XML path as args,
    v8::Handle<v8::FunctionTemplate> savecont_ft = v8::FunctionTemplate::New(isolate, gSpider::save_Callback);
    savecont_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsSave", savecont_ft);
    
    v8::Handle<v8::FunctionTemplate> startPage_ft = v8::FunctionTemplate::New(isolate, gSpider::startLinkPage_call);
    startPage_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsStartPage", startPage_ft);
    
    v8::Handle<v8::FunctionTemplate> endPage_ft = v8::FunctionTemplate::New(isolate, gSpider::endLinkPage_call);
    endPage_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsEndPage", endPage_ft);
    
    v8::Handle<v8::FunctionTemplate> click_ft = v8::FunctionTemplate::New(isolate, gSpider::click_call);
    click_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(isolate, "gsClick", click_ft);
    
    init_AjaxWrap(isolate, global);
    init_ImageWrap(isolate, global);
    
    return global;
}

void testip()
{
    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    
    if ((he = gethostbyname("search.jd.com")) == NULL) {  // get the host info
        herror("gethostbyname");
    }
    
    // print information about this host:
    printf("Official name is: %s\n", he->h_name);
    addr_list = (struct in_addr **)he->h_addr_list;
    
    printf("    IP addresses: ");
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");

}
/**
 *   Main entry
 */
int main(int argc, const char * argv[])
{
    char path[256];
    getwd(path);   //取得当前工作目录
    printf("working dirctory is:%s\n", path);
    
    //if(mkdir("jsCache", 777) == -1) perror("mkdir error");
    
    setlocale(LC_ALL,"zh_CN.UTF-8");
//    wchar_t a[10] = L"你好";
//    wprintf(L"this is a test !\n");
//    wprintf(L"%d\n",wcslen(a));
//    wprintf(L"%ls\n",a);
//    return 0;
    
    GSTask* startTask;
    
    dns_cache = new minicfg("dns_cache.txt");
    
    dns_cache->init_dns_cache();
    
    //startTask = new GSTask("http://s.taobao.com/search?q=%E5%86%85%E5%AD%98&commend=all&ssid=s5-e&search_type=item&sourceId=tb.index&spm=1.7274553.1997520841.1&initiative_id=tbindexz_20150508", "test.js");
    
    startTask = new GSTask("http://search.jd.com/Search?keyword=%E5%86%85%E5%AD%98&enc=utf-8", "test.js");
    
    //startTask = new GSTask("http://api.leonweizhao.com/test/debug.html",  "test.js");
    
    //startTask = new GSTask("http://api.leonweizhao.com/test.php",  "test.js");
    
    txt_chrome_task(startTask);
    /*
    for (int i = 0; i < 10; i++){
        printf(" ========================= Start task %d =================\n", i);
        int ret = txt_chrome_task(startTask);
        if (ret == 1) {
            exit(88);
        }
    }
    */
    delete startTask;
    dns_cache->saveFile();
    delete dns_cache;
    
    if (eventList != NULL) {
        EVENT_OBJS* cur = eventList;
        EVENT_OBJS* ne;
        while (cur != NULL) {
            ne = cur->next;
            delete cur;
            cur = ne;
        }
    }
    return 0;
}

/**
    txt_chrome_task
    this is the txt chrome explore main task . 
    @arg    curTask: the target task object
 
    Desc:
    
    the task to below things:
 
    1,  connect to URL, read content of HTML, then parse into HTMLDocument
    2,  wrap window, document , XMLHttpRequest objects into currnet JS.
    3,  load JS that current HTML doc needed .
    4,  save data with matched from config.
    5,  save link to daemon process ( parent process ).
    6,  load customized JS
    7,  wait for ajax callback finished, then read from parent a new link URL and loop all.
 
 
    @return
    0   -   SUCCESS
    1   -   READ HTML ERROR
    2   -   JS LINK READ ERROR
    3   -   AJAX READ ERROR
    4   -   JS RUN ERROR
    5   -   LOG ERROR
    6   -   OTHER ERROR
 */

int txt_chrome_task(GSTask* curTask)
{
    // init global vars
    bzero(gHost, 64);
    bzero(gProto, 16);
    
    // init v8 env
    Isolate* isolate = Isolate::New();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    
    
    HTMLWindow* htmlWin = new HTMLWindow;
    HTMLDocument* htmlDoc = new HTMLDocument;
    htmlWin->the_doc = htmlDoc;
    htmlDoc->parentWin = htmlWin;
    
    Navigator* pNavi = new Navigator;
    // get URL content
    int et = 1;
    int status = 0;
    char* html = get_html(curTask->url_, et, 0, status);
    if (html == NULL) {
        printf("get_html error\n");
        exit(99);
    }
    FILE* dlog = fopen("debug.html", "w+");
    if (dlog == NULL) {
        printf("log file create error\n");
        return 6;
    }
    fwrite(html, strlen(html), 1, dlog); fflush(dlog); fclose(dlog);
    
    
    if (status != 200) {
        delete htmlDoc;
        delete pNavi;
        delete htmlWin;
        return 1;
    }
    
    if (et == 0){
        html = convert_gbk2utf8(html); // if hs->encodeType=gbk
    }
        
    wchar_t* ww = m2w(html);
    if (!ww) exit(66);
    
    // save into file for debug ..............
    
    // parse HTML , load more JS
    htmlDoc->dom_tree = HTMLTree::getRoot(ww);
    
    if (!htmlDoc->dom_tree) {
        
        delete [] html;
        delete htmlDoc;
        delete pNavi;
        delete htmlWin;
        
        return 2;
    }
    //HTMLBranch* bb = htmlDoc->dom_tree->getBranchByAttrname((wchar_t*)L"id", (wchar_t*)L"loginbar");
    
    //wstring rehtml = bb->getHTMLString();
    //wprintf(L"HTMLFORmat=====\n%ls\n", rehtml.c_str());
    //char* re = w2m(rehtml.c_str());
    //fwrite(re, strlen(re), 1, flog); fflush(flog); fclose(flog);
    //delete [] html;
    
    /**
     * appending JS in page, either link or js content
     */
    
    // global definition
    
    
    
    // ajax object
    
    Handle<ObjectTemplate> global = install_globals(isolate);
    // ---------------------------------
    // global objects
    // =========================================================================
    
    Handle<Context> context = Context::New(isolate, NULL, global);
    Context::Scope context_scope(context);
    //context->Enter();
    
    
    v8::Handle<v8::Object> v8RealGlobal = v8::Handle<v8::Object>::Cast(context->Global()->GetPrototype());
    Local<Object> jsWin = WrapHTMLWindowObject(isolate, htmlWin);
    v8RealGlobal->SetPrototype(jsWin); // set property enabled
    
    context->Global()->Set(String::NewFromUtf8(isolate, "window"), jsWin);
    
    Local<Object> naviObj = NavigatorWrap(isolate, pNavi);
    context->Global()->Set(String::NewFromUtf8(isolate, "navigator"), naviObj);
    
    //Handle<Object> html_obj = init_DomWrap(isolate, htmlDoc);
    htmlWin->doc_Handle = init_DomWrap(isolate, htmlDoc);
    htmlWin->console_Handle =  consoleWrap(isolate, &(htmlWin->_console));
    
    context->Global()->Set(String::NewFromUtf8(isolate, "document"), htmlWin->doc_Handle);
    context->Global()->Set(String::NewFromUtf8(isolate, "console"), htmlWin->console_Handle);
    
    // =========================================================================
    sp_debug("+++++++++++++++++++ Parse HTML OK and start run js on DOM ..............\n");
    wstring  page_js_str;
    vector<HTMLBranch*> jsLinks;
    loadPageJS(page_js_str, jsLinks, htmlDoc->dom_tree, context);
    
    //char* page_js = w2m(page_js_str.c_str());
    //createdJSNodes.clear();
    
    sp_debug("**************** page js finished (%ld) Script >>>>>>>>>\n", jsLinks.size());
    //runJSCode(context, page_js);
    //delete page_js;
    
    
    // run created JS code
    if (createdJSNodes.size()>0) {
        sp_debug("++++++++++++++ running created code ++++++++++++++++++++++++++\n");
        
        int i = 0;
        for(createdNode = createdJSNodes.begin(); createdNode != createdJSNodes.end(); createdNode++)
        {
            //sp_debug("[%s] attribute value removed\n", attrs[i].attr_value);
            wstring created_js = getJSFromBranch(createdJSNodes[i]);
            char* page_js2 = w2m(created_js.c_str());
            runJSCode(context->GetIsolate(), page_js2);
            delete page_js2;
            i++;
        }
        
    }
    
    
    // run js from user setted file !!!!
    sp_debug("*************************************  custom js start *******************************\n");
    wstring custom_js = read_js_content(curTask->jsPath_);
    char* add_js = w2m(custom_js.c_str());
    runJSCode(context->GetIsolate(), add_js);
    delete add_js;
    sp_debug("*************************************  custom js finished *****************************\n");
    // decode js and compile run with exception support
    
    
    //context->Exit();
    
    //V8::Dispose();
    // functions
    
//    Persistent<Function> gs_save;
//    Handle<String> fc_save_name = String::NewFromUtf8(isolate, "onreadystatechange");
//    Handle<Value> save_val = context->Global()->Get(fc_save_name);
//    
//    // If there is no Process function, or if it is not a function,
//    // bail out
//    if (save_val->IsFunction()){
//        
//        // It is a function; cast it to a Function
//        Handle<Function> process_fun = Handle<Function>::Cast(save_val);
//        gs_save.Reset(isolate, process_fun);
//        
//        const int argc = 0;
//        Handle<Value> argv[argc] = { };
//        v8::Local<v8::Function> process = v8::Local<v8::Function>::New(isolate, gs_save);
//        Handle<Value> result = process->Call(context->Global(), argc , argv);
//        if (result.IsEmpty()) {
//            
//        }
//    }
    
    
    /**
     *      release objects
     */
    
    delete htmlDoc;
    delete pNavi;
    delete htmlWin;
    
    return 0;
    
}
/**
 * end func
 */

