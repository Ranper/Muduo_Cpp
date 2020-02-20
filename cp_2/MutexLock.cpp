#include<pthread.h>
#include<cassert>

/*
用到的函数
pthread_mutex_init()
pthread_mutex_destory()
pthread_mutex_lock()
pthread_mutex_unlock()
@data: 2020/02/18
*/

class MutexLock
{
public:
    explicit MutexLock(pthread_mutex_t & mutex):
    mutex_(mutex),holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL); // 初始化
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
        holder_ = CurrentThread::tid();  //持有该锁的线程id
    }

    void unlock()
    {
        holder_ = 0;
        pthread_mutex_destroy(&mutex_);  // 先清零，再释放
    }

    bool isLockedByThisThread()  // 判断是不是当前进程持有锁
    {
        return holder_ == CurrentThrad::tid();  
    }

    void assertLocked()  // 如果是当前进程获得了锁，不报错。如果是其他线程或者没有线程获得锁，报错
    {
        assert(isLockedByThisThread());
    }
    pthread_mutex_t * getPthreadMutex()  // 返回该线程持有的锁，返回的是地址，可以更改其状态
    {
        return &mutex_;
    }

    ~MutexLock()
    {
        pthread_mutex_destroy(&mutex_);
    }

private:
    pthread_mutex_t &mutex_; //这里要声明为引用形式，表明用的是初始化时传递的那个mutex
    pid_t holder_;
};