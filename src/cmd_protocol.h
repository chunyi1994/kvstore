#ifndef PROTOCOL
#define PROTOCOL
#include <cstddef>
#include <boost/asio.hpp>
#include <string>
namespace kvstore {

    
const int POS_TYPE = 0;
//request
const int POS_TOTAL_LENGTH = 1;
//get命令
const int POS_GET_KEY_LENGTH = POS_TOTAL_LENGTH + 2;
const int POS_GET_KEY = POS_GET_KEY_LENGTH + 2;
//use命令
const int POS_DB_ID = POS_TOTAL_LENGTH + 2;
//response
const int POS_MSG = POS_TOTAL_LENGTH + 2;
//type r
const int REQ_TYPE_PUT = 0x00;
const int REQ_TYPE_GET = 0x01;
const int REQ_TYPE_DEL = 0x02;
const int REQ_TYPE_USE = 0x03;
const int REQ_TYPE_LOGIN = 0x04;
const int REQ_TYPE_MULT = 0x05;
const int REQ_TYPE_EXEC = 0x06;
const int REQ_TYPE_DISCARD = 0x07;
const int REQ_TYPE_WATCH = 0x08;
const int RESP_TYPE_OK = 0x00;
const int RESP_TYPE_ERR = 0x01;


inline int get_pos_value(std::size_t key_len) {
    return POS_GET_KEY + static_cast<int>(key_len);
}


union Len {
    unsigned short us;
    char c[2];
};


//first_item 适用于只有一个值的请求
static int first_item(const std::string& str, std::string& key) {
    key = str.substr(3, str.size() - 3);
    return 0;
}

static unsigned short total_len(const std::string& msg) {
    unsigned short len = *((unsigned short*) (msg.data() + POS_TOTAL_LENGTH));
    len = boost::asio::detail::socket_ops::network_to_host_short(len);
    return len;
}
//get_key 和 get_value 适用于 put请求
static int get_key(const std::string& str, std::size_t total_len, std::string& key) {
    unsigned short key_len_s = *((unsigned short*) (str.data() + POS_GET_KEY_LENGTH));
    key_len_s = boost::asio::detail::socket_ops::network_to_host_short(key_len_s);
    std::size_t key_len = static_cast<std::size_t>(key_len_s);
    if (key_len + POS_GET_KEY > total_len ) {
        return -1;
    }
    key = str.substr(POS_GET_KEY, key_len);
    return 0;
}

static int  get_value(const std::string& str,std::size_t total_len,
        std::size_t key_len, std::string& value) {
    const int POS_VALUE = get_pos_value(key_len);
    std::size_t value_len = total_len - static_cast<std::size_t>(POS_VALUE);
    if (POS_VALUE == 0) {
        value = "";
    } else {
        value = str.substr(POS_VALUE, value_len);
    }
    return 0;
}

static std::string get_response(const std::string& msg, int type) {
    std::string resp = std::string{"xxx"}  + msg;
    Len len;
    len.us = boost::asio::detail::socket_ops::host_to_network_short(resp.size());
    resp[POS_TYPE] = type;
    resp[POS_TOTAL_LENGTH] = len.c[0];
    resp[POS_TOTAL_LENGTH + 1] = len.c[1];
    return resp;
}

static void set_total_len(std::string& msg) {
    Len len;
    len.us = static_cast<unsigned short> (msg.length());
    len.us = boost::asio::detail::socket_ops::host_to_network_short(len.us);
    msg[POS_TOTAL_LENGTH] = len.c[0];
    msg[POS_TOTAL_LENGTH + 1] = len.c[0 + 1];
}

static std::string get_use_request(std::size_t db) {
    std::string msg("xxxx");
    set_total_len(msg);
    msg[POS_DB_ID] = static_cast<unsigned char>(db);
    msg[POS_TYPE] = REQ_TYPE_USE;
    return msg;
}

static std::string get_put_request(std::string key, std::string value) {
    std::string msg = std::string("xxxxx") + key + value;
    msg[POS_TYPE] = REQ_TYPE_PUT;
    set_total_len(msg);
    Len len;
    len.us = static_cast<unsigned short> (key.length());
    len.us = boost::asio::detail::socket_ops::host_to_network_short(len.us);
    msg[POS_GET_KEY_LENGTH] = len.c[0];
    msg[POS_GET_KEY_LENGTH + 1] = len.c[0 + 1];
    return msg;
}

static std::string get_get_request(std::string key) {
    std::string msg = std::string("xxx") + key;
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_GET;
    return msg;
}

static std::string get_del_request(std::string key) {
    std::string msg = std::string("xxx") + key;
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_DEL;
    return msg;
}

static std::string get_login_request(std::string password) {
    std::string msg = std::string("xxx") + password;
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_LOGIN;
    return msg;
}

static std::string get_mult_request() {
    std::string msg("xxx");
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_MULT;
    return msg;
}

static std::string get_exec_request() {
    std::string msg("xxx");
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_EXEC;
    return msg;
}

static std::string get_discard_request() {
    std::string msg("xxx");
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_DISCARD;
    return msg;
}

static std::string get_watch_request(const std::string& key) {
    std::string msg = std::string("xxx") + key;
    set_total_len(msg);
    msg[POS_TYPE] = REQ_TYPE_WATCH;
    return msg;
}


}//namespace


//协议的长度2字节采用unsigned short， 一字节采用unsigned char
//客户端request

// --------------------------------------------------------------------------------------------------------
// | 请求类型   |    总 长 度      |   key长度  | key正文 + value 正文（如果有）|
// |  1byte        |     2bytes       |   2bytes    |  变长                                              |
//----------------------------------------------------------------------------------------------------------

//请求类型
//0x00  put
//0x01  get
//0x02 del
//0x03 use
//0x04 login
//0x05 mult
//0x06 exec

//get
// ----------------------------------------------------------------------
// | 请求类型   |    总 长 度         | key正文|
// |  1byte        |     2bytes          |  变长     |
//------------------------------------------------------------------------

//put
// ----------------------------------------------------------------------------------------
// | 请求类型   |    总 长 度      |   key长度  | key正文 + value正文|
// |  1byte        |     2bytes       |   2bytes    |             变 长              |
//----------------------------------------------------------------------------------------

//del
// -----------------------------------------------------------------------
// | 请求类型   |    总 长 度      |   key长度  | key正文|
// |  1byte        |     2bytes       |   2bytes    |  变长     |
//-----------------------------------------------------------------------

//use
// ----------------------------------------------------------
// | 请求类型   |    总 长 度      |   数据库id  |
// |  1byte        |     2bytes       |   1byte       |
//-----------------------------------------------------------

//watch
// -----------------------------------------------------------------------
// | 请求类型   |    总 长 度      |   key长度  | key正文|
// |  1byte        |     2bytes       |   2bytes    |  变长     |
//-----------------------------------------------------------------------

//login
// ----------------------------------------------------------
// | 请求类型   |    总 长 度         | 密码正文 |
// |  1byte        |     2bytes          |  变长       |
//----------------------------------------------------------


//discard

//服务端response、
// ----------------------------------------------------------
// | 答复类型   |  总长度          |   message    |
// |  1byte        |    2bytes        |   变长           |
//------------------------------------------------------------

//答复类型
// 0x00 ok
// 0x01 error



#endif // PROTOCOL

