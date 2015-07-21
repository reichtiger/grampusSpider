//
//  testWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/10/21.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

//#include "Stdafx.h"

#include "testWrap.h"

namespace gSpider {
    void MyWindow_initialize(Isolate* isolate, Handle<ObjectTemplate> global)
    {
        // XMLHttpRequest wrap
        Handle<FunctionTemplate> dom_t = FunctionTemplate::New(isolate, MyWindow_Constructor);
        dom_t->SetClassName(String::NewFromUtf8(isolate, "MyWindow"));
        global->Set(String::NewFromUtf8(isolate, "MyWindow"), dom_t);
        
        Handle<ObjectTemplate> dom_proto = dom_t->PrototypeTemplate();
        
        
        Handle<ObjectTemplate> inst = dom_t->InstanceTemplate();
        inst->SetInternalFieldCount(1);
        
    }
    
    /**
     * wrap document constructor
     * this function will return wrapped object to JS
     */
    void MyWindow_Constructor(const FunctionCallbackInfo<Value>& args)
    {
        Handle<Object> object = args.This();
        HandleScope handle_scope(Isolate::GetCurrent());
        
        // cause in JS no host and port to init socket. so use global to here.
        MyWindow *mywin = new MyWindow();
        
        object->SetInternalField(0, External::New(Isolate::GetCurrent(), mywin));
        
        args.GetReturnValue().Set(object);
    }
}

