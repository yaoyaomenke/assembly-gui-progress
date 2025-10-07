#include <windows.h>
#include <stdio.h>
typedef int Py_ssize_t;
class PyObject;
// Python C API ����ָ�����Ͷ���
typedef int (*Py_InitializeType)(void);
typedef void (*Py_FinalizeType)(void);
typedef PyObject* (*PyModule_GetDict)(PyObject*);
typedef void (*PyObject_SetAttrString)(PyObject*,const char*,PyObject*);
typedef PyObject* (*PyImport_ImportModuleType)(const char*);
typedef PyObject* (*PyImport_AddModule)(const char*);
typedef PyObject* (*PyObject_CallObjectType)(PyObject*, PyObject*);
typedef PyObject* (*PyObject_GetAttrString)(PyObject*, const char*);
typedef PyObject* (*PyBool_FromLong)(long long);
typedef PyObject* (*PyObject_CallMethodType)(PyObject*, const char*, const char*, ...);
typedef PyObject* (*PyObject_CallMethod)(PyObject*, const char*, const char *, ...);
typedef PyObject* (*PyTuple_NewType)(Py_ssize_t);
typedef int (*PyTuple_SetItemType)(PyObject*, Py_ssize_t, PyObject*);
typedef PyObject* (*PyUnicode_FromStringType)(const char*);
typedef PyObject* (*PyLong_FromLongType)(long);
typedef PyObject* (*PyDict_New)(void);
typedef PyObject* (*PyDict_GetItemString)(PyObject*,const char*);
typedef PyObject* (*PyDict_SetItemString)(PyObject *dp, const char *key, PyObject *item);
typedef void (*Py_DecRefType)(PyObject*);
typedef int (*PyRun_SimpleStringType)(const char*);
typedef PyObject* (*Py_BuildValueType)(const char*, ...);

// ����ָ�����
Py_InitializeType Py_InitializePtr = NULL;
PyBool_FromLong PyBool_FromLongPtr = NULL;
PyDict_SetItemString PyDict_SetItemStringPtr =NULL;
PyDict_GetItemString PyDict_GetItemStringPtr=NULL;
PyModule_GetDict PyModule_GetDictPtr=NULL;
PyObject_SetAttrString PyObject_SetAttrStringPtr = NULL;
PyImport_AddModule PyImport_AddModulePtr=NULL;
PyObject_GetAttrString PyObject_GetAttrStringPtr = NULL;
Py_FinalizeType Py_FinalizePtr = NULL;
PyImport_ImportModuleType PyImport_ImportModulePtr = NULL;
PyObject_CallObjectType PyObject_CallObjectPtr = NULL;
PyObject_CallMethodType PyObject_CallMethodPtr = NULL;
PyTuple_NewType PyTuple_NewPtr = NULL;
PyTuple_SetItemType PyTuple_SetItemPtr = NULL;
PyUnicode_FromStringType PyUnicode_FromStringPtr = NULL;
PyLong_FromLongType PyLong_FromLongPtr = NULL;
Py_DecRefType Py_DecRefPtr = NULL;
PyRun_SimpleStringType PyRun_SimpleStringPtr = NULL;
PyDict_New PyDict_NewPtr = NULL;
Py_BuildValueType Py_BuildValuePtr = NULL;
typedef PyObject* (*PyObject_CallType)(PyObject*, PyObject*, PyObject*);
PyObject_CallType PyObject_CallPtr = NULL;

