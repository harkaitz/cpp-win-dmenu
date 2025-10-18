#include "config.h"
#include "dmenu.hpp"

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <windowsx.h>
#include <commctrl.h>
#include <iostream>
#include <algorithm>

#ifndef GWL_HINSTANCE
#  define GWL_HINSTANCE -6
#endif


Gui gui;
Font font;
Prompt prompt;
ComboBox cb;
CommandLine commandLine;

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    return gui.initialize(hInstance, iCmdShow);
}

// -------------------------------------------------------------------
// ---- Command line arguments ---------------------------------------
// -------------------------------------------------------------------

void
CommandLine::help()
{
    printf("usage: dmenu [-iv] [-l lines] [-p prompt] [-fn font] [-fs fontsz]\n"
           "\n"
           "More info: " PROGRAM_HOMEPAGE "\n");
}

int
CommandLine::parseArguments()
{
    char   buffer[1024] = {0};
    int    exitFlag = 0;
    size_t argc = __argc;
    char** argv = __argv;

    cb.caseSensitive = true;

    for (size_t i = 0; i < argc; ++i) {
        
        // Print help.
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            help();
            SendMessage(gui.hwnd, WM_CLOSE, 0, 0);
            exitFlag = -1;
            break;
        }

        // Print version.
        if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
            printf("%s\n", gui.version.c_str());
            SendMessage(gui.hwnd, WM_CLOSE, 0, 0);
            exitFlag = -1;
            break;
        }

        // Line count.
        if (!strcmp(argv[i], "-l")) {
            cb.lineNumber = Util::stringToInt(argv[++i]);
            continue;
        }

        // Monitor (unimplemented)
        if (!strcmp(argv[i], "-m")) {
            i++;
            continue;
        }
        
        // Configure prompt.
        if (!strcmp(argv[i], "-p")) {
            prompt.exists = true;
            prompt.value = Util::stringToWString(argv[++i]);
            prompt.length = prompt.value.length();
            continue;
        } 

        // Configure font.
        if (!strcmp(argv[i], "-fn")) {
            font.name = argv[++i];
            continue;
        }
        if (!strcmp(argv[i], "-fs")) {
            font.size = (Util::stringToInt(argv[++i]));
            continue;
        }

        // Color (unimplemented).
        if (!strcmp(argv[i], "-nb") || !strcmp(argv[i], "-nf") || !strcmp(argv[1], "-sb")) {
            i++;
            continue;
        }

        // Windows ID (unimplemented)
        if (!strcmp(argv[i], "-w")) {
            i++;
            continue;
        }
        
        // Case insensitive.
        if (!strcmp(argv[i], "-i")) {
            cb.caseSensitive = false;
            continue;
        }

        // Appears at the bottom (unimplemented.)
        if (!strcmp(argv[i], "-b")) continue;
        
        // Grab the keyboard (unimplemented.)
        if (!strcmp(argv[i], "-f")) continue;
        
    }
    if (exitFlag == 0) {
         while (fgets(buffer, sizeof(buffer), stdin)) {
              char *nl = strchr(buffer, '\n');
              if (nl) *nl = '\0';
              cb.items.push_back(std::string(buffer));
         }
    }

    return exitFlag;
}

// -------------------------------------------------------------------
// ---- Main Window ( Initialization and event loop) -----------------
// -------------------------------------------------------------------

LRESULT CALLBACK
WindowHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, Gui& gui);

