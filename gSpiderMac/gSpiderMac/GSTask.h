//
//  GSTask.h
//  gSpiderMac
//
//  Created by reich on 14/11/21.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

#ifndef __gSpiderMac__GSTask__
#define __gSpiderMac__GSTask__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <v8.h>

#include <assert.h>
#include <wchar.h>

class GSTask {
public:
    char*   url_;
    int     depth_;
    char*   jsPath_;
public:
    GSTask(const char* url, const char* js, int dep = 0);
    ~GSTask();
};

#endif /* defined(__gSpiderMac__GSTask__) */
