//
//  ScreenWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#include "ScreenWrap.h"

namespace gSpider {
    /**
     *
     *  Location object wrap
     */
    Handle<Object> ScreenWrap(Isolate* isolate, Screen* scr) {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        /***************************************************************************
         *  properties
         availHeight	返回显示屏幕的高度 (除 Windows 任务栏之外)。
         availWidth	返回显示屏幕的宽度 (除 Windows 任务栏之外)。
         bufferDepth	设置或返回调色板的比特深度。
         colorDepth	返回目标设备或缓冲器上的调色板的比特深度。
         deviceXDPI	返回显示屏幕的每英寸水平点数。
         deviceYDPI	返回显示屏幕的每英寸垂直点数。
         fontSmoothingEnabled	返回用户是否在显示控制面板中启用了字体平滑。
         height	返回显示屏幕的高度。
         logicalXDPI	返回显示屏幕每英寸的水平方向的常规点数。
         logicalYDPI	返回显示屏幕每英寸的垂直方向的常规点数。
         pixelDepth	返回显示屏幕的颜色分辨率（比特每像素）。
         updateInterval	设置或返回屏幕的刷新率。
         width
         */
        templ->SetAccessor(String::NewFromUtf8(isolate, "availHeight"), ScreenWrap_availHeight);
        templ->SetAccessor(String::NewFromUtf8(isolate, "availWidth"), ScreenWrap_availWidth);
        templ->SetAccessor(String::NewFromUtf8(isolate, "bufferDepth"), ScreenWrap_bufferDepth);
        templ->SetAccessor(String::NewFromUtf8(isolate, "colorDepth"), ScreenWrap_colorDepth);
        templ->SetAccessor(String::NewFromUtf8(isolate, "deviceXDPI"), ScreenWrap_deviceXDPI);
        templ->SetAccessor(String::NewFromUtf8(isolate, "deviceYDPI"), ScreenWrap_deviceYDPI);
        templ->SetAccessor(String::NewFromUtf8(isolate, "fontSmoothingEnabled"), ScreenWrap_fontSmoothingEnabled);
        templ->SetAccessor(String::NewFromUtf8(isolate, "height"), ScreenWrap_height);
        templ->SetAccessor(String::NewFromUtf8(isolate, "logicalXDPI"), ScreenWrap_logicalXDPI);
        templ->SetAccessor(String::NewFromUtf8(isolate, "logicalYDPI"), ScreenWrap_logicalYDPI);
        templ->SetAccessor(String::NewFromUtf8(isolate, "pixelDepth"), ScreenWrap_pixelDepth);
        templ->SetAccessor(String::NewFromUtf8(isolate, "updateInterval"), ScreenWrap_updateInterval);
        templ->SetAccessor(String::NewFromUtf8(isolate, "width"), ScreenWrap_width);
        
        // functions
        templ->Set(isolate, "assign", FunctionTemplate::New(isolate, LocationWrap_assign));
        templ->Set(isolate, "reload", FunctionTemplate::New(isolate, LocationWrap_reload));
        templ->Set(isolate, "replace", FunctionTemplate::New(isolate, LocationWrap_replace));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, scr ));
        
        return handle_scope.Escape(result);
    }
    
    /**
     *      properties and functions body
     */
    
    Screen* getOriScreenPtr(Local<Object> self)
    {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        return static_cast<Screen*>(ptr);
    }
    
    void ScreenWrap_availHeight(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_availWidth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_bufferDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_colorDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_deviceXDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_deviceYDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_fontSmoothingEnabled(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_height(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_logicalXDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_logicalYDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_pixelDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_updateInterval(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void ScreenWrap_width(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    
}