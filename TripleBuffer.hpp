#ifndef TRIPLE_BUFFER
#define TRIPLE_BUFFER

#include <array>
#include <mutex>


enum BuffState
{
    WRITING,
    READING,
    READY,
    OLD
};

template <class T>
class TripleBuffer
{
public:
    TripleBuffer(){};
    ~TripleBuffer() = default;

    void * producerGet()
    {
        bool miss = true;
        int ready = -1;
        _mtx.lock();
        for(int i=0;i<_buffState.size();i++)
        {
            if(_buffState[i] == BuffState::OLD)
            {
                _buffState[i] = BuffState::WRITING;
                _producerIndex = i;
                miss = false;
                break;
            }
            else if (_buffState[i] == BuffState::READY)
            {
                ready = i;
            }
        }
        if (miss)
        {
            _buffState[ready] = BuffState::WRITING;
            _producerIndex = ready;
        }
        _mtx.unlock();
        return &(_tBuff[_producerIndex]);
    }

    void producerGive()
    {
        _mtx.lock();
        _buffState[_producerIndex] = BuffState::READY;
        _mtx.unlock();
    }

    void * consumerGet()
    {
        _mtx.lock();
        for(int i=0;i<_buffState.size();i++)
        {
            if(_buffState[i] == BuffState::READY)
            {
                _buffState[i] = BuffState::READING;
                _consumerIndex = i;
                _mtx.unlock();
                return &(_tBuff[_consumerIndex]);
            }
        }
        _mtx.unlock();
        return nullptr;
    }
    void consumerGive()
    {
        _mtx.lock();
        _buffState[_consumerIndex] = BuffState::OLD;
        _mtx.unlock();
    }

private:
    std::mutex _mtx;
    std::array<T,3> _tBuff;
    std::array<BuffState,3> _buffState { OLD, OLD, OLD };
    int _producerIndex;
    int _consumerIndex;
};

#endif