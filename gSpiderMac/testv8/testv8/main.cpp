//
//  main.cpp
//  testv8
//
//  Created by reich on 14/11/18.
//  Copyright (c) 2014年 chupeng. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <v8.h>

#include <assert.h>
#include <wchar.h>

using namespace std;
using namespace v8;

/*****************************************************************
 *  exception functions
 */
const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
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

void runJSCode(v8::Handle<v8::Context> context, char* jscode)
{
    Local<String> source = String::NewFromUtf8(context->GetIsolate(), jscode);
    v8::Local<v8::String> name(v8::String::NewFromUtf8(context->GetIsolate(), "(tigerv8)"));
    v8::HandleScope handle_scope(context->GetIsolate());
    std::string gbkStr3 = executeString(context->GetIsolate(), source, name, true, true);
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


static void win_Callback(const v8::FunctionCallbackInfo<v8::Value>& args){
    
}

class HTMLWindow{
    
};

Handle<Object> WrapHTMLWindowObject(Isolate* isolate, HTMLWindow* htmlWin) {
    
    EscapableHandleScope handle_scope(isolate);
    
    Local<ObjectTemplate> templ = ObjectTemplate::New();
    templ->SetInternalFieldCount(1);
    
    Local<Object> result = templ->NewInstance();
    result->SetInternalField(0, External::New(isolate, htmlWin));
    
    return handle_scope.Escape(result);
}
/******************************************************************************
 *          main func start
 *
 ******************************************************************************/

int main(int argc, const char * argv[]) {
    
    
    Isolate* isolate = Isolate::GetCurrent();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    
    
    Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);
    
    v8::Handle<v8::FunctionTemplate> log_ft = v8::FunctionTemplate::New(isolate, log_Callback);
    log_ft->InstanceTemplate()->SetInternalFieldCount(1);
    global->Set(String::NewFromUtf8(isolate, "alert"), log_ft);
    
    
    Handle<Context> context = Context::New(isolate, NULL, global);
    
    v8::Handle<v8::Object> v8RealGlobal = v8::Handle<v8::Object>::Cast(context->Global()->GetPrototype());
    
    Context::Scope context_scope(context);
    
    HTMLWindow* htmWin = new HTMLWindow;
    
    Local<Object> jsWin = WrapHTMLWindowObject(isolate, htmWin);
    v8RealGlobal->Set(String::NewFromUtf8(isolate, "window"), jsWin);
    v8RealGlobal->SetPrototype(jsWin);
    
    
    // set global objects and functions
    
    runJSCode(context, (char*)"alert(window);window.asdf=44; alert(asdf);");
    
    std::cout << "********* v8 executed finished !! ********** \n";
    return 0;
}



