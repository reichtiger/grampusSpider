//
//  DomWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/1.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

//#include "Stdafx.h"

#include "DomWrap.h"

using namespace gSpider;

extern Location* Loc;
extern EVENT_OBJS* eventList;

namespace gSpider {
    
    HTMLDocument::HTMLDocument()
    {
        _innerHTML = NULL;
        _innerHTML = new char[64];
        strcat(_innerHTML, "this is by William.zhao, reichtiger@gmail.com");
    }
    
    HTMLDocument::~HTMLDocument()
    {
        printf("~~~~~~~~~~~~~~ dealloc Document\n");
        delete dom_tree;
        if (_innerHTML != NULL) delete [] _innerHTML;
    }
    
    char* HTMLDocument::getHTML()
    {
        //printf("getName() called ......%s\n", _innerHTML);
        //_innerHTML = w2m(html_tree->getHTMLString().c_str());
        return _innerHTML;
    }
    
    void HTMLDocument::removeTree()
    {
        delete dom_tree;
    }
    
    void HTMLDocument::setTree(wchar_t* inStr)
    {
        dom_tree = HTMLTree::getRoot(inStr);
    }
    /*******************************************************************************
     * HTMLDocument Wrap definition
     *
     
     HTML DOM Nodes
     In the HTML DOM (Document Object Model), everything is a node:
     
     The document itself is a document node
     All HTML elements are element nodes
     All HTML attributes are attribute nodes
     Text inside HTML elements are text nodes
     Comments are comment nodes
     ******************************************************************************/
    
