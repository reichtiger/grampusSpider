//
//  elementWrap.cpp
//  gSpiderMac
//
//  Created by reich on 14/11/5.
//  Copyright (c) 2014年 zhaohu. All rights reserved.
//

//#include "Stdafx.h"

#include "elementWrap.h"

extern vector<HTMLBranch*> createdJSNodes;

const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

void runJSCode(v8::Isolate* isolate, char* jscode)
{
    if (jscode == NULL ) {
        return;
    }
    Local<String> source = String::NewFromUtf8(isolate, jscode);
    v8::Local<v8::String> name(v8::String::NewFromUtf8(isolate, "(tiger+v8) ====> "));
    v8::HandleScope handle_scope(isolate);
    std::string gbkStr3 = executeString(isolate, source, name, true, true);
}

void ReportException(Isolate* isolate, v8::TryCatch* try_catch)
{
    HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn""t provide any extra information about this error; just
        // print the exception.
        printf("%s\n", exception_string);
    } else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        printf("%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        printf("%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++) {
            printf(" ");
        }
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++) {
            printf("^");
        }
        printf("\n");
        v8::String::Utf8Value stack_trace(try_catch->StackTrace());
        if (stack_trace.length() > 0) {
            const char* stack_trace_string = ToCString(stack_trace);
            printf("%s\n", stack_trace_string);
        }
    }
}

std::string executeString(v8::Isolate* isolate, v8::Handle<v8::String> source,
                          v8::Handle<v8::Value> name,
                          bool print_result,
                          bool report_exceptions)
{
    
    HandleScope handle_scope(isolate);
    v8::TryCatch try_catch;   //’‚¿Ô…Ë÷√“Ï≥£ª˙÷∆
    v8::ScriptOrigin origin(name);
    v8::Handle<v8::Script> script = v8::Script::Compile(source, &origin);
    
    if (script.IsEmpty()) {
        // Print errors that happened during compilation.
        if (report_exceptions)
            ReportException(isolate, &try_catch);
        return "";
    } else {
        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty()) {
            //ASSERT(try_catch.HasCaught());
            // Print errors that happened during execution.
            if (report_exceptions)
                ReportException(isolate, &try_catch);
            return "";
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                // If all went well and the result wasn""t undefined then print
                // the returned value.
                v8::String::Utf8Value ret(result);
                printf("%s\n", *ret);
                std::string gbk;
                //ConvertUtf8ToGBK(gbk, *ret);
                return gbk;
            }
            return "";
        }
    }
}

/**
 *  element wrap start *****************************************
 *
 */

extern EVENT_OBJS* eventList;

