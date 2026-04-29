// ESP-IDF stub: full prometheus replacement
#pragma once
#include <string>
#include <map>

namespace prometheus {

struct Gauge   { void Set(double) {} void Increment(double=1.0) {} void Decrement(double=1.0) {} double Value() { return 0; } };
struct Counter { void Increment(double=1.0) {} double Value() { return 0; } };
struct Histogram { void Observe(double) {} };
struct Summary   { void Observe(double) {} };

template<typename T> struct Family {
    T& Add(const std::map<std::string,std::string>&) { static T t; return t; }
};

template<typename T> struct BuilderT {
    BuilderT& Name(const std::string&) { return *this; }
    BuilderT& Help(const std::string&) { return *this; }
    BuilderT& Labels(const std::map<std::string,std::string>&) { return *this; }
    Family<T>& Register(void*) { static Family<T> f; return f; }
};

static inline BuilderT<Counter>   BuildCounter()   { return {}; }
static inline BuilderT<Gauge>     BuildGauge()     { return {}; }
static inline BuilderT<Histogram> BuildHistogram() { return {}; }
static inline BuilderT<Summary>   BuildSummary()   { return {}; }

namespace simpleapi {
    using gauge_metric_t   = Gauge;
    using counter_metric_t = Counter;
    using hist_metric_t    = Histogram;
    using summary_metric_t = Summary;
    static inline Gauge&   make_gauge(const char*, const char*) { static Gauge g; return g; }
    static inline Counter& make_counter(const char*, const char*) { static Counter c; return c; }
}

} // namespace prometheus