    /**
     *  create HTMLDocument and parse html into it . return document object to JS
     *
     */
    Handle<Object> init_DomWrap(Isolate* isolate, HTMLDocument* html_doc)
    {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "innerHTML"), DomWrap_innerHTML, DomWrap_innerHTML_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "cookie"), DomWrap_cookie, DomWrap_cookie_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "location"), DomWrap_Location);
        templ->SetAccessor(String::NewFromUtf8(isolate, "body"), DomWrap_Location);
        templ->SetAccessor(String::NewFromUtf8(isolate, "defaultView"), DomWrap_defaultView);
        templ->SetAccessor(String::NewFromUtf8(isolate, "nodeType"), DomWrap_nodeType);
        templ->SetAccessor(String::NewFromUtf8(isolate, "documentElement"), DomWrap_documentElement );
        templ->SetAccessor(String::NewFromUtf8(isolate, "readyState"), DomWrap_readyState );
        
        // functions
        templ->Set(isolate, "getElementById", FunctionTemplate::New(isolate, DomWrap_getElementById));
        templ->Set(isolate, "getElementsByTagName", FunctionTemplate::New(isolate, DomWrap_getElementsByTagName));
        templ->Set(isolate, "getElementsByClassName", FunctionTemplate::New(isolate, DomWrap_getElementsByClassName));
        templ->Set(isolate, "createElement", FunctionTemplate::New(isolate, DomWrap_createElement));
        templ->Set(isolate, "createTextNode", FunctionTemplate::New(isolate, DomWrap_createTextNode));
        templ->Set(isolate, "addEventListener", FunctionTemplate::New(isolate, DomWrap_addEventListener));
        templ->Set(isolate, "createDocumentFragment", FunctionTemplate::New(isolate, DomWrap_createDocumentFragment));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, html_doc));
        
        return handle_scope.Escape(result);
    }
    
    void DomWrap_innerHTML(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        char* ret = ttt->getHTML();
        if (ret) {
            info.GetReturnValue().Set(String::NewFromUtf8(
                                                          info.GetIsolate(), ret, String::kNormalString,
                                                          static_cast<int>(strlen(ret))));
        }
        
    }
    
    void DomWrap_innerHTML_setter(Local<String> name,
                                 Local<Value> value_obj,
                                 const PropertyCallbackInfo<void>& info)
    {
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        if (!value_obj.IsEmpty()) {
            String::Utf8Value str(value_obj);
            wchar_t* wstr = m2w(*str);
            ttt->removeTree();
            ttt->setTree(wstr);
        }
        
    }
    
    void DomWrap_cookie(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "jd;com"));
    }
    
    void DomWrap_cookie_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        // do nothing
    }
    
    void DomWrap_body(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        vector<HTMLBranch*> brchs;
        ttt->dom_tree->getBranchesByTagName((wchar_t*)L"body", brchs);
        int nums = (int)brchs.size();
        
        if (nums == 1) {
            Handle<Object> retObj = element_Wrap(info.GetIsolate(), brchs[0]);
            info.GetReturnValue().Set(retObj);
        }
        
        
    }
    
    void DomWrap_nodeType(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(Integer::New(info.GetIsolate(), 9));
    }
    
    void DomWrap_documentElement(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        HTMLBranch* rrr = ttt->dom_tree;
        
        if (rrr) {
            Handle<Object> retObj = element_Wrap(info.GetIsolate(), rrr);
            
            info.GetReturnValue().Set(retObj);
        }
        
    }
    
    void DomWrap_readyState(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        // because we always call runJSCode AFTER parsed the HTML Dom , so it must be complete.
        
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "complete"));
    }
    
    
    Handle<Object> WrapHTMLWindowObject2(Isolate* isolate, HTMLWindow* htmlWin) {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, htmlWin));
        
        return handle_scope.Escape(result);
    }
    
    void DomWrap_defaultView(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        Handle<Object> window_obj = WrapHTMLWindowObject2(info.GetIsolate(), ttt->parentWin);
        info.GetReturnValue().Set(window_obj);
    }
    
    /**
     *  get Element functions
     *
     *
     */
    
    
    
    void DomWrap_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Local<External> wrap = Local<External>::Cast(args.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        String::Utf8Value str1(args[0]);
        //strcat(*str1, " - called");
        sp_debug("...........getElementById: %s ", *str1);
        wchar_t* wstr = m2w(*str1);
        HTMLBranch* rrr = ttt->dom_tree->getBranchByAttrname((wchar_t*)L"id", wstr);
        
        if (rrr) {
            sp_debug("...Done\n");
        }
        
        Handle<Object> retObj = element_Wrap(args.GetIsolate(), rrr);
        if (*str1) {
            //args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), *str1, String::kNormalString,static_cast<int>(strlen(*str1))));
            
            args.GetReturnValue().Set(retObj);
        }
        args.GetReturnValue().Set(retObj);
    }
    
    void DomWrap_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        EscapableHandleScope handle_scope(args.GetIsolate());
        Local<External> wrap = Local<External>::Cast(args.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        if (ttt->dom_tree == NULL) {
            args.GetReturnValue().Set(Null(args.GetIsolate()));
            return;
        }
        
        String::Utf8Value str1(args[0]);
        //strcat(*str1, " - called");
        sp_debug(".........getElementsByTagName:%s\n", *str1);
        wchar_t* wstr = m2w(*str1);
        if (wstr == NULL ) {
            printf("NULL name\n");
            return;
        }
        
        vector<HTMLBranch*> brchs;
        ttt->dom_tree->getBranchesByTagName(wstr, brchs);
        int nums = (int)brchs.size();
        
        Local<Array> aryOfObjs = Array::New(args.GetIsolate(), nums);
        
        // Return an empty result if there was an error creating the array.
        if (aryOfObjs.IsEmpty())
        {
            
        }
        
        
        if (brchs.size() > 0) {
            vector<HTMLBranch*>::iterator bit;
            int x = 0;
            for (bit = brchs.begin(); bit != brchs.end(); bit++) {
               Handle<Object> retObj = element_Wrap(args.GetIsolate(), *bit.base());
                aryOfObjs->Set(x, retObj);
                x++;
            }
        }
        
        args.GetReturnValue().Set(handle_scope.Escape(aryOfObjs));
        
    }
    
    void DomWrap_getElementsByClassName(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        EscapableHandleScope handle_scope(args.GetIsolate());
        Local<External> wrap = Local<External>::Cast(args.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        String::Utf8Value str1(args[0]);
        //strcat(*str1, " - called");
        sp_debug("getElementsByClassName:%s\n", *str1);
        wchar_t* wstr = m2w(*str1);
        vector<HTMLBranch*> brchs;
        ttt->dom_tree->getBranchesByAttr(brchs, (wchar_t*)L"class", wstr);
        int nums = (int)brchs.size();
        
        Local<Array> aryOfObjs = Array::New(args.GetIsolate(), nums);
        
        // Return an empty result if there was an error creating the array.
        if (aryOfObjs.IsEmpty())
        {
            
        }
        
        if (brchs.size() > 0) {
            vector<HTMLBranch*>::iterator bit;
            int x = 0;
            for (bit = brchs.begin(); bit != brchs.end(); bit++) {
                Handle<Object> retObj = element_Wrap(args.GetIsolate(), *bit.base());
                aryOfObjs->Set(x, retObj);
                x++;
            }
        }
        
        Persistent<Value> ret;
        ret.Reset(args.GetIsolate(), aryOfObjs);
        
        args.GetReturnValue().Set(ret);
    }
    
    void DomWrap_createElement(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        EscapableHandleScope handle_scope(args.GetIsolate());
        String::Utf8Value str1(args[0]);
        
        sp_debug(".................creating Element 【%s】\n", *str1);
        
        if (strcmp(*str1, "option") == 0) {
            
        }
        else{
            
        }
        
        HTMLBranch* new_brch = new HTMLBranch(m2w(*str1));
        new_brch->type = TYPE_DOM_BRANCH;
        
        Handle<Object> retObj = element_Wrap(args.GetIsolate(), new_brch);
        retObj->SetPrototype(retObj); // set property enabled
        
        if (*str1) {
            args.GetReturnValue().Set(retObj);
        }
        args.GetReturnValue().Set(retObj);
        
    }
    
    void DomWrap_createTextNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        //EscapableHandleScope handle_scope(args.GetIsolate());
        String::Utf8Value str1(args[0]);
        
        sp_debug(".................creating TextNode 【%s】\n\n", *str1);
        
        HTMLBranch* leaf_brch = new HTMLBranch;
        leaf_brch->isLeaf = true;
        
        wchar_t* wstr = m2w(*str1);
        size_t len = wcslen(wstr);
        leaf_brch->pLeafValue = new wchar_t[len+1];
        wcscpy(leaf_brch->pLeafValue, m2w(*str1));
        leaf_brch->pLeafValue[len] = '\0';
        
        Handle<Object> retObj = textNode_Wrap(args.GetIsolate(), leaf_brch);
        
        args.GetReturnValue().Set(retObj);
        
    }
    
    void DomWrap_Location(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
        if (Loc) {
            Local<Object> result = LocationWrap(info.GetIsolate(), Loc);
            
            info.GetReturnValue().Set(result);
        }
    }
    
    // addEventListener
    void DomWrap_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Local<External> wrap = Local<External>::Cast(args.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLDocument* ttt = static_cast<HTMLDocument*>(ptr);
        
        HTMLBranch* curBrch = ttt->dom_tree;
        
        if (curBrch) {
            
            String::Utf8Value str1(args[0]);
            String::Utf8Value str2(args[1]);
            
            printf("Dom addEventListener %s(\n======\n%s\n====== on %ls\n", *str1, *str2, curBrch->brch_name);
            
            curBrch->addClickEvent(EVENT_TYPE_CLICK, m2w(*str2));
            
            if (eventList == NULL) {
                eventList = new EVENT_OBJS;
                eventList->p = curBrch;
                eventList->type = EVENT_TYPE_CLICK;
                eventList->fv.Reset(args.GetIsolate(), args[1]);
                eventList->next = NULL;
            }else{
                EVENT_OBJS* cur = eventList;
                EVENT_OBJS* last = eventList;
                
                while (cur != NULL) {
                    last = cur;
                    cur = eventList->next;
                }
                
                EVENT_OBJS* new_ev = new EVENT_OBJS;
                new_ev->p = curBrch;
                eventList->type = EVENT_TYPE_CLICK;
                new_ev->fv.Reset(args.GetIsolate(), args[1]);
                new_ev->next = NULL;
                
                last->next = new_ev;
            }
            
            
        }

    }
    
    
    /**
     *  pretend branch as document fragment, just pay attention to methods of appendChild, etc.
     
        NOTE:  if add a child , the child is from dom , then must remove from dom.
     
        when fragment is been added to others, just add fragment's childs without fragment itself !!!
     
     */
    void DomWrap_createDocumentFragment(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        EscapableHandleScope handle_scope(args.GetIsolate());
        sp_debug(".....creating DocumentFragment \n");
        HTMLBranch* frag = new HTMLBranch;
        frag->type = TYPE_DOM_FREG;
        
        Handle<Object> retObj = element_Wrap(args.GetIsolate(), frag);
        retObj->SetPrototype(retObj); // set property enabled
        if (frag) {
            args.GetReturnValue().Set(retObj);
        }

        
    }
} // end namespace
