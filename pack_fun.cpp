#include "string.h"
using namespace std;
const char* get_parameter_string(DWORD64 address, PROCESS_INFORMATION processInfo) {
    static char buffer[256];
    SIZE_T bytesRead;
    ReadProcessMemory(processInfo.hProcess, (LPCVOID)address, buffer, sizeof(buffer) - 1, &bytesRead);
    buffer[bytesRead] = '\0';
    return buffer;
}

int get_parameter_int(DWORD64 address, PROCESS_INFORMATION processInfo) {
    int value = 0;
    ReadProcessMemory(processInfo.hProcess, (LPCVOID)address, &value, sizeof(value), NULL);
    return value;
}
// 在 pythonf.cpp 中添加这些函数
PyObject* parse_pack_parameters(CONTEXT* context, PROCESS_INFORMATION processInfo) {
    PyObject *kwargs = PyDict_NewPtr();

    // 从寄存器读取参数标志
    DWORD64 param_flags = context->Rdx;

    // 检查并设置每个可能的参数
    if (param_flags & 0x01) { // side
        const char *side_value = get_parameter_string(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "side", PyUnicode_FromStringPtr(side_value));
        context->R8 += strlen(side_value) + 1; // 移动到下一个参数
    }

    if (param_flags & 0x02) { // fill
        const char *fill_value = get_parameter_string(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "fill", PyUnicode_FromStringPtr(fill_value));
        context->R8 += strlen(fill_value) + 1;
    }

    if (param_flags & 0x04) { // expand
        int expand_value = get_parameter_int(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "expand", PyLong_FromLongPtr(expand_value));
        context->R8 += sizeof(int);
    }

    if (param_flags & 0x08) { // anchor
        const char *anchor_value = get_parameter_string(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "anchor", PyUnicode_FromStringPtr(anchor_value));
        context->R8 += strlen(anchor_value) + 1;
    }

    if (param_flags & 0x10) { // padx
        int padx_value = get_parameter_int(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "padx", PyLong_FromLongPtr(padx_value));
        context->R8 += sizeof(int);
    }

    if (param_flags & 0x20) { // pady
        int pady_value = get_parameter_int(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "pady", PyLong_FromLongPtr(pady_value));
        context->R8 += sizeof(int);
    }

    if (param_flags & 0x40) { // ipadx
        int ipadx_value = get_parameter_int(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "ipadx", PyLong_FromLongPtr(ipadx_value));
        context->R8 += sizeof(int);
    }

    if (param_flags & 0x80) { // ipady
        int ipady_value = get_parameter_int(context->R8, processInfo);
        PyDict_SetItemStringPtr(kwargs, "ipady", PyLong_FromLongPtr(ipady_value));
        context->R8 += sizeof(int);
    }

    return kwargs;
}
std::string next_string(std::string s) {
    int n = s.length();
    int i = n - 1;

    // 从右向左找到第一个不是'z'的字符
    while (i >= 0 && s[i] == 'z') {
        s[i] = 'a';
        i--;
    }

    if (i >= 0) {
        // 如果找到了不是'z'的字符，将其递增
        s[i]++;
    } else {
        // 如果所有字符都是'z'，在开头添加一个'a'
        s = 'a' + s;
    }

    return s;
}
std::string get_fun_string(string name,string event_name){
    string ans="\n"
               "\n"
               "def ___"+name+"_():\n"
               "\n"
               "        # 发送消息给C++\n"
               "        message = \"c "+event_name+"\"\n"
               "        win32file.WriteFile(handle, message.encode())\n"
               "\n";
    return ans;
}