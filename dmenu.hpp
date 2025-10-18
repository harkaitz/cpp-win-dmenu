#ifndef GUI_HPP
#define GUI_HPP

#include <windows.h>
#include <iostream>
#include <sstream>
#include <list>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <vector>

class CommandLine {
public:
    int  parseArguments();
    void help();
};

class Font {
public:
    inline ~Font() {};
    char *name = NULL;
    int   size = 0;
};

class Prompt {
public:
    inline ~Prompt() {}
    bool exists = false;
    std::wstring value;
    size_t length = 0;
};

class TextBox {
public:
    static HWND      hwnd;
    static long long hwnd_id;
    static WNDPROC   window_proc;
    static void createTextBox(HWND hwnd, int x, int y, int width, int height, char* fontName);
};

class ComboBox {
public:
    inline ~ComboBox() {}
    
    int  setLineNumber();
    void createComboBox(HWND hwnd, int x, int y, int width, int height, char* fontName);
    
    std::vector<std::string> items;
    HWND      hwnd = NULL;
    long long hwnd_id = 3 ;
    int       lineNumber;
    bool      caseSensitive ;

};

class Gui {
public:
    inline ~Gui() {};
    
    static Gui* instance();
    int initialize(const HINSTANCE hInstance, int iCmdShow);
    friend LRESULT CALLBACK WindowHandler(HWND, UINT, WPARAM, LPARAM, Gui&);
    bool isRunningCheck();
    void setupGui(HWND hwnd);
    
    HWND hwnd;
    int width;
    int height;
    
    std::wstring name;
    std::string version;
    
    TextBox textBox;

    void createLabel(HWND hwnd, int x, int y, int width, int height, char* fontName, std::wstring text);

};

extern Gui  gui;
extern Font font;
extern Prompt prompt;
extern ComboBox cb;

class Util {
public:
    static LPWSTR       charPtrToLpcwstr(const char* source);
    static std::wstring stringToWString(const std::string& str);
    static std::wstring stringToLpcwstr(std::string source);
    static int          stringToInt(std::string input);
    static void   setFont(HWND hwnd, char* fontName, int height, int width);
    static size_t findSubStrinInString(std::string data, std::string subString, size_t pos = 0);
};



#endif
