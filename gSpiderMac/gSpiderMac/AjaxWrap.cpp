//
//  AjaxWrap.cpp
//  gSpiderMac
//
//  Created by reichtiger on 10/6/14.
//  Copyright (c) 2014 reichtiger. All rights reserved.
//

//#include "Stdafx.h"

#include "AjaxWrap.h"

extern char    gHost[32];
extern char*   gPath;
extern int     gPort;
extern minicfg* dns_cache;

namespace gSpider {
    
    Persistent<Function> gs_save;
    
    void read_some(void* sender, char* str, size_t n)
    {
        wprintf(L"reading bytes = %ld bytes. %s\n", n, str);
    }
    
    
    /*******************************************************************************
     * XMLHttpRequest definition
     *
     ******************************************************************************/
    
    XMLHttpRequest::XMLHttpRequest(Local<Object> _jsObject, char* host, int port)
    {
        _host = host;
        _port = port;
        response_txt = NULL;
        status = -1;
        readyState = -1;
        callback_setted = false;
    }
    
    XMLHttpRequest::~XMLHttpRequest()
    {
        printf("dealloc XMLHttpRequest\n");
        hs.cleanALL();
        
    }
    
    void XMLHttpRequest::open(char* method, char* pagePath, bool flag)
    {
        size_t url_len = strlen(pagePath) + 32;
        char* url = new char[url_len];
        memset(url, 0, url_len);
        strncpy(url, _host, strlen(_host));
        strncpy(url+strlen(_host), "/", 1);
        strncpy(url+strlen(_host)+1, pagePath, strlen(pagePath));
        
        hs.parseURL(url);
        
        if (strcmp(method, "POST") == 0) {
            hs.isPost = 1;
        }
        
        char* cached_ip = dns_cache->getValue(hs.host);
        if (cached_ip) {
            hs.init_ip_sock(cached_ip);
        }else{
            // update dns cache , if exist , use it
            char* ipaddr = hs.init_sock(hs.host, hs.port);
            if (ipaddr == NULL)
            {
                return;
            }
            
            dns_cache->saveValue(hs.host, ipaddr);
        }
        
        //sock.ptrReadAll = read_all_finished;
        //sock.ptrReadSome = read_some;
        if (hs.connect_wait() <0){
            printf("connect failed\n");
            return;
        }
        delete [] url;
        
    }
    
    char* XMLHttpRequest::send(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (hs.isPost) {
            String::Utf8Value str(args[0]);
            printf("AJAX POST =%s\n", *str);
            hs.do_post(*str);
        }else{
            hs.do_get();
        }
        
        char* ret = NULL;
        
        if (hs.RecvBytes()>0)
        {
            hs.splitHTMLHeader();
            set_status(200);
            
            //printf("html head=%s\n", sock.html);
            printf("html body len = %d, %s\n", hs.html_body_len, hs.encodeType);
            response_txt = new char[hs.html_body_len+1];
            if (response_txt) {
                strncpy(response_txt, hs.html_body, hs.html_body_len);
                response_txt[hs.html_body_len] = '\0';
                set_readyState(4);
                //char* utf8_html = convert_gbk2utf8(response_txt); // if hs->encodeType=gbk
                printf("utf8 html (len=%ld)= %s\n", strlen(response_txt), response_txt);
                //for (int i = 0; i < 5; i++) sleep(1);// delay few seconds
                if (strlen(response_txt) <= 0 ) return NULL;
                
                if (callback_setted){
                    ready_callback(args);
                }
                return response_txt;
            }
        }
        ready_callback(args);
        return ret;
    }
    
    void XMLHttpRequest::ready_callback(const FunctionCallbackInfo<v8::Value>& args)
    {
        
        Local<Value> save_val = Local<Value>::New(args.GetIsolate(), _onreadystatechange);
        
        // If there is no Process function, or if it is not a function,
        // bail out
        if (save_val->IsFunction()){
            printf("AJAX callback_calling ============\n");
            
            Handle<Function> process_fun = Handle<Function>::Cast(save_val);
            //ori_ptr->_onreadystatechange.Reset(info.GetIsolate(), process_fun);
            gs_save.Reset(args.GetIsolate(), process_fun);
            // It is a function; cast it to a Function
            v8::Local<v8::Function> process = v8::Local<v8::Function>::New(args.GetIsolate(), gs_save);
            Handle<Value> result = process->Call(args.Holder(), 0 , NULL);
            if (result.IsEmpty()) {
                
            }
        }
    }
    int XMLHttpRequest::get_status()
    {
        return status;
    }
    
    void XMLHttpRequest::set_status(int status_)
    {
        status = status_;
        
        //Handle<Function> callback(Function::Cast(*onreadystatechange));
        //callback->Call(NULL, 0, NULL);
    }
    
