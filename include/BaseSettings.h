#ifndef _BASESETTINGS_H_
#define _BASESETTINGS_H_

#include <string>
#include "json.hpp"

std::string Unicode2Ansi(const wchar_t* wcharBuff);
std::string Unicode2Ansi(const std::wstring & wstr);
std::wstring Ansi2Unicode(const char* charBuff);
std::wstring Ansi2Unicode(const std::string& str);

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

template<typename T>
    T BaseSettings::get(const char* key)
{
    return jData.at(key).get<T>();
}

template<typename T>
    T BaseSettings::get(const std::string& key)
{
    return jData.at(key).get<T>();
}

template<typename T>
    const T BaseSettings::get_ptr(const char* key)
{
    return jData.at(key).get_ptr<T>();
}

template<typename T>
    const T BaseSettings::get_ptr(const std::string& key)
{
    return jData.at(key).get_ptr<T>();
}

template<typename T>
    void BaseSettings::set(const char* key, T value)
{
    jData[key] = value;
    setUpdated();
}

template<class T>
    void BaseSettings::set(const std::string& key, T value)
{
    jData[key] = value;
    setUpdated();
}

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

