// 手写 生产者/消费者模式
template<class T>
class block_queue{
private:
    int max_size;
    int size;
    T* array;  // 阻塞队列
    int front;
    int back;
    locker mutex;
    cond m_cond;

public:
    block_queue(int max=10000){
        max_size=max;
        array=new T[max_size]; //
        front=-1;
        back=-1;
    }
    void clear(){
        // 线程安全
        mutex.lock();
        size=0;
        front=-1;
        back=-1;
        mutex.unlock();
    }
    ~ block_queue(){
        mutex.lock();
        if(array!=NULL){
            delete [] array;
        }
        mutex.unlock();
    }
    bool full{
        mutex.lock();
        if(size>=max_size){
            mutex.unlock();
            return true;
        }
        mutex.unlock();
        return false;
    }
    bool empty() 
    {
        mutex.lock();
        if (0 == m_size)
        {
            mutex.unlock();
            return true;
        }
        mutex.unlock();
        return false;
    }
    bool push(const T &item){
        mutex.lock();
        if(size>=max_Size){
            m_cond.broadcast();
            mutex.unlock();
            return false;
        }

        back=(back+1)%max_size;  ///
        array[back]=item;        ///
        size++;
        m_cond.broacast();
        mutex.unlock();
        return true;

    }

    bool pop(T& item){
        mutex.lock();
        while(size<=0){
            //当重新抢到互斥锁，pthread_cond_wait返回为0
            if (!m_cond.wait(mutex.get()))
            {
                m_mutex.unlock();
                return false;
            }
        }
        front=(front+1)%max_size;
        item=array[front];
        size--;
        mutex.unlock();
        return true;
    }
};