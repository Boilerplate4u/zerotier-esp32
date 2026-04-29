#pragma once
#include <string>
#include <map>
namespace nlohmann {
class json {
    std::string _str;
public:
    json() {}
    json(const std::string& s) : _str(s) {}
    json(const char* s) : _str(s ? s : "") {}
    template<typename T> T get() const { return T{}; }
    bool is_null() const { return true; }
    bool is_object() const { return false; }
    bool contains(const std::string&) const { return false; }
    json& operator[](const std::string&) { static json j; return j; }
    std::string dump(int=0) const { return _str; }
    static json parse(const std::string&) { return {}; }
    static json object() { return {}; }
};
template<> inline std::string json::get<std::string>() const { return _str; }
template<> inline int json::get<int>() const { return 0; }
template<> inline bool json::get<bool>() const { return false; }
} // namespace nlohmann
