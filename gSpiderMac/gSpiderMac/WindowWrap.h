//
//  WindowWrap.h
//  gSpiderMac
//
//  Created by reichtiger on 10/6/14.
//  Copyright (c) 2014 zhaohu. All rights reserved.
//

#ifndef     __WINDOWWRAP_HEADER__
#define     __WINDOWWRAP_HEADER__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <v8.h>
#include "WZ_HTTPSocket.h"
#include "HTMLTree.h"
#include "util.h"
#include "AjaxWrap.h"
#include "DomWrap.h"
#include "LocationWrap.h"
#include "ScreenWrap.h"


using namespace::v8;

struct WINDOW_EVENTS{
    HTMLBranch* p;
    int type; // mouse click , mouseover and mousemove
    Persistent<Value> fv;
    EVENT_OBJS* next;
};

namespace gSpider {
    
    class Console{
        
    };
    /**
     *  window object declare
     */
    
    class HTMLWindow{
    public:
        char*   _host;
        int     _port;
        char*   _curPath;
        WZ_HTTPSocket   sock;
        Persistent<Value> _onmousewheel;
        bool    mousecallback_setted;
        
        Handle<Object> doc_Handle;
        HTMLDocument* the_doc;
        // console
        Handle<Object> console_Handle;
        Console     _console;
        
    public:
        HTMLWindow();
        virtual ~HTMLWindow(){
            printf("~~~~~~~~~~ dealloc  Window \n");
        };
        void open();
        
        void    mousewheel_callback(const FunctionCallbackInfo<v8::Value>& args);
        void    onload(const FunctionCallbackInfo<v8::Value>& args);
    };
    
    
    /**
     *  console wrap
     *
     */
    Handle<Object> consoleWrap(Isolate* isolate, Console* pConsole);
    void consoleWrap_log(const FunctionCallbackInfo<Value>& args);
    
    
    /**
     *  window object Wrap
     *
     *
     */
    Handle<Object> WrapHTMLWindowObject(Isolate* isolate, HTMLWindow* htmlWin);
    
    /**
     *  properties wrap
     
     closed             返回窗口是否已被关闭。
     defaultStatus      设置或返回窗口状态栏中的默认文本。
     document           对 Document 对象的只读引用。请参阅 Document 对象。
     history            对 History 对象的只读引用。请参数 History 对象。
     innerheight        返回窗口的文档显示区的高度。
     innerwidth         返回窗口的文档显示区的宽度。
     length             设置或返回窗口中的框架数量。
     location           用于窗口或框架的 Location 对象。请参阅 Location 对象。
     name               设置或返回窗口的名称。
     Navigator          对 Navigator 对象的只读引用。请参数 Navigator 对象。
     opener             返回对创建此窗口的窗口的引用。
     outerheight        返回窗口的外部高度。
     outerwidth         返回窗口的外部宽度。
     pageXOffset        设置或返回当前页面相对于窗口显示区左上角的 X 位置。
     pageYOffset        设置或返回当前页面相对于窗口显示区左上角的 Y 位置。
     parent             返回父窗口。
     Screen             对 Screen 对象的只读引用。请参数 Screen 对象。
     self               返回对当前窗口的引用。等价于 Window 属性。
     status             设置窗口状态栏的文本。
     top                返回最顶层的先辈窗口。
     window	window      属性等价于 self 属性，它包含了对窗口自身的引用。
     
     
     frameElement       Returns the <iframe> element in which the current window is inserted
     frames             Returns all <iframe> elements in the current window
     screenLeft         Returns the horizontal coordinate of the window relative to the screen
     screenTop          Returns the vertical coordinate of the window relative to the screen
     screenX            Returns the horizontal coordinate of the window relative to the screen
     screenY            Returns the vertical coordinate of the window relative to the screen
     scrollX            An alias of pageXOffset
     scrollY            An alias of pageYOffset
     self               Returns the current window
     status             Sets or returns the text in the statusbar of a window
     top                Returns the topmost browser window
     
     */
    void WindowWrap_console(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    
    void WindowWrap_innerheight(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_innerwidth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_Document(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_Location(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_Screen(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_top(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_self(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_onmousewheelGetter(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_onmousewheelSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    void WindowWrap_onloadGetter(Local<String> name, const PropertyCallbackInfo<v8::Value>& info);
    void WindowWrap_onloadSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info);
    
    /**
     *  functions wrap
     
     alert()            显示带有一段消息和一个确认按钮的警告框。
     blur()             把键盘焦点从顶层窗口移开。
     clearInterval()	取消由 setInterval() 设置的 timeout。
     clearTimeout()     取消由 setTimeout() 方法设置的 timeout。
     close()            关闭浏览器窗口。
     confirm()          显示带有一段消息以及确认按钮和取消按钮的对话框。
     createPopup()      创建一个 pop-up 窗口。
     focus()            把键盘焦点给予一个窗口。
     moveBy()           可相对窗口的当前坐标把它移动指定的像素。
     moveTo()           把窗口的左上角移动到一个指定的坐标。
     open()             打开一个新的浏览器窗口或查找一个已命名的窗口。
     print()            打印当前窗口的内容。
     prompt()           显示可提示用户输入的对话框。
     resizeBy()         按照指定的像素调整窗口的大小。
     resizeTo()         把窗口的大小调整到指定的宽度和高度。
     scrollBy()         按照指定的像素值来滚动内容。
     scrollTo()         把内容滚动到指定的坐标。
     setInterval()      按照指定的周期（以毫秒计）来调用函数或计算表达式。
     setTimeout()       在指定的毫秒数后调用函数或计算表达式。
     
     */
    void WindowWrap_alert(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_open(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_XMLHttpRequest(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_setInterval(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_setTimeout(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_scrollTo(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_addEventListener(const FunctionCallbackInfo<Value>& args);
    void WindowWrap_scroll(const FunctionCallbackInfo<Value>& args);
    
}

#endif

