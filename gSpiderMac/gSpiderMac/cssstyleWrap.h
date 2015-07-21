//
//  cssstyleWrap.h
//  gSpiderMac
//
//  Created by reich on 14/12/19.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__cssstyleWrap__
#define __gSpiderMac__cssstyleWrap__

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

using namespace::v8;
using namespace::std;

namespace gSpider {
    class cssstyle{
        
    };
    
    void cssstyle_color(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void cssstyle_color_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    Handle<Object> cssstyle_Wrap(Isolate* isolate, cssstyle* pStyle);
    
}

#endif /* defined(__gSpiderMac__cssstyleWrap__) */
