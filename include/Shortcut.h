#ifndef _SHORTCUT_H_
#define _SHORTCUT_H_
#include <string>
#include <windows.h>
#include <shobjidl.h>
#include <shlguid.h>
#include "Console.h"

class Shortcut
{
public:
    Shortcut():bInit(false)
    {
        CoInitialize(0);
        HRESULT hr;
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
        if(FAILED(hr))
        {
            CONSOLE<<"CoCreateInstance ShellLink Failed"<<ENDL;
            return;
        }
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
        if(FAILED(hr))
        {
            CONSOLE<<"QueryInterface PersistFile Failed"<<ENDL;
            return;
        }
        bInit = true;
    }
    ~Shortcut()
    {
        CoUninitialize();
    }

    void create(const wchar_t* targetPath, const wchar_t* targetName, const wchar_t* linkPath)
    {
        if (bInit)
        {
            std::wstring tPath(targetPath);
            tPath += targetName;
            CONSOLE<< tPath <<ENDL;
            pShellLink->SetPath(tPath.c_str());
            pShellLink->SetWorkingDirectory(targetPath);
            pShellLink->SetHotkey('K' | (HOTKEYF_CONTROL | HOTKEYF_ALT) << 8);
            
            std::wstring lPath(linkPath);
            lPath = lPath + targetName + L".lnk";
            CONSOLE<< lPath <<ENDL;
            pPersistFile->Save(lPath.c_str(), true);
        }
    }
private:
    IShellLink* pShellLink;
    IPersistFile* pPersistFile;
    bool bInit;
};

#endif
