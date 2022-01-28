#include <gtest/gtest.h>
#include "BaseSettings.h"

TEST(BaseSettings, file_not_exist) {
  BaseSettings settings("../test/file_not_exist.json");
  EXPECT_TRUE(settings.empty());
  EXPECT_FALSE(settings.exist("any_key"));
}

TEST(BaseSettings, get) {
  BaseSettings settings("../test/test.json");
  int num = settings.get<int>("number");

  EXPECT_EQ(num, 101);
}

TEST(BaseSettings, get_ptr) {
  BaseSettings settings("../test/test.json");
  const std::string* pStr = settings.get_ptr<std::string*>("string");

  EXPECT_STREQ(pStr->c_str(), "hello");
}

TEST(BaseSettings, get_invalid_key) {
  BaseSettings settings("../test/test.json");
  EXPECT_THROW(int num = settings.get<int>("invalid"),nlohmann::json::out_of_range);
}

TEST(BaseSettings, set_erase) {
  BaseSettings settings("../test/test.json");
  settings.set("newInt",1001);
  int newInt = settings.get<int>("newInt");

  EXPECT_EQ(newInt, 1001);

  settings.erase("newInt");
}

TEST(BaseSettings, update) {
  BaseSettings settings("../test/test.json");
  //update an invalid key
  EXPECT_FALSE(settings.update("newInt",1001));
  EXPECT_THROW(int newInt = settings.get<int>("newInt"),nlohmann::json::out_of_range);
  //update a valid key, the change will be write to disk
  //next Test Case will check the file again and recover to origin
  EXPECT_TRUE(settings.update("number",102));
  int num = settings.get<int>("number");
  EXPECT_EQ(num, 102);
}

TEST(BaseSettings, update_for_recovery) {
  BaseSettings settings("../test/test.json");
  //get last test value
  int num = settings.get<int>("number");
  EXPECT_EQ(num, 102);
  //recovery to the original value
  EXPECT_TRUE(settings.update("number",101));
  num = settings.get<int>("number");
  EXPECT_EQ(num, 101);
}

TEST(BaseSettings,wchar_convert){
    wchar_t* pwstr = L"This is a wstring";
    std::string wstr_result = Unicode2Ansi(pwstr);
    EXPECT_TRUE(wstr_result.compare("This is a wstring") == 0);
    
    char* pStr = "This is a string";
    std::wstring str_result = Ansi2Unicode(pStr);
    EXPECT_TRUE(str_result.compare(L"This is a string") == 0);
}

