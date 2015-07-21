//
//  LocationWrap.h
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__LocationWrap__
#define __gSpiderMac__LocationWrap__

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
    
    class Location{
    public:
        Location();
        ~Location();
        
    public:
        char    href_[128];
        
        char* get_href();
        void set_href(char* url);
    };
    
    Handle<Object> LocationWrap(Isolate* isolate, Location* loc);
    
    /***************************************************************************
     *  properties
     
     hash           设置或返回从井号 (#) 开始的 URL（锚）。
     host           设置或返回主机名和当前 URL 的端口号。
     hostname       设置或返回当前 URL 的主机名。
     href           设置或返回完整的 URL。
     pathname       设置或返回当前 URL 的路径部分。
     port           设置或返回当前 URL 的端口号。
     protocol       设置或返回当前 URL 的协议。
     search         设置或返回从问号 (?) 开始的 URL（查询部分）。
     
     */
    void LocationWrap_hash(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_host(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_hostname(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_href(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_pathname(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_port(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_protocol(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void LocationWrap_search(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    /***************************************************************************
     *  functions
     
     assign()	加载新的文档。
     reload()	重新加载当前文档。
     replace()	用新的文档替换当前文档。
     
     */
    
    void LocationWrap_assign(const v8::FunctionCallbackInfo<v8::Value>& args);
    void LocationWrap_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    void LocationWrap_replace(const v8::FunctionCallbackInfo<v8::Value>& args);
    
}

#endif /* defined(__gSpiderMac__LocationWrap__) */
