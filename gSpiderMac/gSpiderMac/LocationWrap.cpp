//
//  LocationWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#include "LocationWrap.h"

extern char    gProto[16];
extern char    gHost[32];
extern char*   gPath;
extern int     gPort;

namespace gSpider {
 
    Location::Location()
    {
        strcat(href_, "http://test.com/");
    }
    
    Location::~Location()
    {
        
    }
    
    char* Location::get_href()
    {
        return href_;
    }
    
    void Location::set_href(char* url)
    {
        if (href_) {
            memset(href_, 0, strlen(href_));
            strncpy(href_, url, strlen(url));
        }
    }
    
    /**
     *
     *  Location object wrap
     */
    Handle<Object> LocationWrap(Isolate* isolate, Location* loc) {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "hash"), LocationWrap_hash);
        templ->SetAccessor(String::NewFromUtf8(isolate, "host"), LocationWrap_host);
        templ->SetAccessor(String::NewFromUtf8(isolate, "hostname"), LocationWrap_hostname);
        templ->SetAccessor(String::NewFromUtf8(isolate, "href"), LocationWrap_href);
        templ->SetAccessor(String::NewFromUtf8(isolate, "pathname"), LocationWrap_pathname);
        templ->SetAccessor(String::NewFromUtf8(isolate, "port"), LocationWrap_port);
        templ->SetAccessor(String::NewFromUtf8(isolate, "protocol"), LocationWrap_protocol);
        templ->SetAccessor(String::NewFromUtf8(isolate, "search"), LocationWrap_search);
        
        // functions
        templ->Set(isolate, "assign", FunctionTemplate::New(isolate, LocationWrap_assign));
        templ->Set(isolate, "reload", FunctionTemplate::New(isolate, LocationWrap_reload));
        templ->Set(isolate, "replace", FunctionTemplate::New(isolate, LocationWrap_replace));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, loc ));
        
        return handle_scope.Escape(result);
    }
    
    /**
     *      properties and functions body
     
     hash           设置或返回从井号 (#) 开始的 URL（锚）。
     host           设置或返回主机名和当前 URL 的端口号。
     hostname       设置或返回当前 URL 的主机名。
     href           设置或返回完整的 URL。
     pathname       设置或返回当前 URL 的路径部分。
     port           设置或返回当前 URL 的端口号。
     protocol       设置或返回当前 URL 的协议。
     search         设置或返回从问号 (?) 开始的 URL（查询部分）。
     
     */
    
    Location* getOriLocationPtr(Local<Object> self)
    {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        return static_cast<Location*>(ptr);
    }
    
    
    void LocationWrap_hash(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), ""));
    }
    void LocationWrap_host(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void LocationWrap_hostname(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void LocationWrap_href(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){
        
        Location* pLoc = getOriLocationPtr(info.Holder());
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), pLoc->href_));
    }
    void LocationWrap_pathname(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void LocationWrap_port(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void LocationWrap_protocol(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        if (strstr(gHost, "https://")) {
            info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "https:"));
        }else{
            info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "http:"));
        }
    }
    void LocationWrap_search(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    
    /***************************************************************************
     *  functions
     */
    
    void LocationWrap_assign(const v8::FunctionCallbackInfo<v8::Value>& args){}
    void LocationWrap_reload(const v8::FunctionCallbackInfo<v8::Value>& args){}
    void LocationWrap_replace(const v8::FunctionCallbackInfo<v8::Value>& args){}
    
}