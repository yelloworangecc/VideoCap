#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <iostream>
#include <string>

//use this macro only once
#define CONSOLE_INIT() Console Console::instance

//access macro
#define CONSOLE Console::getInstance()

//output control macros
#define NARR "Console::narr"
#define WIDE "Console::wide"
#define ENDL "Console::endl"
#define DEC "Console::dec"
#define HEX "Console::hex"
#define OCT "Console::oct"

class Console
{
public:
    static Console& getInstance() { return instance; }
    
    void attach()
    {
        if(!GetStdHandle(STD_OUTPUT_HANDLE)) 
        {
            AttachConsole(ATTACH_PARENT_PROCESS);
            FILE * f;
            freopen_s(&f,"conout$","w",stdout);
        }
        
        bAttached = true;
    }

    //narrow type
    Console& operator<<(const char* str)
    {
        if (bAttached)
        {
            bWide = false;
            //special const char * string for output control
            bool skip = true;
            if (strcmp(str, WIDE) == 0) bWide = true;
            else if (strcmp(str, NARR) == 0) bWide = false;
            else if (strcmp(str, ENDL) == 0)
            {
                if (bWide) std::wcout << std::endl;
                else std::cout << std::endl;
            }
            else if (strcmp(str, DEC) == 0)
            {
                if (bWide) std::wcout << std::dec;
                else std::cout << std::dec;
            }
            else if (strcmp(str, HEX) == 0)
            {
                if (bWide) std::wcout << std::hex;
                else std::cout << std::hex;
            }
            else if (strcmp(str, OCT) == 0)
            {
                if (bWide) std::wcout << std::oct;
                else std::cout << std::oct;
            }
            else skip = false;
            if (skip) return *this;
       
            //other const char * string
            std::cout << str;
        }
        return *this;
    }
    //narrow type
    Console& operator<<(const std::string& str)
    {
        if (bAttached)
        {
            bWide = false;
            std::cout << str;
        }
        return *this;
    }
    //narrow type
    Console& operator<<(char str)
    {
        if (bAttached)
        {
            bWide = false;
            std::cout << str;
        }
        return *this;
    }

    //wide type
    Console& operator<<(const wchar_t* str)
    {
        if (bAttached)
        {
            bWide = true;
            std::wcout << str;
        }
        return *this;
    }
    //wide type
    Console& operator<<(const std::wstring& str)
    {
        if (bAttached)
        {
            bWide = true;
            std::wcout << str;
        }
        return *this;
    }
    //wide type
    Console& operator<<(wchar_t str)
    {
        if (bAttached)
        {
            bWide = true;
            std::wcout << str;
        }
        return *this;
    }
    //other common type
    template <class T> Console& operator<<(T str)
    {
        if (bAttached)
        {
            if(bWide) std::wcout << str;
            else std::cout << str;
        }
        return *this;
    }

    static Console instance;
    
private:
    Console():bAttached(false), bWide(false){}
    ~Console(){}
    Console(const Console& console){}
    Console& operator=(const Console& console){}
    
    bool bAttached;
    bool bWide;
};

#endif
