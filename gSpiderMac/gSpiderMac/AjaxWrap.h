//
//  AjaxWrap.h
//  gSpiderMac
//
//  Created by reichtiger on 10/6/14.
//  Copyright (c) 2014 reichtiger. All rights reserved.
//

#ifndef     __AJAXWRAP_HEADER__
#define     __AJAXWRAP_HEADER__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <v8.h>
#include "WZ_HTTPSocket.h"
#include "util.h"
#include "minicfg.h"


using namespace v8;

namespace gSpider {
    /**
     *  XMLHttpRequest declare
     */

    class XMLHttpRequest{
    public:
        char*   _host;
        int     _port;
        char*   _curPath;
        WZ_HTTPSocket   hs;
        int     status;
        int     readyState;
        char*   response_txt;
        Persistent<Value> _onreadystatechange;
        //Persistent<Object> jsObject;
        bool    callback_setted;
        
    public:
        XMLHttpRequest(Local<Object> _jsObject, char* host, int port);
        ~XMLHttpRequest();
        void open(char* method,
                  char* pagePath,
                  bool flag);
        void setRequestHeader();
        char* send(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void read_all_finished(void* sender, char* str, size_t n);
        char* getHost(){
            if (_host) {
                return _host;
            }
            return NULL;
        }
        
        int     get_status();
        void    set_status(int status_);
        int     get_readyState();
        void    set_readyState(int state);
        
        void    ready_callback(const FunctionCallbackInfo<v8::Value>& args);
    };
    
    
    /**
     *  Ajax Wrap
     *
     *
     */
    Local<Object> get_ajax_obj(const FunctionCallbackInfo<Value>& args);
    void init_AjaxWrap(Isolate* isolate, Handle<ObjectTemplate> global);
    void AjaxWrap_Constructor(const FunctionCallbackInfo<Value>& args);

    void AjaxWrap_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    void AjaxWrap_setRequestHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    void AjaxWrap_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    /**
     *  properties
     */
    void AjaxWrap_status(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void AjaxWrap_readyState(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void AjaxWrap_onReadyStateChange(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void AjaxWrap_responseText(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void AjaxWrap_onReadyStateChangeSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
}

#endif