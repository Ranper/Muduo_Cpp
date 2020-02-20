#include <deque>
#include <pthread.h>
#include <assert.h>
#include <mutex>   

/*
条件变量的使用
wait端
  1、 必须与mutex一起使用，该布尔表达式的读写需受此mutex保护
  2、 在mutex已上锁的时候时候才能调用wait() 
  3、 把判断布尔条件和wait()放到while循环中。 
*/
using namespace std;
muduo::MutexLock mutex;
muduo::Condition cond(mutex); // 条件变量必须与mutex 一起使用，布尔表达式的读写需要mutex保护
std::deque<int> queue;        // 生产消费队列 ， 资源队列

int dequeue()
{
    MutexLockGuard lock(mutex); // 加锁
    while (queue.empty())  // 判断的布尔表达式
    {
        cond.wait();  // 原子地unlock mutex 并进入等待，不会与enqueue死锁
                      //  wait()执行完毕时会自动重新加锁 
    }
    assert(!queue.empty());  // 再次判断资源队列里是不是有资源
    int top = queue.front(); // 获取头节点
    queue.pop_front();       // 弹出头结点
    return top;
}
/*
条件变量的使用
signal/broadcast端
  1、 不一定要在mutex已上锁的情况下调用signal（理论上）
  2、 在signal之前一般要修改布尔表达式 
  3、 修改布尔表达式通常要用mutex保护
  4、 注意区分signal/broadcast。signal表示资源可用，broadcast表明状态变化。
*/

void enqueue(int n)
{
    {  // 临界区
        MutexLockGuard lock(mutex);  // 3、 修改布尔表达式通常要用mutex保护
        queue.push_back(n);  // 修改布尔表达式
    }
    cond.notify();

}