#ifndef TRANSACTION
#define TRANSACTION

#include <vector>
#include <map>
#include <string>
namespace kvstore {

class Transaction {
    public:
    Transaction() : cmds_(), watchs_(), mult_flag_(false) {}
    bool mult() const { return mult_flag_; }
    void set_mult(bool flag) { mult_flag_ = flag; }
    void add_cmd(const std::string& cmd) { cmds_.push_back(cmd); }
    void add_watch(const std::string& key, const std::string& value) { watchs_[key] = value; }
    void clear_cmd() { cmds_.clear(); }
    void clear_watch() { watchs_.clear(); }
    std::vector<std::string>& cmds() { return cmds_; }
    std::map<std::string, std::string> & watchs() { return watchs_; }
private:
    std::vector<std::string> cmds_;
    std::map<std::string, std::string> watchs_;
    bool mult_flag_;
};

} // namespace
#endif // TRANSACTION

