import os

stub_dir = '/opt/stacks/zt-esp32/components/zerotier_node/port/stubs'
os.makedirs(f'{stub_dir}/prometheus', exist_ok=True)
os.makedirs(f'{stub_dir}/nlohmann', exist_ok=True)

with open(f'{stub_dir}/prometheus/simpleapi.h', 'w') as f:
    f.write("""\
#pragma once
#include <memory>
#include <string>
#include <map>
namespace prometheus {
class Registry {};
template<typename T> class Histogram;
template<typename T> struct BuilderT {
    BuilderT& Name(const std::string&) { return *this; }
    BuilderT& Help(const std::string&) { return *this; }
    BuilderT& Labels(const std::map<std::string,std::string>&) { return *this; }
    T& Register(Registry&) { static T t; return t; }
};
struct Counter { void Increment(double v=1.0) {} double Value() { return 0; } };
struct Gauge   { void Set(double) {} void Increment(double v=1.0) {} void Decrement(double v=1.0) {} double Value() { return 0; } };
template<> class Histogram<double> { public: void Observe(double) {} };
using counter_family_t = BuilderT<Counter>;
using gauge_family_t   = BuilderT<Gauge>;
using hist_family_t    = BuilderT<Histogram<double>>;
static inline BuilderT<Counter> BuildCounter() { return {}; }
static inline BuilderT<Gauge>   BuildGauge()   { return {}; }
static inline BuilderT<Histogram<double>> BuildHistogram() { return {}; }
namespace simpleapi {
    using gauge_metric_t   = Gauge;
    using counter_metric_t = Counter;
    using hist_metric_t    = Histogram<double>;
    using counter_family_t = BuilderT<Counter>;
    using gauge_family_t   = BuilderT<Gauge>;
    using hist_family_t    = BuilderT<Histogram<double>>;
    static inline BuilderT<Counter> BuildCounter() { return {}; }
    static inline BuilderT<Gauge>   BuildGauge()   { return {}; }
    static inline BuilderT<Histogram<double>> BuildHistogram() { return {}; }
}
} // namespace prometheus
""")

with open(f'{stub_dir}/prometheus/histogram.h', 'w') as f:
    f.write('#pragma once\n#include <prometheus/simpleapi.h>\n')

with open(f'{stub_dir}/nlohmann/json.hpp', 'w') as f:
    f.write("""\
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
""")

print('STUBS_OK')
