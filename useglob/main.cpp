#include <iostream>
#include <include/v8.h>
#include <include/libplatform/libplatform.h>

using namespace std;
using namespace v8;

#define V8STRING(s) String::NewFromUtf8(isolate, s)
#define RETURN_UNDEFINED(isolate) args.GetReturnValue().Set(Undefined( isolate ));

#define V_SET_METHOD(target, name, callb, isolate) target->Set(String::NewFromUtf8(isolate, name), FunctionTemplate::New( isolate, callb ));


Handle<String> ReadFile(Isolate* isolate, const string& name);

void printMessage(const FunctionCallbackInfo<Value>& args)
{
	HandleScope scope(args.GetIsolate());

	if (args.Length())
	{
		String::Utf8Value message(args[0]->ToString());
		if (message.length())
		{
			printf("%s\n", *message);
		}
	}

	RETURN_UNDEFINED(args.GetIsolate());
}

void getVersion(const FunctionCallbackInfo<Value>& args)
{
	HandleScope scope(args.GetIsolate());

	if (args.Length() >= 1) {
		Local<Function> callback = Local<Function>::Cast(args[0]);
		Local<Value> cbargs[] = { Number::New(args.GetIsolate(), 0.4) };
		callback->Call(callback, 1, cbargs);
	}

	RETURN_UNDEFINED(args.GetIsolate());
}

void greet(const FunctionCallbackInfo<Value>& args)
{
	HandleScope scope(args.GetIsolate());

	if (args.Length() >= 1) {
		String::Utf8Value name(args[0]->ToString());
		if (name.length())
		{
			printf("Hello %s!\n", *name);
		}
	}

	RETURN_UNDEFINED(args.GetIsolate());
}

int main (int argc, char *argv[])
{
	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
 	V8::InitializePlatform(platform);

	V8::Initialize();

	Isolate* isolate = Isolate::New();
	{
		Isolate::Scope isolate_scope(isolate);
		HandleScope handle_scope(isolate);

		Local<ObjectTemplate> global = ObjectTemplate::New(isolate);

		global->Set(String::NewFromUtf8(isolate, "version"), Number::New(isolate, 0), ReadOnly);
		global->Set(String::NewFromUtf8(isolate, "print"), FunctionTemplate::New(isolate, printMessage));
		global->Set(V8STRING("getVersion"), FunctionTemplate::New(isolate, getVersion));

		V_SET_METHOD(global, "greet", greet, isolate);

		Local<Context> context = Context::New(isolate, NULL, global);
		Context::Scope context_scope(context);
		Local<String> source = ReadFile(isolate, "/home/lestad/projects/t8v/useglob/javascript.js");

	    // Compile the source code.
	    Local<Script> script = Script::Compile(source);

	    // Run the script to get the result.
	    Local<Value> result = script->Run();

	    // Load variable from JS and convert to String
	    Handle<Value> vName = context->Global()->Get(String::NewFromUtf8(isolate,"name"));
	    String::Utf8Value sName(Handle<String>::Cast(vName));
	    //cout << "name: " << *sName << endl;


	    // Load variable from JS and convert to NUMBER(Integer, Float)
	    Handle<Value> vVersion = context->Global()->Get(String::NewFromUtf8(isolate, "v"));
	    Handle<Number> sVersion(Handle<Number>::Cast(vVersion));
	    //cout << "v: " << sVersion->Value() << endl;


	    // Get function from JavaScript
	    Handle<Value> vFunc = context->Global()->Get(V8STRING("__callee"));
	    Handle<Function> fnFunc = Handle<Function>::Cast(vFunc);

	    // Call function with args
	    const int args_length = 1;
	    Handle<Value> args[args_length] = { sVersion };
	    fnFunc->Call(fnFunc, args_length, args);

	    // Convert the result to an UTF8 string and print it.
	    //String::Utf8Value utf8(result);
	    //printf("%s\n", *utf8);
	}


	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	return 0;
}

Handle<String> ReadFile(Isolate* isolate, const string& name) {
	FILE* file = fopen(name.c_str(), "rb");
	if (file == NULL) {
		cout << "File: " << name.c_str() << " not found!" << endl;
		return Handle<String>();
	}

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
	int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
	i += read;
	}
	fclose(file);
	Handle<String> result =
	  String::NewFromUtf8(isolate, chars, String::kNormalString, size);
	delete[] chars;
	return result;
}
