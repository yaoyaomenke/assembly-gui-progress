#include <windows.h>
#include "cjvm/main.cpp"
#include "iostream"
#include "map"
#include <thread>
#include "pack_fun.cpp"
#include <string>
#include "queue"
using namespace std;
PROCESS_INFORMATION processInfo;
queue<string> window_event_lst;
string fun_name="a";
bool windowStart=false;
PyObject* tk=NULL;
void deal_int(CONTEXT*  context,int i,PROCESS_INFORMATION processInfo,DEBUG_EVENT debugEvent);
bool cach_debug_event(){
    DEBUG_EVENT debugEvent;
    WaitForDebugEvent(&debugEvent, 1);

    if (debugEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
        // 目标程序已退出，记录退出码
        DWORD exitCode = debugEvent.u.ExitProcess.dwExitCode;
        return 1;
    }

    // 处理其他事件（如INT指令、线程创建等）
    if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
        // 假设已获取目标进程句柄 hProcess，且 exceptionAddress 有效
        BYTE opCode[2];  // 存储读取的机器码（至少2字节）
        SIZE_T bytesRead;
        if (ReadProcessMemory(processInfo.hProcess,debugEvent.u.Exception.ExceptionRecord.ExceptionAddress,opCode,2,&bytesRead)){
            if (bytesRead == 2 && opCode[0] == 0xCD) {
                CONTEXT ctx;
                ZeroMemory(&ctx, sizeof(CONTEXT));
                ctx.ContextFlags = CONTEXT_ALL;
                if (GetThreadContext(processInfo.hThread, &ctx)) {
                    deal_int(&ctx,(short)opCode[1],processInfo,debugEvent);
                    SetThreadContext(processInfo.hThread, &ctx);
                } else {
                    cout<<"GetThreadContext Error"<<endl;
                }
            }
        }
    }

    // 通知目标进程继续执行
    ContinueDebugEvent(
            debugEvent.dwProcessId,
            debugEvent.dwThreadId,
            DBG_CONTINUE
    );
    return 0;
}
void deal_gui_event(){
    HANDLE hPipe;
    char buffer[1024];
    DWORD dwRead, dwWritten;

    // 创建命名管道
    hPipe = CreateNamedPipe(
            (LPCSTR)"\\\\.\\pipe\\rthtyjyuPipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            1024,
            1024,
            0,
            NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        exit(-9);
    }
    DWORD bytesAvailable = 0;
    ConnectNamedPipe(hPipe, NULL);
    while (true){
        PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL);
        if(bytesAvailable>0){
            ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
            buffer[dwRead] = '\0';
            std::string str = buffer;
            window_event_lst.push(str);
        }
        if(!windowStart)continue;
        if(cach_debug_event()){
            return;
        }
    }
}
string read_process_cstring(long long address, PROCESS_INFORMATION processInfo){
    string ans="";
    while (true){
        u_char c=0;
        ReadProcessMemory(processInfo.hProcess, (LPCVOID)address, &c, sizeof(c), NULL);
        if (c=='\0'){
            return ans;
        }
        ans.push_back(c);
        address++;
    }
}
class python_gui_frame{
public:
    PyObject *args;
    PyObject *kwargs;
    PyObject* frameClass;
};
python_gui_frame new_python_gui_frame(string name, PyObject* parent_window){
    PyObject *tkModule = PyImport_ImportModulePtr("tkinter.ttk");
    PyObject *frameClass = PyObject_GetAttrStringPtr(tkModule, name.c_str());
    PyObject *args = PyTuple_NewPtr(1);
    PyTuple_SetItemPtr(args, 0,parent_window);
    PyObject* kwargs = PyDict_NewPtr();
    python_gui_frame a;
    a.args=args;
    a.kwargs=kwargs;
    a.frameClass=frameClass;
    return a;
}
void deal_int(CONTEXT*  context,int i,PROCESS_INFORMATION processInfo,DEBUG_EVENT debugEvent){
    context->Rip += 2;
    switch (i) {
        case 10:{
            printf("a");
            break;
        }
        case 20:{
            context->Rax=MessageBoxA(NULL, read_process_cstring(context->Rcx,processInfo).c_str(), read_process_cstring(context->Rdx,processInfo).c_str(), context->R8);
            break;
        }
        case 21:{
            PyObject *tkModule = PyImport_ImportModulePtr("tkinter");
            PyObject *tkClass = PyObject_GetAttrStringPtr(tkModule, "Tk");
            PyObject *tkInstance = PyObject_CallObjectPtr(tkClass, NULL);
            context->Rax = (long long)tkInstance;
            PyObject* mainModule = PyImport_AddModulePtr("__main__");
            PyObject* globals = PyModule_GetDictPtr(mainModule);
            PyObject* fun_object = PyDict_GetItemStringPtr(globals, "___a_");
            PyObject_CallMethodPtr(tkInstance, "protocol", "(sO)", PyUnicode_FromStringPtr("WM_DELETE_WINDOW"),fun_object);
            break;
        }
        case 22: {
            PyObject_CallMethodPtr(reinterpret_cast<PyObject *>(context->Rcx), "geometry", "(s)", read_process_cstring(context->Rdx, processInfo).c_str());
            break;
        }
        case 23:{
            tk=reinterpret_cast<PyObject *>(context->Rcx);
            windowStart=true;
            break;
        }
        case 24:{
            PyObject_CallMethodPtr(reinterpret_cast<PyObject *>(context->Rcx), "title", "(s)", read_process_cstring(context->Rdx, processInfo).c_str());
            break;
        }
        case 25:{
            python_gui_frame j=new_python_gui_frame("Frame", reinterpret_cast<PyObject *>(context->Rcx));
            PyObject *args = j.args;
            PyObject *kwargs = j.kwargs;
            PyDict_SetItemStringPtr(kwargs, "padding", PyUnicode_FromStringPtr(read_process_cstring(context->Rdx, processInfo).c_str()));
            PyObject *frameClass = j.frameClass;
            context->Rax= reinterpret_cast<DWORD64>(PyObject_CallPtr(frameClass, args, kwargs));
            Py_DecRefPtr(args);
            Py_DecRefPtr(kwargs);
            Py_DecRefPtr(frameClass);
            break;
        }
        case 26:{
            python_gui_frame j=new_python_gui_frame("Button", reinterpret_cast<PyObject *>(context->Rcx));
            PyObject *args = j.args;
            PyObject *kwargs = j.kwargs;
            PyObject *buttonClass = j.frameClass;
            PyDict_SetItemStringPtr(kwargs, "text", PyUnicode_FromStringPtr(read_process_cstring(context->Rdx, processInfo).c_str()));
            if(context->R8!=0){
                fun_name=next_string(fun_name);
                string fun_string=get_fun_string(fun_name, read_process_cstring(context->R8, processInfo));
                PyRun_SimpleStringPtr(fun_string.c_str());
                PyObject* mainModule = PyImport_AddModulePtr("__main__");
                PyObject* globals = PyModule_GetDictPtr(mainModule);
                PyObject* fun_object = PyDict_GetItemStringPtr(globals, ("___"+fun_name+"_").c_str());
                PyDict_SetItemStringPtr(kwargs, "command",fun_object);
            }
            context->Rax= reinterpret_cast<DWORD64>(PyObject_CallPtr(buttonClass, args, kwargs));
            break;
        }
            // 在 deal_int 函数中添加 case 27
        case 27:{
            // 解析 pack 参数并调用 pack 方法
            PyObject* kwargs = parse_pack_parameters(context, processInfo);
            PyObject_CallMethodPtr(reinterpret_cast<PyObject*>(context->Rcx), "pack", "(O)", kwargs);
            Py_DecRefPtr(kwargs);
            break;
        }
        case 28:{
            if(window_event_lst.empty()){context->Rax =0;
                break;}
            LPVOID remoteMemory = VirtualAllocEx(processInfo.hProcess, NULL, window_event_lst.front().size(), MEM_COMMIT | MEM_RESERVE, PAGE_READONLY);
            WriteProcessMemory(processInfo.hProcess, remoteMemory, window_event_lst.front().c_str(), window_event_lst.front().size(), NULL);
            window_event_lst.pop();
            context->Rax = (unsigned __int64)remoteMemory;
            break;
        }
        case 29:{
            //传入一个tk窗口的PyObject,销毁这个窗口。
            PyObject_CallMethodPtr(reinterpret_cast<PyObject *>(context->Rcx), "destroy", "()");
            break;
        }
        case 30:{
            //传入一个tk窗口的PyObject,返回这个窗口的父窗口。
            PyObject* parent = PyObject_GetAttrStringPtr(reinterpret_cast<PyObject *>(context->Rcx), "master");
            context->Rax = (unsigned __int64)parent;
            break;
        }
        case 31:{
            python_gui_frame j=new_python_gui_frame("Label", reinterpret_cast<PyObject *>(context->Rcx));
            PyObject *args = j.args;
            PyObject *kwargs = j.kwargs;
            PyObject *buttonClass = j.frameClass;
            PyDict_SetItemStringPtr(kwargs, "text", PyUnicode_FromStringPtr(read_process_cstring(context->Rdx, processInfo).c_str()));
            PyDict_SetItemStringPtr(kwargs, "font", PyLong_FromLongPtr(context->R8));
            context->Rax= reinterpret_cast<DWORD64>(PyObject_CallPtr(buttonClass, args, kwargs));
            break;
        }
        case 32:{
            python_gui_frame j=new_python_gui_frame("Text", reinterpret_cast<PyObject *>(context->Rcx));
            PyObject *args = j.args;
            PyObject *kwargs = j.kwargs;
            PyObject *buttonClass = j.frameClass;
            PyDict_SetItemStringPtr(kwargs, "width", PyLong_FromLongPtr(get_parameter_int(context->Rdx, processInfo)));
            PyDict_SetItemStringPtr(kwargs, "height", PyLong_FromLongPtr(get_parameter_int(context->R8, processInfo)));
            context->Rax= reinterpret_cast<DWORD64>(PyObject_CallPtr(buttonClass, args, kwargs));
            break;
        }
        case 33:{
            PyObject_CallMethodPtr(reinterpret_cast<PyObject *>(context->Rcx), "resizable", "(OO)", PyBool_FromLongPtr(context->Rdx), PyBool_FromLongPtr(context->R8));
            break;
        }
        case 34:{
            PyObject_CallMethodPtr(reinterpret_cast<PyObject *>(context->Rcx), "iconbitmap", "(O)", PyUnicode_FromStringPtr(read_process_cstring(context->Rdx, processInfo).c_str()));
            break;
        }
    }
}
int main(int argc, char *argv[]){
    if(argc<3){
        return 0;
    }
    STARTUPINFOA startupInfo;
    CreateProcessA(
            NULL,               // 程序路径
            argv[1], // 命令行
            NULL, NULL, FALSE,
            DEBUG_PROCESS|PROCESS_VM_WRITE|PROCESS_VM_OPERATION,      // 关键标志：将新进程纳入调试状态
            NULL, NULL,
            &startupInfo, &processInfo
    );
    LoadPythonDLL(argv[2],argv[3]);
    Py_InitializePtr();
    thread t1(deal_gui_event);
    PyRun_SimpleStringPtr("import tkinter as tk");
    PyRun_SimpleStringPtr("from tkinter import *");
    PyRun_SimpleStringPtr( "import win32file\n"
                           "handle = win32file.CreateFile(\n"
                           "r\"\\\\.\\pipe\\rthtyjyuPipe\",\n"
                           "            win32file.GENERIC_READ | win32file.GENERIC_WRITE,\n"
                           "            0,\n"
                           "            None,\n"
                           "            win32file.OPEN_EXISTING,\n"
                           "            0,\n"
                           "            None\n"
                           "        )\n");
    PyRun_SimpleStringPtr("\n"
                          "\n"
                          "def ___a_():\n"
                          "        # 发送消息给C++\n"
                          "        message = \"c close\"\n"
                          "        win32file.WriteFile(handle, message.encode())\n"
                          "\n");
    while (true) {
        if(cach_debug_event()!=0){
            return 1;
        }
        if(windowStart){
            PyObject_CallMethodPtr(tk, "update", "()");
        }
    }
}