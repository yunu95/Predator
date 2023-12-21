#pragma once
#include <mutex>

namespace yunutyEngine
{
    // mutex를 이용해 하나의 객체 레퍼런스에 대한 여러 스레드들의 접근을 제한하는 클래스
    // 이 객체는 RAII에 충실하므로, 레퍼런스의 사용이 다 끝나면 이 리소스에 대한 점유상태를 해제하기 위해서라도 이 객체는 삭제되어야 한다. 
    template <typename T>
    class MutexedReference {
    public:
        MutexedReference(T& ref, std::mutex& mtx) : data(ref), mutex(mtx) {
            mutex.lock();
        }

        ~MutexedReference() {
            mutex.unlock();
        }

        T& operator*() {
            return data;
        }

    private:
        T& data;
        std::mutex& mutex;
    };
}
