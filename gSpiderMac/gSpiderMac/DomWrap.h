//
//  DomWrap.h
//  gSpiderMac
//
//  Created by reich on 14/11/1.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__DomWrap__
#define __gSpiderMac__DomWrap__

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
#include "elementWrap.h"
#include "LocationWrap.h"

using namespace::v8;

namespace gSpider{
    
    // C++ object HTMLDocument <--> DomWrap
    class HTMLWindow;
    
    class HTMLDocument{
    public:
        HTMLDocument();
        ~HTMLDocument();
        char*   _innerHTML;
        
        char*   getHTML();
        
        HTMLBranch* dom_tree;
        HTMLWindow* parentWin;
        
        void removeTree();
        void setTree(wchar_t* inStr);
    };
    
    
    // warpping HTMLDocument to "document"
    
    Handle<Object> init_DomWrap(Isolate* isolate, HTMLDocument* html_doc);
    
    /***************************************************************************
     *  properties
     */
    void DomWrap_innerHTML(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_innerHTML_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    // cookie
    void DomWrap_cookie(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_cookie_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    void DomWrap_Location(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_body(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_defaultView(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_nodeType(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_documentElement(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void DomWrap_readyState(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    /***************************************************************************
     *  functions
     */
    void DomWrap_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_getElementsByClassName(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_createElement(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_createTextNode(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    void DomWrap_createDocumentFragment(const v8::FunctionCallbackInfo<v8::Value>& args);
}

#endif /* defined(__gSpiderMac__DomWrap__) */
