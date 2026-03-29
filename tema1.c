#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey = NULL;
    int choice = 0;

    printf("Registry Root Keys:\n");
    printf("[1] HKEY_CLASSES_ROOT\n");
    printf("[2] HKEY_CURRENT_USER\n");
    printf("[3] HKEY_LOCAL_MACHINE\n");
    printf("[4] HKEY_USERS\n");
    printf("[5] HKEY_CURRENT_CONFIG\n");
    printf("Select a key index (1-5): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input format.\n");
        return 1;
    }

    switch (choice) {
        case 1: hKey = HKEY_CLASSES_ROOT; break;
        case 2: hKey = HKEY_CURRENT_USER; break;
        case 3: hKey = HKEY_LOCAL_MACHINE; break;
        case 4: hKey = HKEY_USERS; break;
        case 5: hKey = HKEY_CURRENT_CONFIG; break;
        default:
            printf("Invalid selection index. Execution terminated.\n");
            return 1;
    }

    char subKeyName[256];
    DWORD nameLength;
    DWORD index = 0;
    LONG lResult;

    printf("\nSubkeys:\n\n");

    while (1) {
        nameLength = 256;
        lResult = RegEnumKeyExA(hKey, index, subKeyName, &nameLength, NULL, NULL, NULL, NULL);

        if (lResult == ERROR_SUCCESS) {
            printf("%s\n", subKeyName);
            index++;
        } else if (lResult == ERROR_NO_MORE_ITEMS) {
            break;
        } else {
            printf("System Error Code: %ld\n", lResult);
            break;
        }
    }

    return 0;
}