int
Gui::initialize(const HINSTANCE hInstance, int iCmdShow)
{
    name    = PROGRAM_NAME;
    version = PROGRAM_VERSION;
    width   = GetSystemMetrics(SM_CXSCREEN);
    height  = GetSystemMetrics(SM_CYSCREEN);
    
    INT Result = 0;
    if (!isRunningCheck()) {
        
        Result = commandLine.parseArguments();
        if (Result) {
            return Result;
        }
        MSG msg;
        WNDCLASS wndclass = { 0 } ;
        
        wndclass.cbClsExtra   = 0;
        wndclass.cbWndExtra   = 0;
        wndclass.lpszMenuName = NULL;
        wndclass.hInstance    = hInstance;
        wndclass.lpfnWndProc  = (WNDPROC) WindowHandler;
        wndclass.hCursor      = LoadCursor(NULL, IDC_ARROW);
        wndclass.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.style        = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszClassName = name.c_str();

        if (!RegisterClass(&wndclass)) {
            MessageBox(NULL, TEXT("This program requires Windows NT!"), name.c_str(), MB_ICONERROR);
            return -1;
        }

        hwnd = CreateWindow(name.c_str(),
                            name.c_str(),
                            WS_POPUP | WS_SYSMENU,
                            0,                                                    // initial x position
                            0,                                                    // initial y position
                            width,                                                // initial width
                            cb.items.size() > 0 ? height / 5 : 20, // initial height
                            NULL,                                                 // parent window handle
                            NULL,                                                 // window menu handle
                            hInstance,                                            // program instance handle
                            NULL);                                                // creation parameters
        
        if (NULL == hwnd) {
            DWORD error = ::GetLastError();
            std::string message = std::system_category().message(error);
            printf("error: %s", message.c_str());
            return -1;
        }

        setupGui(hwnd);
        ShowWindow(hwnd, iCmdShow);
        UpdateWindow(hwnd);

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            switch (msg.message) {
            case WM_KEYDOWN: {
                if ((GetAsyncKeyState(VK_ESCAPE) & 0x01)) {
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                }
            } break;
            }
        }
        Result = msg.wParam;
    }
    return Result;
}

