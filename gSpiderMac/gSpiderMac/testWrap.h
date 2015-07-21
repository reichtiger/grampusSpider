//
//  testWrap.h
//  gSpiderMac
//
//  Created by reich on 14/10/21.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__testWrap__
#define __gSpiderMac__testWrap__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <v8.h>
#include "HTMLTree.h"
#include "util.h"
#include "DomWrap.h"

using namespace v8;

namespace gSpider {
    class MyWindow{
        
    };
    void MyWindow_initialize(Isolate* isolate, Handle<ObjectTemplate> global);
    void MyWindow_Constructor(const FunctionCallbackInfo<Value>& args);
}
#endif /* defined(__gSpiderMac__testWrap__) */
