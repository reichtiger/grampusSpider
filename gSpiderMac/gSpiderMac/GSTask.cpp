//
//  GSTask.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/21.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#include "GSTask.h"

GSTask::GSTask(const char* url, const char* js, int dep)
{
    size_t url_len = strlen(url);
    size_t js_len = strlen(js);
    
    url_ = new char[url_len+1];
    if (url_) {
        strncpy(url_, url, url_len);
        url_[url_len] = '\0';
    }
    
    jsPath_ = new char[js_len+1];
    if (jsPath_) {
        strncpy(jsPath_, js, js_len);
        jsPath_[js_len] = '\0';
    }
    
    depth_ = dep;
}

GSTask::~GSTask()
{
    if (url_ != NULL) {
        delete url_;
    }
    
    if (jsPath_ != NULL ) {
        delete jsPath_;
    }
}