LRESULT CALLBACK
WindowHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, Gui& gui)
{
    PAINTSTRUCT ps;
    HDC hdcStatic = (HDC)wParam;
    switch (message) {
    case WM_ACTIVATE:
    case WM_SETFOCUS:
    case WM_ENABLE:
    case WM_MOVE:
    case WM_FONTCHANGE:
        SetTextColor(hdcStatic, RGB(240, 216, 192));
        SetBkColor(hdcStatic, RGB(79, 78, 77));
        return (INT_PTR)CreateSolidBrush(RGB(107, 105, 99));
        break;
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == CBN_SELENDOK) {
            if (LOWORD(wParam) == cb.hwnd_id) {
                HWND hcombo = (HWND)lParam;
                LRESULT index = SendMessageW(hcombo, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                wchar_t buf[256];
                SendMessageW(hcombo, (UINT)CB_GETLBTEXT, (WPARAM)index, (LPARAM)buf);
                printf("%ls\n", buf);
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        if (HIWORD(wParam) == CBN_EDITUPDATE) {
            static WCHAR str[128];
            if (LOWORD(wParam) == cb.hwnd_id) {
                HWND hcombo = (HWND)lParam;
                GetWindowText(hcombo, str, 128);
                std::vector<std::string> _tempList;
                //ComboBox_SelectString(hwndComboBox, 0, L"sa");
                for (size_t i = 0; i < cb.items.size(); ++i) {
                    // Delete all items
                    std::wstring stemp = std::wstring(cb.items[i].begin(), cb.items[i].end());
                    LPCWSTR sw = stemp.c_str();
                    SendMessageW(cb.hwnd, CB_DELETESTRING, 0, (LPARAM)sw);
                    
                    // Add new items to vector
                    std::wstring ws(str);
                    std::string test(ws.begin(), ws.end());
                    
                    size_t pos = 0;
                    if (!cb.caseSensitive) {
                        pos = Util::findSubStrinInString(cb.items[i], test);
                    } else {
                        pos = cb.items[i].find(test, 0);
                    }
                    
                    if (std::string::npos != pos) {
                        _tempList.push_back(cb.items[i]);
                    }
                }
                for (size_t i = 0; i < _tempList.size(); ++i) {
                    std::wstring stemp = std::wstring(_tempList[i].begin(), _tempList[i].end());
                    LPCWSTR sw = stemp.c_str();
                    SendMessageW(cb.hwnd, CB_ADDSTRING, 0, (LPARAM)sw);
                }
                ComboBox_ShowDropdown(cb.hwnd, TRUE);
                //ComboBox_SetCurSel(hwndComboBox, 1);
            }
        }
        break;
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLOREDIT:
        SetTextColor(hdcStatic, RGB(240, 216, 192));
        SetBkColor(hdcStatic, RGB(79, 78, 77));
        return (INT_PTR)CreateSolidBrush(RGB(107, 105, 99));
        break;
    case WM_CTLCOLORSTATIC:
        SetTextColor(hdcStatic, RGB(240, 216, 192));
        SetBkColor(hdcStatic, RGB(79, 78, 77));
        return (INT_PTR)CreateSolidBrush(RGB(107, 105, 99));
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// -------------------------------------------------------------------
// ---- Main Window --------------------------------------------------
// -------------------------------------------------------------------

bool
Gui::isRunningCheck()
{
    std::wstring suffix = L"_Mutex";
    std::wstring fullName = name + suffix;
    CreateMutex(NULL, FALSE, fullName.c_str());
    return (ERROR_ALREADY_EXISTS == GetLastError());
}

void
Gui::setupGui(HWND m_hwnd)
{
    if (cb.items.size() > 0 && prompt.exists) {
        createLabel(m_hwnd, 0, 0, prompt.length*8, 155, font.name, prompt.value);
        cb.createComboBox(m_hwnd,
                          prompt.exists ? prompt.length * 8 : 0, 0,
                          width - (prompt.length * 8),
                          cb.lineNumber > 0 ? cb.setLineNumber() : 125,
                          font.name);
    } else if (cb.items.size() > 0) {
        cb.createComboBox(hwnd,
                          prompt.exists ? prompt.length * 8 : 0, 0,
                          prompt.exists ? (width - (prompt.length * 8)) : width,
                          cb.lineNumber > 0 ? cb.lineNumber : 125,
                          font.name);
    } else if (prompt.exists) {
        createLabel(m_hwnd, 0, 0, prompt.length*8, 125, font.name, prompt.value);
        textBox.createTextBox(m_hwnd,
                              prompt.exists ? prompt.length * 8 : 0, 0,
                              prompt.exists ? width - prompt.length : width,
                              200,
                              font.name);
    } else {
        textBox.createTextBox(m_hwnd,
                              prompt.exists ? prompt.length * 8 : 0, 0,
                              prompt.exists ? width - prompt.length : width,
                              200,
                              font.name);
    }
}

void
Gui::createLabel(HWND hwnd, int x, int y, int width, int height, char* fontName, std::wstring text)
{
    long long i = GetWindowLong (hwnd, GWL_HINSTANCE);
    HWND hwndPrompt = CreateWindow(L"static", NULL,
                                   WS_CHILD | WS_VISIBLE,
                                   x, y, width, height,
                                   hwnd, NULL,
                                   (HINSTANCE) i, NULL);
    if (font.name != NULL) {
        Util::setFont(hwndPrompt, fontName,
                      font.size>0 ? (font.size * 2) + (font.size / 2) : 20,
                      font.size>0 ? font.size : 8);
    }    
    SetWindowText(hwndPrompt, text.c_str());
}

// -------------------------------------------------------------------
// ---- Text Box -----------------------------------------------------
// -------------------------------------------------------------------

HWND      TextBox::hwnd = NULL;
long long TextBox::hwnd_id = 4;
WNDPROC   TextBox::window_proc = NULL;

static LRESULT CALLBACK
TextBoxWindowHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
     wchar_t buf[256];
     switch (msg) {
     case WM_KEYDOWN:
          switch (wParam) {
          case VK_RETURN:
               SendMessage(hwnd, (UINT)EM_GETLINE, (WPARAM)0, (LPARAM)buf);
               printf("%ls", buf);
               SendMessage(hwnd, WM_CLOSE, 0, 0);
               break;
          }
          break;
     case WM_CLOSE:
          PostQuitMessage(0);
          return 0;
          break;
     }
     return CallWindowProc(TextBox::window_proc, hwnd, msg, wParam, lParam);
}

void
TextBox::createTextBox(HWND hwnd, int x, int y, int width, int height, char* fontName)
{
    HWND hwndTextBox = CreateWindow
        (WC_EDIT, L"",
         WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VISIBLE | ES_WANTRETURN,
         x, y, width, height,
         hwnd, (HMENU)hwnd_id, NULL, NULL);
    if (font.name != NULL) {
        Util::setFont(hwndTextBox, fontName,
                      font.size>0 ? (font.size * 2) + (font.size / 2) : 20,
                      font.size>0 ? font.size : 8);
    }
    window_proc = (WNDPROC) SetWindowLongPtr(hwndTextBox, GWLP_WNDPROC, (LONG_PTR)TextBoxWindowHandler);
    SetFocus(hwndTextBox);
}

// -------------------------------------------------------------------
// ---- ComboBox -----------------------------------------------------
// -------------------------------------------------------------------

void
ComboBox::createComboBox(HWND m_hwnd, int x, int y, int width, int height, char* fontName)
{
    hwnd = CreateWindow(WC_COMBOBOX, L"",
                        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                        x, y, width, height,
                        m_hwnd, (HMENU)hwnd_id, NULL, NULL);

    if (font.name != NULL) {
        Util::setFont(hwnd,
                      fontName,
                      font.size>0 ? (font.size * 2) + (font.size / 2) : 20,
                      font.size>0 ? font.size : 8);
    }

    if (items.size()) {
        for (size_t i = 0; i < items.size(); ++i) {
            std::wstring stemp = std::wstring(items[i].begin(), items[i].end());
            LPCWSTR sw = stemp.c_str();
            SendMessageW(hwnd, CB_ADDSTRING, 0, (LPARAM)sw);
        }
        ComboBox_ShowDropdown(hwnd, TRUE);
        
        RECT rect;
        int comboHeight = 0;

        if(GetWindowRect(hwnd, &rect)) {
             // int comboWidth = rect.right - rect.left;
            comboHeight = rect.bottom - rect.top;
        }
        SetWindowPos(m_hwnd, NULL,
                     0, 0,
                     prompt.exists ? width + prompt.length * 8 : width,
                     comboHeight,
                     0);
        SetFocus(hwnd);
    } else {
        printf("There's no elmenets!");
    }
}

int
ComboBox::setLineNumber()
{
    int output = 0;
    if (1 == lineNumber) { output =  50; }
    else if (2 == lineNumber) { output =  85; }
    else if (3 == lineNumber) { output = 105; }
    else { output = 105 + ((lineNumber - 3) * 20); }
    return output;
}

// -------------------------------------------------------------------
// ---- Utilities ----------------------------------------------------
// -------------------------------------------------------------------

LPWSTR
Util::charPtrToLpcwstr(const char* source) {
    wchar_t wtext[500];
    mbstowcs(wtext, source, strlen(source) + 1);
    LPWSTR ptr = wtext;
    return ptr;
}

std::wstring
Util::stringToLpcwstr(std::string source) {
    std::wstring temp = std::wstring(source.begin(), source.end());
    return temp;
}

void
Util::setFont(HWND hwnd, char* fontName, int height, int width) {
    wchar_t wtext[400];
    mbstowcs(wtext, fontName, strlen(fontName) + 1);
    LPWSTR ptr = wtext;
    
    HFONT hFont = CreateFont(height, width,
                             0, 0,
                             FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, ptr);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, 0);
}

int
Util::stringToInt(std::string input) {
    int output;
    std::stringstream stream(input);
    stream >> output;
    return output;
}

size_t
Util::findSubStrinInString(std::string data, std::string subString, size_t pos) {
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    std::transform(subString.begin(), subString.end(), subString.begin(), ::tolower);
    return data.find(subString, pos);
}

std::wstring
Util::stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
