#include <iostream>
#include <windows.h>
#include <cstdint>
#include <string>
#include <memory>

#pragma pack(push, 1)
struct ID3v1Tag {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    uint8_t genre;
};
#pragma pack(pop)

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

// 2 Uzduotis

void ReadID3v1Tag(const wstring& filePath) {
    HANDLE file = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE) {
        wcerr << L"Unsuccessful file opening: " << filePath << endl;
        return;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(file, &fileSize);

    if (fileSize.QuadPart < 128) {
        wcerr << L"File too small, to have such tag." << endl;
        CloseHandle(file);
        return;
    }

    SetFilePointer(file, -128, NULL, FILE_END);

    ID3v1Tag tag;
    DWORD bytesRead;
    if (ReadFile(file, &tag, sizeof(tag), &bytesRead, NULL) && bytesRead == sizeof(tag)) {
        if (strncmp(tag.tag, "TAG", 3) == 0) {
            cout << "Dainos pavadinimas: " << string(tag.title, 30) << endl;
            cout << "Atlikejas: " << string(tag.artist, 30) << endl;
            cout << "Albumas: " << string(tag.album, 30) << endl;
            cout << "Metai: " << string(tag.year, 4) << endl;
        } else {
            cout << "ID3v1 zyma nerasta." << endl;
        }
    }

    CloseHandle(file);
}

int main(){

    wstring catalogue = L"C:\\Users\\erwin\\Downloads";
    SearchForMP3(catalogue);
    wstring file = L"C:\\Users\\erwin\\Downloads\\bensound-far.mp3";
    ReadID3v1Tag(file);
    return 0;
}
