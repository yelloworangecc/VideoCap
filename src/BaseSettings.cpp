#include <fstream>
#include <iostream>
#include <windows.h>
#include "BaseSettings.h"


#define WCHAR_CONVERT_MAX_LEN 1024

std::string Unicode2Ansi(const wchar_t* wcharBuff)
{
    int len;
    char charBuff[WCHAR_CONVERT_MAX_LEN];

    len = WideCharToMultiByte(CP_ACP, 0, wcharBuff, -1, NULL, 0, NULL, NULL);
    if (len == 0) return "";
    
    WideCharToMultiByte(CP_ACP, 0, wcharBuff, -1, charBuff, len, NULL, NULL);
    charBuff[len] = '\0';
    return charBuff;
}

std::string Unicode2Ansi(const std::wstring & wstr)
{
    return Unicode2Ansi(wstr.c_str());
}

std::wstring Ansi2Unicode(const char* charBuff)
{
    int len;
    wchar_t wcharBuff[WCHAR_CONVERT_MAX_LEN];
    len = MultiByteToWideChar(CP_ACP, 0, charBuff, -1, NULL, 0);

    if (len == 0) return L"";
    MultiByteToWideChar(CP_ACP, 0, charBuff, -1, wcharBuff, len);
    wcharBuff[len] = L'\0';
    return wcharBuff;
}

std::wstring Ansi2Unicode(const std::string& str)
{
    return Ansi2Unicode(str.c_str());
}


BaseSettings::BaseSettings(const char* _fileName)
    :fileName(_fileName),bUpdated(false)
{
    load();
}

BaseSettings::BaseSettings(const std::string& _fileName)
    :fileName(_fileName)
{
    load();
}

BaseSettings::~BaseSettings()
{
    if (isUpdated()) write();
}

void BaseSettings::erase(const char* key)
{
    if (jData.contains(key))
    {
        jData.erase(key);
        setUpdated();
    }
}

void BaseSettings::erase(const std::string& key)
{
    if (jData.contains(key))
    {
        jData.erase(key);
        setUpdated();
    }
}

void BaseSettings::print()
{
    std::cout<<std::setw(2)<<jData<<std::endl;
}

void BaseSettings::load()
{
    std::ifstream i(fileName.c_str());
    if (i) i >> jData;
}

void BaseSettings::write()
{
    std::ofstream o(fileName.c_str());
    if (o) o << std::setw(2) << jData << std::endl;
}

