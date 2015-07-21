//
//  WindowWrap.cpp
//  gSpiderMac
//
//  Created by reichtiger on 10/6/14.
//  Copyright (c) 2014 zhaohu. All rights reserved.
//


//#include "Stdafx.h"

#include "WindowWrap.h"

using namespace gSpider;

extern char    gProto[16];
extern char    gHost[32];
extern char*   gPath;
extern int     gPort;

Location * Loc = new Location;

WINDOW_EVENTS* window_evt_list = NULL;



namespace gSpider {

    Persistent<Function> mouseFunc_save;
    Persistent<Value> window_onload_fv;
    bool windowonloadsetted = false;
    //static HTMLWindow* htmlWin;
    
    //static HTMLDocument* win_doc;
    static Screen* win_screen;

    
    /**
     * HTMLWindow functions ++++++++++++++++++++++++++++++++++++++++++++
     */
    
    HTMLWindow::HTMLWindow()
    {
        mousecallback_setted = false;
    }
    
    void HTMLWindow::open()
    {
        printf("=============\naaa=====\n");
    }
    
    void HTMLWindow::mousewheel_callback(const FunctionCallbackInfo<v8::Value>& args )
    {
        Local<Value> save_val = Local<Value>::New(args.GetIsolate(), _onmousewheel);
        
        // If there is no Process function, or if it is not a function,
        // bail out
        if (save_val->IsFunction()){
            printf("onmousewheel callback_calling ============\n");
            
            Handle<Function> process_fun = Handle<Function>::Cast(save_val);
            //ori_ptr->_onreadystatechange.Reset(info.GetIsolate(), process_fun);
            mouseFunc_save.Reset(args.GetIsolate(), process_fun);
            // It is a function; cast it to a Function
            v8::Local<v8::Function> process = v8::Local<v8::Function>::New(args.GetIsolate(), mouseFunc_save);
            Handle<Value> result = process->Call(args.Holder(), 0 , NULL);
            if (result.IsEmpty()) {
                
            }
        }
    }
    
    void HTMLWindow::onload(const FunctionCallbackInfo<v8::Value>& args)
    {
        Local<Value> save_val = Local<Value>::New(args.GetIsolate(), _onmousewheel);
        Persistent<Function> func_save;
        // If there is no Process function, or if it is not a function,
        // bail out
        if (save_val->IsFunction()){
            printf("window onload calling ============\n");
            
            Handle<Function> process_fun = Handle<Function>::Cast(save_val);
            //ori_ptr->_onreadystatechange.Reset(info.GetIsolate(), process_fun);
            mouseFunc_save.Reset(args.GetIsolate(), process_fun);
            // It is a function; cast it to a Function
            v8::Local<v8::Function> process = v8::Local<v8::Function>::New(args.GetIsolate(), func_save);
            Handle<Value> result = process->Call(args.Holder(), 0 , NULL);
            if (result.IsEmpty()) {
                
            }
        }
    }
    
