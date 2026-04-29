path = '/opt/stacks/zt-esp32/ZeroTierOne/node/Mutex.hpp'
with open(path, 'r') as f:
    content = f.read()

if 'CONFIG_IDF_TARGET' in content:
    print('ALREADY_PATCHED')
    exit(0)

insert = '''
#elif defined(CONFIG_IDF_TARGET)

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace ZeroTier {

class Mutex {
  public:
\tMutex()  { _mh = xSemaphoreCreateMutex(); }
\t~Mutex() { vSemaphoreDelete(_mh); }

\tinline void lock()   const { xSemaphoreTake(const_cast<Mutex*>(this)->_mh, portMAX_DELAY); }
\tinline void unlock() const { xSemaphoreGive(const_cast<Mutex*>(this)->_mh); }

\tclass Lock {
\t  public:
\t\tLock(Mutex &m)        : _m(&m)                    { m.lock(); }
\t\tLock(const Mutex &m)  : _m(const_cast<Mutex*>(&m)) { _m->lock(); }
\t\t~Lock() { _m->unlock(); }
\t  private:
\t\tMutex* const _m;
\t};

  private:
\tMutex(const Mutex&) {}
\tconst Mutex& operator=(const Mutex&) { return *this; }
\tSemaphoreHandle_t _mh;
};

} // namespace ZeroTier

'''

old = '#endif\t // _WIN32\n\n#endif'
if old in content:
    content = content.replace(old, '#endif\t // _WIN32\n' + insert + '#endif')
else:
    print('ERROR: insertion point not found')
    print(repr(content[-150:]))
    exit(1)

with open(path, 'w') as f:
    f.write(content)
print('MUTEX_PATCHED')
