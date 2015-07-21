//
//  ScreenWrap.h
//  gSpiderMac
//
//  Created by reich on 14/11/20.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__ScreenWrap__
#define __gSpiderMac__ScreenWrap__

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
    class Screen{
    public:
        Screen(){};
        ~Screen(){};
        
    public:
        
    };
    
    Handle<Object> ScreenWrap(Isolate* isolate, Screen* scr);
    
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
    void ScreenWrap_availHeight(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_availWidth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_bufferDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_colorDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_deviceXDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_deviceYDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_fontSmoothingEnabled(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_height(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_logicalXDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_logicalYDPI(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_pixelDepth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_updateInterval(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void ScreenWrap_width(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    /***************************************************************************
     *  functions
     */
    
    void LocationWrap_assign(const v8::FunctionCallbackInfo<v8::Value>& args);
    void LocationWrap_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    void LocationWrap_replace(const v8::FunctionCallbackInfo<v8::Value>& args);

}
#endif /* defined(__gSpiderMac__ScreenWrap__) */
