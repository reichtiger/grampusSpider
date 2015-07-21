//
//  spiderActions.h
//  gSpiderMac
//
//  Created by reich on 14/11/9.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__spiderActions__
#define __gSpiderMac__spiderActions__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <v8.h>
#include "WZ_HTTPSocket.h"
#include "util.h"

using namespace v8;

extern int gClick_pages;
extern int gCurPage;

namespace gSpider {
    
    static void add_link_Callback(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        
        
    }
    
    static void save_Callback(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        for (int i = 0; i < args.Length(); i++) {
            
            String::Utf8Value str(args[i]);
            if (args[i]->IsString()) {
                printf("SAVING STRING=====>%s\n", *str);
            }else{
                printf("PRINT (OBJECT) --->%s\n", *str);
            }
            
        }
        printf("\n");
        fflush(stdout);
        
    }
    
    static void log_Callback(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        
        bool first = true;
        for (int i = 0; i < args.Length(); i++) {
            if (first) {
                first = false;
            } else {
                printf(" ");
            }
            String::Utf8Value str(args[i]);
            if (args[i]->IsString()) {
                printf("LOG (string) --->%s\n", *str);
            }
            else if (args[i]->IsInt32()){
                int x =  args[i]->Int32Value();
                printf("LOG (int) --> %d\n", x);
            }
            else if (args[i]->IsObject()){
                printf("LOG (Object) --> %s\n", *str);
            }
            
            
        }
        printf("\n");
        fflush(stdout);
    }
    
    static void startLinkPage_call(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (args[0]->IsInt32()) {
            gClick_pages = args[0]->Int32Value();
            gCurPage += 1;
        }
    }
    
    static void endLinkPage_call(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (gCurPage == gClick_pages) {
            printf("pages end.\n");
        }
    }
    
    // do the click on element
    //  two args need : gs_CLick(item, deepIndex);
    //  item : document.getElementById('xxx'), the element got
    //  deepIndex : the level of URL, default=0
    
    static void click_call(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        // first do the click
    }
    
}
#endif /* defined(__gSpiderMac__spiderActions__) */