namespace gSpider {
    
    
    HTMLBranch* getOriPtrBranch(Local<Object> self)
    {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        return static_cast<HTMLBranch*>(ptr);
    }

    
    void elementWrap_innerHTML(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            char* ret = w2m(br->innerHTMLStr().c_str());
            if (ret) {
                info.GetReturnValue().Set(String::NewFromUtf8(
                                                              info.GetIsolate(), ret, String::kNormalString,
                                                              static_cast<int>(strlen(ret))));
            }
        }
        
    }
    
    void elementWrap_innerHTML_setter(Local<String> name,
                                      Local<Value> value_obj,
                                      const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                br->set_innerHTML(wstr);
            }

        }
        
    }
    
    void elementWrap_firstChild(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLBranch* curNode = getOriPtrBranch(info.Holder());
        HTMLBranch* firstC = curNode->pFirstChild;
        if (firstC) {
            Handle<Object> retObj = element_Wrap(info.GetIsolate(), firstC);
            
            info.GetReturnValue().Set(retObj);
        }
    }
    
    void elementWrap_lastChild(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLBranch* curNode = getOriPtrBranch(info.Holder());
        HTMLBranch* firstC = curNode->pFirstChild;
        HTMLBranch* lastC = firstC;
        
        while (firstC) {
            lastC = firstC;
            firstC = firstC->sibling_next;
        }
        
        Handle<Object> retObj = element_Wrap(info.GetIsolate(), lastC);
        info.GetReturnValue().Set(retObj);
    }
    
    void elementWrap_parentNode(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLBranch* curNode = getOriPtrBranch(info.Holder());
        
        HTMLBranch* parentN = curNode->pParent;
        if (parentN) {
            Handle<Object> retObj = element_Wrap(info.GetIsolate(), parentN);
            
            info.GetReturnValue().Set(retObj);
            
        }
        
    }
    
    // nodeType
    void elementWrap_nodeType(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        // 1 = element node, 9 = document node
        info.GetReturnValue().Set(Integer::New(info.GetIsolate(), 1));
    }
    
    // type
    void elementWrap_type(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void elementWrap_type_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
            
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    // src
    void elementWrap_src(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void elementWrap_src_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    // async
    void elementWrap_async(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void elementWrap_async_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    
    //href
    void elementWrap_href(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "www.test.com/test/index.php"));
    }
    void elementWrap_href_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    
    // id property
    void elementWrap_id(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void elementWrap_id_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    
    // class property
    void elementWrap_class(Local<String> name, const PropertyCallbackInfo<v8::Value>& info){}
    void elementWrap_class_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            if (!value_obj.IsEmpty()) {
                String::Utf8Value str(value_obj);
                wchar_t* wstr = m2w(*str);
                
                String::Utf8Value name_(name);
                wchar_t* attr_name = m2w(*name_);
                
                br->setAttribute(attr_name, wstr);
                
                delete attr_name;
                delete wstr;
            }
            
        }
    }
    
    void elementWrap_offsetTop(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(Integer::New(info.GetIsolate(), 9900));
    }
    
    // <input value="hello">
    void elementWrap_value(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            wchar_t* v = br->getAttribute((wchar_t*)L"value");
            if (v) {
                info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), w2m(v)));
            }
            
        }
    }
    
    void elementWrap_value_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            String::Utf8Value str(value_obj);
            wchar_t* wstr = m2w(*str);
            
            br->setAttribute((wchar_t*)L"value", wstr);
            
        }
    }
    
    // onsubmit
    void elementWrap_onsubmit(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        
    }
    void elementWrap_onsubmit_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            String::Utf8Value str(value_obj);
            wchar_t* wstr = m2w(*str);
            
            br->setAttribute((wchar_t*)L"onsubmit", wstr);
            
        }
    }
    
    // map indexed attributes object
    void elementWrap_attributes(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        EscapableHandleScope handle_scope(info.GetIsolate());
        
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        int nums = (int)br->attrs.size();
        
        Local<Array> aryOfObjs = Array::New(info.GetIsolate(), nums);
        
        if (nums > 0) {
            vector<ATTRIBUTE>::iterator bit;
            int x = 0;
            for (bit = br->attrs.begin(); bit != br->attrs.end(); bit++) {
                Handle<Object> retObj = attribute_Wrap(info.GetIsolate(), bit.base());
                
                ATTRIBUTE* aa = bit.base();
                char* name = w2m(aa->attr_name);
                
                aryOfObjs->Set(x, retObj);
                aryOfObjs->Set(String::NewFromUtf8(info.GetIsolate(), name), retObj);
                x++;
            }
        }
        
        info.GetReturnValue().Set(handle_scope.Escape(aryOfObjs));
    }
    
    void elementWrap_selected(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        info.GetReturnValue().Set(false);
    }
    
    /**
     * other objects
     */
    Handle<Object> textNode_Wrap(Isolate* isolate, HTMLBranch* leaf_new)
    {
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "innerHTML"), elementWrap_innerHTML, elementWrap_innerHTML_setter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, leaf_new));
        
        return handle_scope.Escape(result);
    }
    
    void elementWrap_style(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        EscapableHandleScope handle_scope(info.GetIsolate());
        
        //HTMLBranch* br = getOriPtrBranch(info.Holder());
        
        // here pretend to get style object, in fact use NULL for go through
        Handle<Object> retObj = cssstyle_Wrap(info.GetIsolate(), NULL);
        
        info.GetReturnValue().Set(retObj);
    }
    
    Handle<Object> element_Wrap(Isolate* isolate, HTMLBranch* brch)
    {
        
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "innerHTML"), elementWrap_innerHTML, elementWrap_innerHTML_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "parentNode"), elementWrap_parentNode);
        templ->SetAccessor(String::NewFromUtf8(isolate, "firstChild"), elementWrap_firstChild);
        templ->SetAccessor(String::NewFromUtf8(isolate, "lastChild"), elementWrap_lastChild);
        templ->SetAccessor(String::NewFromUtf8(isolate, "type"), elementWrap_type, elementWrap_type_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "src"), elementWrap_src, elementWrap_src_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "async"), elementWrap_async, elementWrap_async_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "href"), elementWrap_href, elementWrap_href_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "id"), elementWrap_id, elementWrap_id_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "class"), elementWrap_class, elementWrap_class_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "offsetTop"), elementWrap_offsetTop);
        templ->SetAccessor(String::NewFromUtf8(isolate, "nodeType"), elementWrap_nodeType);
        templ->SetAccessor(String::NewFromUtf8(isolate, "value"), elementWrap_value, elementWrap_value_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "defaultValue"), elementWrap_value, elementWrap_value_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "onsubmit"), elementWrap_onsubmit, elementWrap_onsubmit_setter);
        templ->SetAccessor(String::NewFromUtf8(isolate, "attributes"), elementWrap_attributes);
        templ->SetAccessor(String::NewFromUtf8(isolate, "style"), elementWrap_style);
        //templ->SetAccessor(String::NewFromUtf8(isolate, "click"), elementWrap_click, elementWrap_click_setter);
        
        // for option element
        templ->SetAccessor(String::NewFromUtf8(isolate, "selected"), elementWrap_selected); // for option element
        
        // functions
        templ->Set(isolate, "appendChild", FunctionTemplate::New(isolate, ElementWrap_appendChild));
        templ->Set(isolate, "removeChild", FunctionTemplate::New(isolate, ElementWrap_removeChild));
        templ->Set(isolate, "insertBefore", FunctionTemplate::New(isolate, ElementWrap_insertBefore));
        templ->Set(isolate, "getAttribute", FunctionTemplate::New(isolate, ElementWrap_getAttribute));
        templ->Set(isolate, "click", FunctionTemplate::New(isolate, ElementWrap_click ));
        templ->Set(isolate, "addEventListener", FunctionTemplate::New(isolate, ElementWrap_addEventListener));
        templ->Set(isolate, "getElementsByTagName", FunctionTemplate::New(isolate, ElementWrap_getElementsByTagName));
        templ->Set(isolate, "cloneNode", FunctionTemplate::New(isolate, ElementWrap_cloneNode));
        templ->Set(isolate, "setAttribute", FunctionTemplate::New(isolate, ElementWrap_setAttribute));
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, brch));
        
        return handle_scope.Escape(result);
    }
    
    Handle<Object> attribute_Wrap(Isolate* isolate, ATTRIBUTE* attr)
    {
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "value"), attributeWrap_value, attributeWrap_value_setter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, attr));
        
        return handle_scope.Escape(result);

    }
    
    
    void attributeWrap_value(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        ATTRIBUTE* attr = static_cast<ATTRIBUTE*>(ptr);
        
        if (attr) {
            char* aa = w2m(attr->attr_value);
            if (aa ) {
                info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), aa));
            }
            else info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), ""));
        }
    }
    
    void attributeWrap_value_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        
    }
    
    
    Handle<Object> cssstyle_Wrap(Isolate* isolate, ATTRIBUTE* attr)
    {
        EscapableHandleScope handle_scope(isolate);
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        // set accessores
        templ->SetAccessor(String::NewFromUtf8(isolate, "value"), attributeWrap_value, attributeWrap_value_setter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(isolate, attr));
        
        return handle_scope.Escape(result);
        
    }
    
    void elementWrap_click(Local<String> name, const PropertyCallbackInfo<v8::Value>& info)
    {
        sp_debug("element click property reading.....%s\n");

    }
    void elementWrap_click_setter(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<void>& info)
    {
        String::Utf8Value name2(name);
        String::Utf8Value str(value_obj);
        sp_debug("element property function【%s】 setting.....%s\n", *name2, *str);
        HTMLBranch* br = getOriPtrBranch(info.Holder());
        if (br) {
            sp_debug("++++%s\n", br->brch_name);
            if (wcscmp(br->brch_name, L"p") == 0) {
                ;;
            }
        }
        
    }
    
    /***************************************************************************
     *  
     *  methods !!!!
     *
     */
    void ElementWrap_appendChild(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        String::Utf8Value str1(args[0]);
        HTMLBranch* br = getOriPtrBranch(args.Holder());
        HTMLBranch* newChild = getOriPtrBranch(args[0]->ToObject());
        
        if (newChild->type == TYPE_DOM_BRANCH) {
            if (newChild->isLeaf) {
                printf("appending LEAF -> %ls\n", newChild->pLeafValue);
                br->addBranch(newChild);
            }else{
                //printf(".......................appending child %ls of %ls\n", newChild->brch_name, br->brch_path);
                
                if (br && newChild) {
                    
                    br->addBranch(newChild);
                    
                    args.GetReturnValue().Set(element_Wrap(args.GetIsolate(), newChild));
                }
            }
        }
        else{   // it is fragment, just add it's childs
            
            HTMLBranch* cc = newChild->pFirstChild;
            while (cc) {
                cc->type = TYPE_DOM_BRANCH; // set it to DOM now.
                br->addBranch(cc);
                cc = cc->sibling_next;
            }
        }
        
    }
    
    void ElementWrap_removeChild(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        String::Utf8Value str1(args[0]);
        HTMLBranch* br = getOriPtrBranch(args.Holder());
        HTMLBranch* newChild = getOriPtrBranch(args[0]->ToObject());
        
        if (newChild) {
            printf(".......................removing child %ls of %ls\n", newChild->brch_name, br->brch_path);
            
            br->removeBranch(newChild);
        }
    }
    
    void ElementWrap_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        HTMLBranch* br = getOriPtrBranch(args[0]->ToObject());
        HTMLBranch* before_brch = getOriPtrBranch(args[1]->ToObject());
        
        printf(".................inserting node before %ls \n", before_brch->brch_path);
        bool ret = curBrch->insertBranchBefore(br, before_brch);
        if (ret) {
            // to check if it is script element. if YES, load src or content into v8
            if (wcscmp(br->brch_name, L"script") == 0) {
                createdJSNodes.push_back(br);
            }
            
        }
    }
    
    void ElementWrap_cloneNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        Local<Boolean> isDeepClone =  args[0]->ToBoolean();
        bool is_deep = isDeepClone->BooleanValue();
        
        HTMLBranch* b2 = new HTMLBranch;
        if (b2) {
            b2->clone(curBrch, is_deep);
            Handle<Object> retObj = element_Wrap(args.GetIsolate(), b2);
            args.GetReturnValue().Set(retObj);
        }
        
    }
    
    void ElementWrap_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        
        if (curBrch) {
            String::Utf8Value attrName(args[0]);
            wchar_t* attr_name = m2w(*attrName);
            
            wchar_t* ret = curBrch->getAttribute(attr_name);
            if (ret) {
                args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), w2m(ret)));
            }
            else{
                args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), ""));
            }
        }
        
    }
    
    void ElementWrap_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        EscapableHandleScope handle_scope(args.GetIsolate());
        Local<External> wrap = Local<External>::Cast(args.Holder()->GetInternalField(0));
        void *ptr = wrap->Value();
        HTMLBranch* curBrch = static_cast<HTMLBranch*>(ptr);
        
        String::Utf8Value str1(args[0]);
        //strcat(*str1, " - called");
        printf(".........ELement's getElementsByTagName:%s\n", *str1);
        wchar_t* wstr = m2w(*str1);
        if (wstr == NULL ) {
            printf("NULL name\n");
            return;
        }
        
        vector<HTMLBranch*> brchs;
        curBrch->getBranchesByTagName(wstr, brchs);
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
    
    // element click
    void ElementWrap_click(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        
        if (curBrch) {
            
            wchar_t* onlick_js = curBrch->getAttribute((wchar_t*)L"onclick");
            if (onlick_js) {
                // do js first , then to jump link or others
                runJSCode(args.GetIsolate(), w2m(onlick_js));
            }
            
            // if addListener setted , run it !!
            if (curBrch->events.size()>0) {
                //printf("running event functions\n");
                
                EVENT_OBJS* cur = eventList;
                while (cur != NULL) {
                    
                    if (cur->p != curBrch) {
                        cur = cur->next;
                        continue;
                    }
                    
                    Local<Value> save_val = Local<Value>::New(args.GetIsolate(), cur->fv);
                    Persistent<Function> gs_save;
                    
                    if (save_val->IsFunction()){
                        printf("click listener callback_calling ============\n");
                        
                        Handle<Function> process_fun = Handle<Function>::Cast(save_val);
                        gs_save.Reset(args.GetIsolate(), process_fun);
                        // It is a function; cast it to a Function
                        v8::Local<v8::Function> process = v8::Local<v8::Function>::New(args.GetIsolate(), gs_save);
                        Handle<Value> result = process->Call(args.Holder(), 0 , NULL);
                        if (result.IsEmpty()) {
                            
                        }
                    }
                    
                    cur = cur->next;
                }
            }
            
            if (wcscmp(curBrch->brch_name, L"a") == 0) {
                wchar_t* href = curBrch->getAttribute((wchar_t*)L"href");
                if (href) {
                    
                }
            }
            else if (wcscmp(curBrch->brch_name, L"input") == 0) {
                
            }
            
        }
    }
    
    void ElementWrap_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        
        if (curBrch) {
            
            String::Utf8Value str1(args[0]);
            String::Utf8Value str2(args[1]);
            
            sp_debug("Element 【%ls】 addEventListener %s\n", curBrch->brch_name, *str1);
            sp_debug("func_body=======\n %s\n===========\n", *str2);
            
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
                    cur = cur->next;
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
    
    void ElementWrap_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        HTMLBranch* curBrch = getOriPtrBranch(args.Holder());
        
        if (curBrch) {
            String::Utf8Value name(args[0]);
            wchar_t* wname = m2w(*name);
            
            String::Utf8Value vv(args[1]);
            wchar_t* wvv = m2w(*vv);
            
            curBrch->setAttribute(wname, wvv);
        }
    }
    /** Image objects ============================================
     *
     */
    
    Local<Object> create_Image_obj(const FunctionCallbackInfo<Value>& args)
    {
        HTMLBranch* imgBrch = new HTMLBranch((wchar_t*)"img");
        
        
        Local<ObjectTemplate> templ = ObjectTemplate::New();
        templ->SetInternalFieldCount(1);
        
        // functions
        //templ->Set(iso, "open", FunctionTemplate::New(args.GetIsolate(), AjaxWrap_open));
        
        // set accessores
        
        //templ->SetAccessor(String::NewFromUtf8(args.GetIsolate(), "responseText"), AjaxWrap_responseText);
        //templ->SetAccessor(String::NewFromUtf8(args.GetIsolate(), "onreadystatechange"), AjaxWrap_onReadyStateChange, AjaxWrap_onReadyStateChangeSetter);
        
        Local<Object> result = templ->NewInstance();
        result->SetInternalField(0, External::New(Isolate::GetCurrent(), imgBrch));
        
        args.GetReturnValue().Set(result);
        return result;
    }
    
    void init_ImageWrap(Isolate* isolate, Handle<ObjectTemplate> global)
    {
        // Image wrap
        
        Handle<FunctionTemplate> templ = FunctionTemplate::New(isolate, Image_Constructor);
        templ->SetClassName(String::NewFromUtf8(isolate, "Image"));
        global->Set(String::NewFromUtf8(isolate, "Image"), templ);
        
    }
    
    void Image_Constructor(const FunctionCallbackInfo<Value>& args)
    {
        // cause in JS no host and port to init socket. so use global to here.
        EscapableHandleScope scope(args.GetIsolate());
        
        args.GetReturnValue().Set(create_Image_obj(args));
        
        // ======================================================
    }
    
}