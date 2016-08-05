#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include<ctime>
namespace libevent{
static std::string getTime(){
    std::time_t t;
    std::time ( &t );
    std::string tStr = std::ctime(&t);
    //去掉回车
    tStr.erase(tStr.size() - 1, 1);
    return tStr;
}
static void debugPrint(const std::string & msg){
    std::cout<<getTime()<<" : "<<msg<<std::endl;
}

static void log(const std::string & msg){
    std::cout<<getTime()<<" : "<<msg<<std::endl;
}

static int toUpper(int c)
{
    return toupper(c);
}

static int toLower(int c)
{
    return tolower(c);
}

// string转小写
static std::string strToLower(const std::string &str)
{
    std::string strTmp = str;
    std::transform(strTmp.begin(),strTmp.end(),strTmp.begin(), toLower);
    return strTmp;
}

// string.compareNoCase
static bool compareNoCase(const std::string &strA,const std::string &strB)
{
    std::string str1 = strToLower(strA);
    std::string str2 = strToLower(strB);
    return (str1 == str2);
}

static bool isDigit(const std::string &str){
    if(str.length() == 0){
        return false;
    }
    if('0' > str[0] || '9'< str[0]){
        if(str[0] != '+' && str[0] != '-'){
            return false;
        }
    }
    for(size_t i = 1; i < str.length(); i++){
        if('0' <=str[i] && str[i] <= '9'){
            continue;
        }else{
            return false;
        }
    }

    return true;
}
static void stringChange(std::string &m){
    m = "\"" + m + "\"\n";
}

static int string2int(const std::string& s){
    if(s.length() == 0){
        return 0;
    }
    std::istringstream is(s);
    int ret = 0;
    is>>ret;
    return ret;
}

static std::string int2string(int i){
    std::stringstream is;
    is<<i;
    return is.str();
}

}
