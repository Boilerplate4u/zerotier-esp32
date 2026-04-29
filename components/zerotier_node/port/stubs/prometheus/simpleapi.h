#pragma once
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <cstdint>
namespace prometheus {
class Registry {};
struct SaveToFile { void enable(const std::string&) {} };
template<typename T> class Histogram;
template<typename T> struct BuilderT {
    BuilderT& Name(const std::string&) { return *this; }
    BuilderT& Help(const std::string&) { return *this; }
    BuilderT& Labels(const std::map<std::string,std::string>&) { return *this; }
    T& Register(Registry&) { static T t; return t; }
    // Add maste returnera en persistent referens - anvander static storage per anrop
    T& Add(const std::map<std::string,std::string>&) {
        static std::vector<T> storage;
        storage.emplace_back();
        return storage.back();
    }
    T& Add(const std::map<std::string,std::string>&, const std::vector<uint64_t>&) {
        static std::vector<T> storage;
        storage.emplace_back();
        return storage.back();
    }
};
struct Counter {
    void Increment(double v=1.0) {}
    Counter& operator+=(double v) { return *this; }
    Counter& operator+=(unsigned long v) { return *this; }
    Counter& operator+=(unsigned int v) { return *this; }
    Counter& operator++() { return *this; }
    Counter  operator++(int) { return *this; }
    double Value() { return 0; }
};
struct Gauge {
    void Set(double) {}
    void Increment(double v=1.0) {}
    void Decrement(double v=1.0) {}
    Gauge& operator=(double) { return *this; }
    Gauge& operator=(unsigned int) { return *this; }
    Gauge& operator=(unsigned short) { return *this; }
    Gauge& operator=(int) { return *this; }
    Gauge& operator+=(double v) { return *this; }
    Gauge& operator-=(double v) { return *this; }
    Gauge& operator++() { return *this; }
    Gauge  operator++(int) { return *this; }
    Gauge& operator--() { return *this; }
    Gauge  operator--(int) { return *this; }
    double Value() { return 0; }
};
template<typename T> class Histogram {
public:
    void Observe(double) {}
    Histogram& operator+=(double v) { return *this; }
};
template<typename T> using CustomFamily = BuilderT<T>;
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
