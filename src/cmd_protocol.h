#ifndef PROTOCOL
#define PROTOCOL
#include <cstddef>
#include <boost/asio.hpp>
#include <string>
namespace kvstore {

const int POS_TYPE = 0;
//request
const int POS_TOTAL_LENGTH = 1;
const int POS_KEY_LENGTH = POS_TOTAL_LENGTH + 2;
const int POS_KEY = POS_KEY_LENGTH + 2;

//response
const int POS_MSG = POS_TOTAL_LENGTH + 2;

//type
const int REQ_TYPE_PUT = 0x00;
const int REQ_TYPE_GET = 0x01;
const int REQ_TYPE_DEL = 0x02;
const int RESP_TYPE_OK = 0x00;
const int RESP_TYPE_ERR = 0x01;

inline int get_pos_value(std::size_t key_len) {
    return POS_KEY + static_cast<int>(key_len);
}


union Len {
    unsigned short us;
    char c[2];
};

static int get_key(const std::string& str, std::size_t total_len, std::string& key) {
    unsigned short key_len_s = *((unsigned short*) (str.data() + POS_KEY_LENGTH));
    key_len_s = boost::asio::detail::socket_ops::network_to_host_short(key_len_s);
    std::size_t key_len = static_cast<std::size_t>(key_len_s);
    if (key_len + POS_KEY > total_len ) {  //1 是type 2是总len   第二个2是key len
        return -1;
    }
    key = str.substr(POS_KEY, key_len);
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

}//namespace

//客户端request

// --------------------------------------------------------------------------------------------------------
// | 请求类型   |    总 长 度      |   key长度  | key正文 + value 正文（如果有）|
// |  1byte        |     2bytes       |   2bytes    |  变长                                              |
//----------------------------------------------------------------------------------------------------------

//请求类型
//0x00  put
//0x01  get


//服务端response、
// ----------------------------------------------------------
// | 答复类型   |  总长度          |   message    |
// |  1byte        |    2bytes        |   变长           |
//------------------------------------------------------------

//答复类型
// 0x00 ok
// 0x01 error



#endif // PROTOCOL

