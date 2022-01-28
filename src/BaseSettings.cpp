#include <fstream>
#include <iostream>
#include <windows.h>
#include "BaseSettings.h"


#define WCHAR_CONVERT_MAX_LEN 1024

/**W I D E   C H A R  2  B Y T E  C H A R**********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 202229
 * Description: convert unicode to ansi, 
                because json lib only support std::string
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
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
/**B A S E   S E T T I N G S . E R A S E***********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022128
 * Description: Erase one setting by key
 * Parameters: 
   - const char* key
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void BaseSettings::erase(const char* key)
{
    if (jData.contains(key))
    {
        jData.erase(key);
        setUpdated();
    }
}
/**B A S E   S E T T I N G S . E R A S E***********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022128
 * Description: Erase one setting by key
 * Parameters: 
   - const std::string& key
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void BaseSettings::erase(const std::string& key)
{
    if (jData.contains(key))
    {
        jData.erase(key);
        setUpdated();
    }
}
/**B A S E   S E T T I N G S . P R I N T***********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 202228
 * Description: print serialized json string to standard output
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void BaseSettings::print()
{
    std::cout<<std::setw(2)<<jData<<std::endl;
}
/**B A S E   S E T T I N G S . L O A D*************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: load json object from file
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void BaseSettings::load()
{
    std::ifstream i(fileName.c_str());
    if (i) i >> jData;
}

/**B A S E   S E T T I N G S . W R I T E***********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: serialize json object to file
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void BaseSettings::write()
{
    std::ofstream o(fileName.c_str());
    if (o) o << std::setw(2) << jData << std::endl;
}

