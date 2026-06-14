#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#pragma comment(lib, "advapi32.lib")


std::wstring GetServiceDll(const std::wstring& serviceName) {
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName + L"\\Parameters";
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return L"";

    wchar_t dllPath[MAX_PATH] = {};
    DWORD size = sizeof(dllPath);
    DWORD type = 0;
    RegQueryValueExW(hKey, L"ServiceDll", NULL, &type, (LPBYTE)dllPath, &size);
    RegCloseKey(hKey);

    return std::wstring(dllPath);
}

int main() {
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCM) return 1;

    DWORD bytesNeeded = 0, servicesReturned = 0, resumeHandle = 0;

    EnumServicesStatusExW(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE,
                          NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle, NULL);

    ENUM_SERVICE_STATUS_PROCESSW* services = (ENUM_SERVICE_STATUS_PROCESSW*)new BYTE[bytesNeeded];
    resumeHandle = 0;

    if (!EnumServicesStatusExW(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE,
                               (LPBYTE)services, bytesNeeded, &bytesNeeded,
                               &servicesReturned, &resumeHandle, NULL)) {
        delete[] (BYTE*)services;
        CloseServiceHandle(hSCM);
        return 1;
    }

   
    std::map<std::wstring, std::vector<std::wstring>> dllUsers;

    for (DWORD i = 0; i < servicesReturned; ++i) {
        std::wstring svcName = services[i].lpServiceName;
        std::wstring dll = GetServiceDll(svcName);

        if (!dll.empty())
            dllUsers[dll].push_back(svcName);
    }


    std::wcout << L"=== Services with a ServiceDll ===\n\n";
    for (auto& [dll, users] : dllUsers) {
        std::wcout << dll << L"\n";
        for (auto& svc : users)
            std::wcout << L"  - " << svc << L"\n";
        std::wcout << L"\n";
    }

    std::wcout << L"=== DLLs shared by MULTIPLE services ===\n\n";
    bool found = false;
    for (auto& [dll, users] : dllUsers) {
        if (users.size() > 1) {
            found = true;
            std::wcout << dll << L"  (" << users.size() << L" services)\n";
            for (auto& svc : users)
                std::wcout << L"  - " << svc << L"\n";
            std::wcout << L"\n";
        }
    }
    if (!found)
        std::wcout << L"  None found.\n";

    delete[] (BYTE*)services;
    CloseServiceHandle(hSCM);
    return 0;
}
