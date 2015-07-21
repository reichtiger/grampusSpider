//
//  NavigatorWrap.h
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__NavigatorWrap__
#define __gSpiderMac__NavigatorWrap__

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <v8.h>
#include "WZ_HTTPSocket.h"
#include "HTMLTree.h"
#include "util.h"

using namespace::v8;
using namespace::std;

namespace gSpider {
    
    class Navigator{
    public:
        Navigator(){};
        ~Navigator(){};
    };
    
    Handle<Object> NavigatorWrap(Isolate* isolate, Navigator* navi);
    
    /***************************************************************************
     *  properties
     
     appCodeName        返回浏览器的代码名。
     appMinorVersion	返回浏览器的次级版本。
     appName            返回浏览器的名称。
     appVersion         返回浏览器的平台和版本信息。
     browserLanguage	返回当前浏览器的语言。
     cookieEnabled      返回指明浏览器中是否启用 cookie 的布尔值。
     cpuClass           返回浏览器系统的 CPU 等级。
     onLine             返回指明系统是否处于脱机模式的布尔值。
     platform           返回运行浏览器的操作系统平台。
     systemLanguage     返回 OS 使用的默认语言。
     userAgent          返回由客户机发送服务器的 user-agent 头部的值。
     userLanguage       返回 OS 的自然语言设置。
     
     */
    void NaviWrap_appCodeName(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_appMinorVersion(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_appName(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_appVersion(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_browserLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_cookieEnabled(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_cpuClass(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_onLine(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_platform(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_systemLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_userAgent(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void NaviWrap_userLanguage(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    /***************************************************************************
     *  functions
     */
    
    void NaviWrap_javaEnabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    void NaviWrap_taintEnabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    
}
#endif /* defined(__gSpiderMac__NavigatorWrap__) */
