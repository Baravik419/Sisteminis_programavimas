#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// 1 uzduotis, naudojamas FindFirstFile

void SearchForMP3(const std::wstring& catalogue) {
    wstring searchPath = catalogue + L"\\*.mp3";
    WIN32_FIND_DATAW data;
    HANDLE searchKey = FindFirstFileW(searchPath.c_str(), &data);

    if (searchKey == INVALID_HANDLE_VALUE) {
        wcerr << L"No files found." << std::endl;
        return;
    }

    do {
        wcout << L"Found file: " << data.cFileName << std::endl;
    } while (FindNextFileW(searchKey, &data));

    FindClose(searchKey);
}




int main(){

    wstring catalogue = L"C:\\Users\\erwin\\Downloads";
    SearchForMP3(catalogue);
    return 0;
}
