//
//  elementWrap.h
//  gSpiderMac
//
//  Created by reich on 14/11/5.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__elementWrap__
#define __gSpiderMac__elementWrap__

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <assert.h>
#include <v8.h>
#include "WZ_HTTPSocket.h"
#include "HTMLTree.h"
#include "util.h"
#include "cssstyleWrap.h"

using namespace::v8;
using namespace::std;


enum EVENT_TYPES{
    EVENT_TYPE_CLICK,
    EVENT_TYPE_MOUSEMOVE,
    EVENT_TYPE_ONLOAD
};

struct EVENT_OBJS {
    HTMLBranch* p;
    int type; // mouse click , mouseover and mousemove
    Persistent<Value> fv;
    EVENT_OBJS* next;
};


std::string executeString(v8::Isolate* isolate, v8::Handle<v8::String> source,
                          v8::Handle<v8::Value> name,
                          bool print_result,
                          bool report_exceptions);
void runJSCode(v8::Isolate* isolate, char* jscode);
void ReportException(Isolate* isolate, v8::TryCatch* try_catch);


namespace gSpider {
    
    /**
     *  properties 
     *  ref: http://www.w3school.com.cn/jsref/dom_obj_all.asp
     *
     */
    void elementWrap_innerHTML(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_innerHTML_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    void elementWrap_firstChild(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_lastChild(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    // type
    void elementWrap_type(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_type_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    // src
    void elementWrap_src(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_src_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    // async
    void elementWrap_async(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_async_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    // href
    void elementWrap_href(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_href_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    // id
    void elementWrap_id(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_id_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    // class
    void elementWrap_class(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_class_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    void elementWrap_nodeType(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    void elementWrap_parentNode(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    //void elementWrap_parentNode_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    void elementWrap_offsetTop(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    // value and defaultValue is almost the same
    void elementWrap_value(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_value_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    // onsubmit
    void elementWrap_onsubmit(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_onsubmit_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    void elementWrap_attributes(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    // click
    void elementWrap_click(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void elementWrap_click_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    
    Handle<Object> element_Wrap(Isolate* isolate, HTMLBranch* brch);
    Handle<Object> textNode_Wrap(Isolate* isolate, HTMLBranch* leaf_new);
    Handle<Object> attribute_Wrap(Isolate* isolate, ATTRIBUTE* attr);
    
    // attribute wrap
    void attributeWrap_value(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void attributeWrap_value_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    
    /**
     *  Functions , methods !!!!!!!!!!!!!!
     *
     */
    void ElementWrap_appendChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_removeChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_click(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_cloneNode(const v8::FunctionCallbackInfo<v8::Value>& args);
    void ElementWrap_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    /**
     *      Image objects
     */
    Local<Object> create_Image_obj(const FunctionCallbackInfo<Value>& args);
    void init_ImageWrap(Isolate* isolate, Handle<ObjectTemplate> global);
    void Image_Constructor(const FunctionCallbackInfo<Value>& args);
}

#endif /* defined(__gSpiderMac__elementWrap__) */