    int XMLHttpRequest::get_readyState()
    {
        return readyState;
    }
    
    void XMLHttpRequest::set_readyState(int state)
    {
        readyState = state;
    }
    /*******************************************************************************
     * AjaxWrap definition
     *
     ******************************************************************************/
    
    XMLHttpRequest* getXMLHttpReqPtr(Local<Object> self)
    {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        return static_cast<XMLHttpRequest*>(ptr);
    }
    
    void init_AjaxWrap(Isolate* isolate, Handle<ObjectTemplate> global)
    {
        // XMLHttpRequest wrap
        
        Handle<FunctionTemplate> ajax_t = FunctionTemplate::New(isolate, AjaxWrap_Constructor);
        ajax_t->SetClassName(String::NewFromUtf8(isolate, "XMLHttpRequest"));
        global->Set(String::NewFromUtf8(isolate, "XMLHttpRequest"), ajax_t);
        
    }
    
    void AjaxWrap_Constructor(const FunctionCallbackInfo<Value>& args)
    {
        // cause in JS no host and port to init socket. so use global to here.
        HandleScope scope(args.GetIsolate());
        
        args.GetReturnValue().Set(get_ajax_obj(args));
        
        // ======================================================
    }
    /**
     * wrap Ajax constructor
     * this function will return wrapped object to JS
     */
    
    Local<Object> get_ajax_obj(const FunctionCallbackInfo<Value>& args)
    {
        XMLHttpRequest* req = new XMLHttpRequest(args.This(), gHost, gPort);
        
        Isolate* iso = Isolate::GetCurrent();
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // functions
        templ->Set(iso, "open", FunctionTemplate::New(iso, AjaxWrap_open));
        templ->Set(iso, "send", FunctionTemplate::New(iso, AjaxWrap_send));
        templ->Set(iso, "setRequestHeader", FunctionTemplate::New(iso, AjaxWrap_setRequestHeader));
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(iso, "status"), AjaxWrap_status);
        templ->SetAccessor(String::NewFromUtf8(iso, "readyState"), AjaxWrap_readyState);
        templ->SetAccessor(String::NewFromUtf8(iso, "responseText"), AjaxWrap_responseText);
        templ->SetAccessor(String::NewFromUtf8(iso, "onreadystatechange"), AjaxWrap_onReadyStateChange, AjaxWrap_onReadyStateChangeSetter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(Isolate::GetCurrent(), req));
        
        args.GetReturnValue().Set(result);
        return result;
    }
    
    void AjaxWrap_open(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(args.Holder());
        String::Utf8Value method(args[0]);
        String::Utf8Value page_path(args[1]);
        String::Utf8Value asyncFlag(args[2]);
        
        bool async_flag = args[2]->ToBoolean()->BooleanValue();
        
        
        //printf("XMLREQ OPEN = %2x, %s\n", ori_ptr, ori_ptr->getHost());
        
        
        printf("AJAX connecting host by【%s】...%s/%s on flag %s\n",
               *method,
               ori_ptr->_host,
               *page_path,
               *asyncFlag);
        
        ori_ptr->open(*method, *page_path, async_flag);
        
    }
    
    void AjaxWrap_setRequestHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        
    }
    
    void AjaxWrap_send(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(args.Holder());
        ori_ptr->send(args);
        
    }
    
    
    void AjaxWrap_status(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(info.Holder());
        
        int s = ori_ptr->get_status();
        printf("status=%d\n", s);
        info.GetReturnValue().Set(Integer::New(Isolate::GetCurrent(), s));
    }
    
    void AjaxWrap_readyState(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(info.Holder());
        
        int s = ori_ptr->get_readyState();
        printf("readyState=%d\n", s);
        info.GetReturnValue().Set(s);
    }
    
    void AjaxWrap_responseText(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(info.Holder());
        
        char* retStr = ori_ptr->response_txt;
        if (retStr){
            info.GetReturnValue().Set(String::NewFromUtf8(
                                                      info.GetIsolate(), retStr, String::kNormalString,
                                                      static_cast<int>(strlen(retStr))));
    
        }
    }
    
    void AjaxWrap_onReadyStateChange(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(info.Holder());
        info.GetReturnValue().Set(ori_ptr->_onreadystatechange);
    }
    
    void AjaxWrap_onReadyStateChangeSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        String::Utf8Value str(name);
        
        XMLHttpRequest* ori_ptr = getXMLHttpReqPtr(info.Holder());
        
        ori_ptr->_onreadystatechange.Reset(info.GetIsolate(), value_obj);
        
        printf("ajax onreadystatechange setted ++++++++++++++++++++++ \n");
        
        ori_ptr->callback_setted = true;
    }
    
}// end namespace gSpider

