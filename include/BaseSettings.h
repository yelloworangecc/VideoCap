#ifndef _BASESETTINGS_H_
#define _BASESETTINGS_H_

#include <string>
#include "json.hpp"

std::string Unicode2Ansi(const wchar_t* wcharBuff);
std::string Unicode2Ansi(const std::wstring & wstr);
std::wstring Ansi2Unicode(const char* charBuff);
std::wstring Ansi2Unicode(const std::string& str);

/**B A S E   S E T T I N G S***********************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: Base class for 1) loading settings from json file 2) storaging settings 3) writing settings back to json file
 * Members: 
   - 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
class BaseSettings
{
public:
    BaseSettings(const char* fileName);
    BaseSettings(const std::string& fileName);
    ~BaseSettings();

    template<typename T>
        T get(const char* key);
    template<typename T>
        T get(const std::string& key);
    template<typename T>
        const T get_ptr(const char* key);
    template<typename T>
        const T get_ptr(const std::string& key);
    template<typename T>
        void set(const char* key, T value);
    template<typename T>
        void set(const std::string& key, T value);
    template<typename T>
        bool update(const char* key, T value);
    template<typename T>
        bool update(const std::string& key, T value);
    void erase(const char* key);
    void erase(const std::string& key);
    bool empty(){return jData.empty();}
    bool exist(const std::string& key){return jData.contains(key);}
    bool exist(const char* key){return jData.contains(key);}
    void print();

private:
    void load();
    void write();
    bool isUpdated(){return bUpdated;}
    void setUpdated(){bUpdated = true;}

    std::string fileName;
    nlohmann::json jData;
    bool bUpdated;
};
/**B A S E   S E T T I N G S . G E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: Get one setting value by key, if key not exist, an
                exception is throwed.
 * Parameters: 
   - const char* key
 * Return: template type
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    T BaseSettings::get(const char* key)
{
    return jData.at(key).get<T>();
}
/**B A S E   S E T T I N G S . G E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: Get one setting value by key, if key not exist, an
                exception is throwed.
 * Parameters: 
   - const std::string& key
 * Return: template type
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    T BaseSettings::get(const std::string& key)
{
    return jData.at(key).get<T>();
}
/**B A S E   S E T T I N G S . G E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: Get one setting value by key, if key not exist, an
                exception is throwed.
 * Parameters: 
   - const char* key
 * Return: template type
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    const T BaseSettings::get_ptr(const char* key)
{
    return jData.at(key).get_ptr<T>();
}
/**B A S E   S E T T I N G S . G E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: Get one setting value by key, if key not exist, an
                exception is throwed.
 * Parameters: 
   - const std::string& key
 * Return: template type
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    const T BaseSettings::get_ptr(const std::string& key)
{
    return jData.at(key).get_ptr<T>();
}
/**B A S E   S E T T I N G S . S E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022128
 * Description: add or update one setting with a key and a value
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    void BaseSettings::set(const char* key, T value)
{
    jData[key] = value;
    setUpdated();
}
/**B A S E   S E T T I N G S . S E T***************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022128
 * Description: add or update one setting with a key and a value
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<class T>
    void BaseSettings::set(const std::string& key, T value)
{
    jData[key] = value;
    setUpdated();
}

/**B A S E   S E T T I N G S . U P D A T E*********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: update one setting with a key and a value
 * Parameters: 
   - const char* key
   - T value
 * Return: bool, is key updated OK
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<typename T>
    bool BaseSettings::update(const char* key, T value)
{
    if(jData.contains(key)) 
    {
        jData[key] = value;
        setUpdated();
        return true;
    }
    return false;
}
/**B A S E   S E T T I N G S . U P D A T E*********************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022127
 * Description: update one setting with a key and a value
 * Parameters: 
   - const char* key
   - T value
 * Return: bool, is key updated OK
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
template<class T>
    bool BaseSettings::update(const std::string& key, T value)
{
    if(jData.contains(key))
    {
        jData[key] = value;
        setUpdated();
        return true;
    }
    return false;
}

#endif

