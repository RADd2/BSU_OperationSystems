#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <cwchar>

using namespace std;

string wstringToString(const wstring& wstr) {
    if (wstr.empty()) return string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), 
                                          NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), 
                       &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

wstring stringToWstring(const string& str) {
    if (str.empty()) return wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void killById(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess != NULL) {
        if (TerminateProcess(hProcess, 0)) {
            cout << "[killer] : process " << pid << " successfully killed!" << endl;
        } else {
            DWORD error = GetLastError();
            cout << "[killer] : failed to kill process " << pid << "! Error code: " << error << endl;
        }
        CloseHandle(hProcess);
    } else {
        DWORD error = GetLastError();
        cout << "[killer] : process " << pid << " was not found or access denied! Error: " << error << endl;
    }
}

void killByName(const string& name) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "[killer] : failed to create process snapshot! Error: " << error << endl;
        return;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    wstring wname = stringToWstring(name);
    bool found = false;

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            wstring currentNameW = pe32.szExeFile;
            string currentName = wstringToString(currentNameW);
            
            if (_stricmp(currentName.c_str(), name.c_str()) == 0) {
                cout << "[killer] : found process " << name 
                      << " with pid " << pe32.th32ProcessID << endl;
                killById(pe32.th32ProcessID);
                found = true;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    } else {
        DWORD error = GetLastError();
        cout << "[killer] : failed to enumerate processes! Error: " << error << endl;
    }

    CloseHandle(hSnapshot);
    
    if (!found) {
        cout << "[killer] : no processes found with name: " << name << endl;
    }
}

void killByEnvVar() {
    char* envVar = getenv("PROC_TO_KILL");
    if (envVar != nullptr) {
        string envVarStr = envVar;
        cout << "[killer] : processing environment variable PROC_TO_KILL = " << envVarStr << endl;
        
        stringstream ss(envVarStr);
        string name;

        while (getline(ss, name, ',')) {
            name.erase(0, name.find_first_not_of(" \t\n\r\""));
            name.erase(name.find_last_not_of(" \t\n\r\"") + 1);
            
            if (!name.empty()) {
                cout << "[killer] : killing process: " << name << endl;
                killByName(name);
            }
        }
    } else {
        cout << "[killer] : PROC_TO_KILL environment variable not found." << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "[killer] : starting..." << endl;
    
    killByEnvVar();

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--id") {
            if (i + 1 < argc) {
                try {
                    DWORD pid = stoul(argv[++i]);
                    cout << "[killer] : killing process by id: " << pid << endl;
                    killById(pid);
                } catch (const exception& e) {
                    cout << "[killer] : invalid PID: " << argv[i] << " - " << e.what() << endl;
                }
            } else {
                cout << "[killer] : --id option requires an argument" << endl;
            }
        } else if (arg == "--name") {
            if (i + 1 < argc) {
                string name = argv[++i];
                cout << "[killer] : killing process by name: " << name << endl;
                killByName(name);
            } else {
                cout << "[killer] : --name option requires an argument" << endl;
            }
        } else if (arg == "--help" || arg == "-h") {
            cout << "Usage: killer.exe [OPTIONS]" << endl;
            cout << "Options:" << endl;
            cout << "  --id PID      Kill process by ID" << endl;
            cout << "  --name NAME   Kill process by name" << endl;
            cout << "  --help        Show this help message" << endl;
            cout << endl;
            cout << "Also kills processes listed in PROC_TO_KILL environment variable" << endl;
        } else {
            cout << "[killer] : unknown option: " << arg << endl;
            cout << "Use --help for usage information" << endl;
        }
    }

    cout << "[killer] : finished." << endl;
    return 0;
}