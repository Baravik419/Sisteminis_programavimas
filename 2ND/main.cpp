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

// 2 Uzduotis - naudojamas CreateFile

void ReadID3v1Tag(const wstring& filePath) {
    HANDLE file = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        wcerr << L"Unsuccessful file opening: " << filePath << endl;
        return;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(file, &fileSize);

    if (fileSize.QuadPart < 128) {
        wcerr << L"File too small to have a tag." << endl;
        CloseHandle(file);
        return;
    }

    SetFilePointer(file, -128, NULL, FILE_END);

    void* buffer = VirtualAlloc(NULL, sizeof(ID3v1Tag), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!buffer) {
        wcerr << L"Memory allocation failed." << endl;
        CloseHandle(file);
        return;
    }

    DWORD bytesRead;
    if (ReadFile(file, buffer, sizeof(ID3v1Tag), &bytesRead, NULL) && bytesRead == sizeof(ID3v1Tag)) {
        ID3v1Tag* tag = reinterpret_cast<ID3v1Tag*>(buffer);
        if (strncmp(tag->tag, "TAG", 3) == 0) {
            cout << "Title: " << string(tag->title, 30) << endl;
            cout << "Artist: " << string(tag->artist, 30) << endl;
            cout << "Album: " << string(tag->album, 30) << endl;
            cout << "Year: " << string(tag->year, 4) << endl;
        } else {
            cout << "ID3v1 tag not found." << endl;
        }
    }

    VirtualFree(buffer, 0, MEM_RELEASE);
    CloseHandle(file);
}

// 3 uzduotis - naudojamas WriteFile

void WriteID3v1Tag(const wstring& filePath) {
    string title, artist, album, year;
    cout << "Input title (max 30): ";
    getline(cin >> ws, title);
    cout << "Input artist (max 30): ";
    getline(cin, artist);
    cout << "Input album (max 30): ";
    getline(cin, album);
    cout << "Input year of the release (exact 4): ";
    getline(cin, year);

    unique_ptr<ID3v1Tag> tag(new ID3v1Tag());
    memset(tag.get(), 0, sizeof(ID3v1Tag)); // užpildome nuliais

    memcpy(tag->tag, "TAG", 3);
    strncpy(tag->title, title.c_str(), sizeof(tag->title) - 1);
    strncpy(tag->artist, artist.c_str(), sizeof(tag->artist) - 1);
    strncpy(tag->album, album.c_str(), sizeof(tag->album) - 1);
    strncpy(tag->year, year.c_str(), sizeof(tag->year) - 1);
    tag->genre = 0;

    HANDLE file = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        wcerr << L"Unsuccessful file opening: " << filePath << endl;
        return;
    }

    SetFilePointer(file, -128, NULL, FILE_END);

    DWORD bytesWritten;
    if (!WriteFile(file, tag.get(), sizeof(ID3v1Tag), &bytesWritten, NULL) || bytesWritten != sizeof(ID3v1Tag)) {
        wcerr << L"Unsuccessful writing of the tag." << endl;
    } else {
        wcout << L"Successfully wrote new ID3v1 tag." << endl;
    }

    CloseHandle(file);
}

int main(){

    wstring catalogue = L"C:\\Users\\erwin\\Downloads";
    SearchForMP3(catalogue);
    wstring file = L"C:\\Users\\erwin\\Downloads\\bensound-far.mp3";
    ReadID3v1Tag(file);
    WriteID3v1Tag(file);
    ReadID3v1Tag(file);
    return 0;
}
