#ifndef CONFIG2_H
#define CONFIG2_H
#include "test.h"
#include "../config.h"
namespace test {

static void test_config() {
    std::string line;
    std::string key;
    std::string value;
    int err = 0;
    line = "key1:value1";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");

    line = "key1 : value1";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");

    line = "    key1 : value1   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");

    line = "    key1    : value1   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");


    line = "    key1    : \"value1\"   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");


    // key1  :   "value" "     //多一个”也会被清理
    line = "    key1    : \"value1\"\"   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_EQ(value, "value1");

    // key1  :   " value  " "     //多一个”也会被清理
    line = "    key1    : \" value1  \"\"   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_FALSE(err < 0);
    EXCEPT_EQ(key, "key1");
    EXCEPT_NE(value, "value1");
    EXCEPT_EQ(value, " value1  ");
   // EXCEPT_EQ();

    line = "key value";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_TRUE(err < 0);

    line = "key value:";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_TRUE(err < 0);

    line = "key value:      ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_TRUE(err < 0);

    line = "key:   \"\"   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_TRUE(err < 0);

    line = "key:   \"\"   ";
    err = kvstore::parse_line(line, key, value);
    DEBUG_INFO<<"line:"<<line;
    EXCEPT_TRUE(err < 0);
}

} //namespace

#endif // CONFIG2_H

