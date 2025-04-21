#include <iostream>
#include <windows.h>
#include <stdint.h>
#include <string.h>


using namespace  std;

static const char BASE64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

void showMenu();
void showErrorMessage(int code);
void showSystemInfo();
void base64Encode();
void findPrimeNum();

int main() {

    int choice;

    showMenu();
    cin >> choice;

    switch (choice) {
        case 1:
            double a, b, c;

            cin >> a >> b >> c;

            printf("%.2f\n", a);
            printf("%.1f\n", b);
            printf("%.2e\n", c);


            break;
        case 2:

            int e1, e2, e3;

            cin >> e1 >> e2 >> e3;

            showErrorMessage(e1);
            showErrorMessage(e2);
            showErrorMessage(e3);

            break;
        case 3:
            showSystemInfo();
            break;
        case 4:
            base64Encode();
            break;
        case 5:
            findPrimeNum();
            break;
        default:
            cout << "Invalid Choice" << endl;
    }


    return 0;
}

void showMenu() {
    system("cls");
    cout << "1. Formatting the entered number with printf." << endl;
    cout << "2. Displaying error messages using Windows API." << endl;
    cout << "3. Displaying system information." << endl;
    cout << "4. BASE64 computation for \"Secure Programming\" text." << endl;
    cout << "5. Finding prime numbers." << endl;
}

void showErrorMessage(int code) {
    LPVOID buffer = NULL;
    DWORD len = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer,
        0,
        NULL
        );

    if (len > 0) {
        printf("Message %d: %s\n", code, (char *)buffer);
        LocalFree(buffer);
    }
    else {
        printf("No such error exists\n");
    }

}

void showSystemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    printf("Number of Logical Processors: %d\n", si.dwNumberOfProcessors);
    printf("Page size: %d Bytes\n", si.dwPageSize);
    printf("Processor Mask: 0x%p\n", (PVOID)si.dwActiveProcessorMask);
    printf("Minimum process address: 0x%p\n", si.lpMinimumApplicationAddress);
    printf("Maximum process address: 0x%p\n", si.lpMaximumApplicationAddress);
}



char *base64_encode(const uint8_t *in, size_t in_len) {
    size_t out_len = 4 * ((in_len + 2) / 3);
    char *out = (char*)malloc(out_len + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (i < in_len) {
        uint32_t octet_a = i < in_len ? in[i++] : 0;
        uint32_t octet_b = i < in_len ? in[i++] : 0;
        uint32_t octet_c = i < in_len ? in[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        out[j++] = BASE64_table[(triple >> 18) & 0x3F];
        out[j++] = BASE64_table[(triple >> 12) & 0x3F];
        out[j++] = BASE64_table[(triple >> 6) & 0x3F];
        out[j++] = BASE64_table[triple & 0x3F];
    }

    for (size_t k = 0; k < (3 - in_len % 3) % 3; k++) {
        out[out_len - 1 - k] = '=';
    }

    out[out_len] = '\0';
    return out;
}

void base64Encode() {
    const char *text = "Secure Programming";
    char *encoded = base64_encode((const uint8_t *)text, strlen(text));
    if (encoded) {
        printf("%s\n", encoded);
        free(encoded);
    }

}

void findPrimeNum() {


    long long num;

    while ((cin >> num) && num != 0) {
        int count = 0;

        if (num <= 1)
            cout << "FALSE" << endl;
        else {

            for (int i = 2; i * i <= num; i++) {
                if (num % i == 0) {
                    count++;
                }
            }

            if (count > 2) {
                cout << "FALSE" << endl;
            }

            else
                cout << "TRUE" << endl;

        }
    }
}