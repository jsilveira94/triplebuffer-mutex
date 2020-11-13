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

/**
 *  Class to handle one producer and one consumer using a tripleBuffer where the consumer
 *  always wants the last produced element.
 */
template <class T>
class TripleBuffer
{
public:
    TripleBuffer(){};
    ~TripleBuffer() = default;

    /**
     *  Function to be called by the producer to get an avaliable memory to be writen.
     */
    T * producerGet()
    {
        bool miss = true;
        int ready = -1;
        _mtx.lock();
        for(int i=0;i<_buffState.size();i++) // search for an OLD buffer to write 
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
        if (miss) // If there is not OLD buffers uses a READY one.
        {
            _buffState[ready] = BuffState::WRITING;
            _producerIndex = ready;
        }
        _mtx.unlock();
        return &(_tBuff[_producerIndex]);
    }

    /**
     * Function to be called by the producer when the new element has been written.
     */
    void producerGive(T*& buff)
    {
        _mtx.lock();
        for(int i=0;i<_buffState.size();i++)
        {
            if(_buffState[i] == BuffState::READY)
                _buffState[i] = BuffState::OLD;

        }
        _buffState[_producerIndex] = BuffState::READY;
        _mtx.unlock();
    }

    /**
     * Function that gets the last produced element.
     */
    T * consumerGet()
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

    /**
     * Function that returns the consumed element.
     */
    void consumerGive(T*& buff)
    {
        _mtx.lock();
        _buffState[_consumerIndex] = BuffState::OLD;
        _mtx.unlock();
        buff = nullptr;
    }

private:
    std::mutex _mtx;
    std::array<T,3> _tBuff;
    std::array<BuffState,3> _buffState { OLD, OLD, OLD };
    int _producerIndex;
    int _consumerIndex;
};

#endif