    // wrap console
    Handle<Object> consoleWrap(Isolate* isolate, Console* pConsole)
    {
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        
        // functions
        templ->Set(isolate, "log", FunctionTemplate::New(isolate, consoleWrap_log));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, pConsole));
        
        return handle_scope.Escape(result);
    }
    
    void consoleWrap_log(const FunctionCallbackInfo<Value>& args)
    {
        int n = args.Length();
        
        bool first = true;
        for (int i = 0; i < n; i++) {
            if (first) {
                first = false;
                sp_debug("console LOG ===========>");
            } else {
                sp_debug(" ");
            }
            
            String::Utf8Value str(args[i]);
            if (args[i]->IsString()) {
                sp_debug("%s\n", *str);
            }
            else if (args[i]->IsInt32()){
                int x =  args[i]->Int32Value();
                sp_debug("%d\n", x);
            }
            else if (args[i]->IsObject()){
                sp_debug("%s\n", *str);
            }
            
            
        }
        sp_debug("\n");
        fflush(stdout);
        
    }
    /**
     *
     *  window object wrap
     */
    Handle<Object> WrapHTMLWindowObject(Isolate* isolate, HTMLWindow* htmlWin) {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "innerHeight"), WindowWrap_innerheight );
        templ->SetAccessor(String::NewFromUtf8(isolate, "innerWidth"), WindowWrap_innerwidth );
        templ->SetAccessor(String::NewFromUtf8(isolate, "document"), WindowWrap_Document );
        templ->SetAccessor(String::NewFromUtf8(isolate, "location"), WindowWrap_Location );
        templ->SetAccessor(String::NewFromUtf8(isolate, "screen"), WindowWrap_Screen );
        templ->SetAccessor(String::NewFromUtf8(isolate, "top"), WindowWrap_top );
        templ->SetAccessor(String::NewFromUtf8(isolate, "self"), WindowWrap_self );
        templ->SetAccessor(String::NewFromUtf8(isolate, "onmousewheel"), WindowWrap_onmousewheelGetter, WindowWrap_onmousewheelSetter );
        templ->SetAccessor(String::NewFromUtf8(isolate, "onload"), WindowWrap_onloadGetter, WindowWrap_onloadSetter );
        
        
        // functions
        templ->Set(isolate, "alert", FunctionTemplate::New(isolate, WindowWrap_alert));
        templ->Set(isolate, "open", FunctionTemplate::New(isolate, WindowWrap_open));
        templ->Set(isolate, "XMLHttpRequest", FunctionTemplate::New(isolate, WindowWrap_XMLHttpRequest));
        templ->Set(isolate, "setInterval", FunctionTemplate::New(isolate, WindowWrap_setInterval));
        templ->Set(isolate, "setTimeout", FunctionTemplate::New(isolate, WindowWrap_setTimeout ));
        templ->Set(isolate, "scrollTo", FunctionTemplate::New(isolate, WindowWrap_scrollTo ));
        templ->Set(isolate, "addEventListener", FunctionTemplate::New(isolate, WindowWrap_addEventListener ));
        templ->Set(isolate, "scroll", FunctionTemplate::New(isolate, WindowWrap_scroll ));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, htmlWin));
        
        return handle_scope.Escape(result);
    }
    
    HTMLWindow* getWindowPtr(Local<Object> self)
    {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        return static_cast<HTMLWindow*>(ptr);
    }
    
    /**
     *      Properties --------------------------------------------
     
     
     alert()	显示带有一段消息和一个确认按钮的警告框。
     blur()	把键盘焦点从顶层窗口移开。
     clearInterval()	取消由 setInterval() 设置的 timeout。
     clearTimeout()	取消由 setTimeout() 方法设置的 timeout。
     close()	关闭浏览器窗口。
     confirm()	显示带有一段消息以及确认按钮和取消按钮的对话框。
     createPopup()	创建一个 pop-up 窗口。
     focus()	把键盘焦点给予一个窗口。
     moveBy()	可相对窗口的当前坐标把它移动指定的像素。
     moveTo()	把窗口的左上角移动到一个指定的坐标。
     open()	打开一个新的浏览器窗口或查找一个已命名的窗口。
     print()	打印当前窗口的内容。
     prompt()	显示可提示用户输入的对话框。
     resizeBy()	按照指定的像素调整窗口的大小。
     resizeTo()	把窗口的大小调整到指定的宽度和高度。
     scrollBy()	按照指定的像素值来滚动内容。
     scrollTo()	把内容滚动到指定的坐标。
     setInterval()	按照指定的周期（以毫秒计）来调用函数或计算表达式。
     setTimeout()	在指定的毫秒数后调用函数或计算表达式。
     
     
     */
    void WindowWrap_console(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    
    void WindowWrap_XMLHttpReq(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        // if window.XMLHttpRequest visted, return true for it .
        info.GetReturnValue().Set(true);
        
    }
    
    void WindowWrap_innerheight(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(Integer::New(info.GetIsolate(), 3000));
    }
    
    void WindowWrap_innerwidth(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(Integer::New(info.GetIsolate(), 1900));
    }
    
    void WindowWrap_Document(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLWindow* ori_ptr = getWindowPtr(info.Holder());
        
        info.GetReturnValue().Set(ori_ptr->doc_Handle);
        /*
        if (ori_ptr) {
            Local<Object> result = init_DomWrap(info.GetIsolate(), ori_ptr->the_doc);
            
            info.GetReturnValue().Set(result);
        }
         */
    }
    
    void WindowWrap_Location(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        if (Loc) {
            Local<Object> result = LocationWrap(info.GetIsolate(), Loc);
            
            info.GetReturnValue().Set(result);
        }
    }
    
    void WindowWrap_Screen(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        win_screen = new Screen;
        if (win_screen) {
            Local<Object> result = ScreenWrap(info.GetIsolate(), win_screen);
            
            info.GetReturnValue().Set(result);
        }
    }
    
    void WindowWrap_top(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(info.Holder());
    }
    
    void WindowWrap_self(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(info.Holder());
    }
    
    void WindowWrap_onmousewheelGetter(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLWindow* ori_ptr = getWindowPtr(info.Holder());
        info.GetReturnValue().Set(ori_ptr->_onmousewheel);
    }
    
    void WindowWrap_onmousewheelSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        String::Utf8Value str(name);
        
        HTMLWindow* ori_ptr = getWindowPtr(info.Holder());
        
        ori_ptr->_onmousewheel.Reset(info.GetIsolate(), value_obj);
        
        printf("wrap: window onmousewheel setted ++++++++++++++++++++++ \n");
        
        ori_ptr->mousecallback_setted = true;
    }
    
    void WindowWrap_onloadGetter(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void WindowWrap_onloadSetter(Local<String> name,  Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        sp_debug("setting window.onload event...");
        window_onload_fv.Reset(info.GetIsolate(), value_obj);
        windowonloadsetted = true;
    }
    
    
    /**
     *      other objects as properties ===========================================
     
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
    
    void WindowWrap_alert(const FunctionCallbackInfo<Value>& args)
    {
        String::Utf8Value str1(args[0]);
        
        printf("wrap__alert: %s\n", *str1);
    }
    
    void WindowWrap_open(const FunctionCallbackInfo<Value>& args)
    {
        printf("wrap: window open called\n");
        
    }
    
    
    void WindowWrap_XMLHttpRequest(const FunctionCallbackInfo<Value>& args)
    {
        Local<Object> result = get_ajax_obj(args);
        args.GetReturnValue().Set(result);
        
    }
    
    void WindowWrap_setInterval(const FunctionCallbackInfo<Value>& args)
    {
        printf("setInterval called\n");
    }
    
    void WindowWrap_setTimeout(const FunctionCallbackInfo<Value>& args)
    {
        String::Utf8Value str1(args[0]);
        
        printf("wrap: setTimeout called -------func= %s\n", *str1);
        return;
        Persistent<Value> ft;
        ft.Reset(args.GetIsolate(), args[0]);
        usleep(5);
        
        Local<Value> save_val = Local<Value>::New(args.GetIsolate(), ft);
        Persistent<Function> gs_save;
        
        if (save_val->IsFunction()){
            sp_debug("wrap: setTimeout function running ============\n");
            Handle<Function> process_fun = Handle<Function>::Cast(save_val);
            gs_save.Reset(args.GetIsolate(), process_fun);
            // It is a function; cast it to a Function
            v8::Local<v8::Function> process = v8::Local<v8::Function>::New(args.GetIsolate(), gs_save);
            Handle<Value> result = process->Call(args.Holder(), 0 , NULL);
            if (result.IsEmpty()) {
                return;
            }
        }
        
    }
    
    void WindowWrap_scrollTo(const FunctionCallbackInfo<Value>& args)
    {
        String::Utf8Value xx(args[0]);
        String::Utf8Value yy(args[1]);
        
        printf("wrap: scrolled to (%s, %s) \n", *xx, *yy);
        
        // trigger to callback onmousewheel function
        
        HTMLWindow* ori_ptr = getWindowPtr(args.Holder());
        if (ori_ptr->mousecallback_setted)
            ori_ptr->mousewheel_callback(args);
        
    }
    
    void WindowWrap_scroll(const FunctionCallbackInfo<Value>& args)
    {
        String::Utf8Value xx(args[0]);
        String::Utf8Value yy(args[1]);
        
        printf("wrap: scroll to (%s, %s) \n", *xx, *yy);
        
        // trigger to callback onmousewheel function
        
        HTMLWindow* ori_ptr = getWindowPtr(args.Holder());
        if (ori_ptr->mousecallback_setted)
            ori_ptr->mousewheel_callback(args);
        
    }
    
    void WindowWrap_addEventListener(const FunctionCallbackInfo<Value>& args)
    {
        
        String::Utf8Value xx(args[0]);
        String::Utf8Value yy(args[1]);
        
        
        printf("wrap: Window's addEventListener called .....(%s, %s)\n", *xx, *yy);
        
        
    }

} // end namespace