// ����Python DLL
int LoadPythonDLL(const char* dllPath, const char* pydllPath) {
    HMODULE hPython = LoadLibraryA(dllPath);
    HMODULE hPythonDLL = LoadLibraryA(pydllPath);
    if (!hPython) {
        printf("�޷�����Python DLL: %s\n", dllPath);
        return 0;
    }

    // ��ȡ������ַ
    Py_InitializePtr = (Py_InitializeType)GetProcAddress(hPython, "Py_Initialize");
    Py_FinalizePtr = (Py_FinalizeType)GetProcAddress(hPython, "Py_Finalize");
    PyImport_ImportModulePtr = (PyImport_ImportModuleType)GetProcAddress(hPython, "PyImport_ImportModule");
    PyObject_CallObjectPtr = (PyObject_CallObjectType)GetProcAddress(hPython, "PyObject_CallObject");
    PyObject_CallMethodPtr = (PyObject_CallMethodType)GetProcAddress(hPython, "PyObject_CallMethod");
    PyTuple_NewPtr = (PyTuple_NewType)GetProcAddress(hPython, "PyTuple_New");
    PyTuple_SetItemPtr = (PyTuple_SetItemType)GetProcAddress(hPython, "PyTuple_SetItem");
    PyUnicode_FromStringPtr = (PyUnicode_FromStringType)GetProcAddress(hPython, "PyUnicode_FromString");
    PyLong_FromLongPtr = (PyLong_FromLongType)GetProcAddress(hPython, "PyLong_FromLong");
    Py_DecRefPtr = (Py_DecRefType)GetProcAddress(hPython, "Py_DecRef");
    PyRun_SimpleStringPtr = (PyRun_SimpleStringType)GetProcAddress(hPython, "PyRun_SimpleString");
    PyObject_GetAttrStringPtr = (PyObject_GetAttrString)GetProcAddress(hPython, "PyObject_GetAttrString");
    Py_BuildValuePtr = (Py_BuildValueType)GetProcAddress(hPython, "Py_BuildValue");
    PyImport_AddModulePtr = (PyImport_AddModule)GetProcAddress(hPython, "PyImport_AddModule");
    PyModule_GetDictPtr = (PyModule_GetDict)GetProcAddress(hPython, "PyModule_GetDict");
    PyDict_GetItemStringPtr=(PyDict_GetItemString)GetProcAddress(hPythonDLL, "PyDict_GetItemString");
    PyDict_SetItemStringPtr=(PyDict_SetItemString)GetProcAddress(hPythonDLL, "PyDict_SetItemString");
    PyDict_NewPtr=(PyDict_New)GetProcAddress(hPythonDLL, "PyDict_New");
    PyObject_CallPtr=(PyObject_CallType)GetProcAddress(hPythonDLL, "PyObject_Call");
    PyBool_FromLongPtr=(PyBool_FromLong)GetProcAddress(hPythonDLL, "PyBool_FromLong");

    if (!Py_InitializePtr || !Py_FinalizePtr || !PyImport_ImportModulePtr ||
        !PyObject_CallObjectPtr || !PyObject_CallMethodPtr || !PyTuple_NewPtr ||
        !PyTuple_SetItemPtr || !PyUnicode_FromStringPtr || !PyLong_FromLongPtr ||
        !Py_DecRefPtr || !PyRun_SimpleStringPtr || !PyObject_GetAttrStringPtr ||
        !Py_BuildValuePtr|| !PyImport_AddModulePtr|| !PyModule_GetDictPtr||
        PyDict_GetItemStringPtr|| !PyDict_SetItemStringPtr|| !PyDict_NewPtr
        || !PyObject_CallPtr|| !PyBool_FromLongPtr) {
        printf("�޷���ȡ��Ҫ��Python������ַ\n");
        FreeLibrary(hPython);
        return 0;
    }

    printf("Python DLL���سɹ�!\n");
    return 1;
}


//
//
//int main() {
//    printf("C�������Python Tkinter����GUI��ʾ\n");
//
//    // ����Python DLL
//    const char* pythonDllPath = "python39.dll";
//    if (!LoadPythonDLL(pythonDllPath)) {
//        const char* commonPaths[] = {
//                "E:\\python3.12\\python312.dll",
//                NULL
//        };
//
//        int loaded = 0;
//        for (int i = 0; commonPaths[i] != NULL; i++) {
//            printf("���Լ���: %s\n", commonPaths[i]);
//            if (LoadPythonDLL(commonPaths[i])) {
//                loaded = 1;
//                break;
//            }
//        }
//
//        if (!loaded) {
//            printf("�޷��ҵ�Python DLL����ȷ��Python�Ѱ�װ\n");
//            printf("��������˳�...\n");
//            getchar();
//            return 1;
//        }
//    }
//
//    // ����Ҫ����PyObject_Call����
//    HMODULE hPython = GetModuleHandleA("E:\\python3.12\\python312.dll");
//    if (!hPython) {
//        printf("�޷���ȡPythonģ����\n");
//        return 1;
//    }
//    PyObject_CallPtr = (PyObject_CallType)GetProcAddress(hPython, "PyObject_Call");
//    if (!PyObject_CallPtr) {
//        printf("�޷���ȡPyObject_Call������ַ\n");
//        return 1;
//    }
//
//    // ��ʼ��Python������
//    Py_InitializePtr();
//    printf("Python��������ʼ���ɹ�\n");
//
//    int choice;
//    printf("\nѡ����ʾ����:\n");
//    printf("1. ����GUIӦ�� (�Ƽ�)\n");
//    printf("2. �ֲ�����GUI\n");
//    printf("3. ��ʾ��Ϣ��\n");
//    printf("������ѡ�� (1-3): ");
//
//    if (scanf("%d", &choice) != 1) {
//        printf("�������\n");
//        Py_FinalizePtr();
//        return 1;
//    }
//
//
//    // ����Python������
//    Py_FinalizePtr();
//    printf("����ִ�����\n");
//    printf("��������˳�...\n");
//    getchar(); getchar();  // �ȴ��û�����
//
//    return 0;
//}