//
//  NavigatorWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#include "NavigatorWrap.h"

namespace gSpider {
    /**
     *
     *  Navigator object wrap
     */
    Handle<Object> NavigatorWrap(Isolate* isolate, Navigator* navi) {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "appCodeName"), NaviWrap_appCodeName);
        templ->SetAccessor(String::NewFromUtf8(isolate, "appName"), NaviWrap_appName);
        templ->SetAccessor(String::NewFromUtf8(isolate, "appVersion"), NaviWrap_appVersion);
        templ->SetAccessor(String::NewFromUtf8(isolate, "browserLanguage"), NaviWrap_browserLanguage);
        templ->SetAccessor(String::NewFromUtf8(isolate, "cookieEnabled"), NaviWrap_cookieEnabled);
        templ->SetAccessor(String::NewFromUtf8(isolate, "cpuClass"), NaviWrap_cpuClass);
        templ->SetAccessor(String::NewFromUtf8(isolate, "onLine"), NaviWrap_onLine);
        templ->SetAccessor(String::NewFromUtf8(isolate, "platform"), NaviWrap_platform);
        templ->SetAccessor(String::NewFromUtf8(isolate, "systemLanguage"), NaviWrap_systemLanguage);
        templ->SetAccessor(String::NewFromUtf8(isolate, "userAgent"), NaviWrap_userAgent);
        templ->SetAccessor(String::NewFromUtf8(isolate, "userLanguage"), NaviWrap_userLanguage);
        
        // functions
        templ->Set(isolate, "javaEnabled", FunctionTemplate::New(isolate, NaviWrap_javaEnabled));
        templ->Set(isolate, "taintEnabled", FunctionTemplate::New(isolate, NaviWrap_taintEnabled));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, navi));
        
        return handle_scope.Escape(result);
    }
    
    /**
     *      properties and functions body
     */
    
    void NaviWrap_appCodeName(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_appMinorVersion(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_appName(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_appVersion(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_browserLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_cookieEnabled(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_cpuClass(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_onLine(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_platform(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_systemLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void NaviWrap_userAgent(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){
        char* agentStr = (char*)"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)";
        info.GetReturnValue().Set(String::NewFromUtf8(
                                                      info.GetIsolate(), agentStr, String::kNormalString,
                                                      static_cast<int>(strlen(agentStr))));
    }
    void NaviWrap_userLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    
    /***************************************************************************
     *  functions
     */
    
    void NaviWrap_javaEnabled(const v8::FunctionCallbackInfo<v8::Value>& args){}
    void NaviWrap_taintEnabled(const v8::FunctionCallbackInfo<v8::Value>& args){}
    
}