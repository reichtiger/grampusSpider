//
//  cssstyleWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/12/19.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#include "cssstyleWrap.h"

namespace gSpider {
    
    Handle<Object> cssstyle_Wrap(Isolate* isolate, cssstyle* pStyle)
    {
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "color"), cssstyle_color, cssstyle_color_setter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, pStyle));
        
        return handle_scope.Escape(result);
        
    }
    
    void cssstyle_color(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void cssstyle_color_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        
    }
    
}