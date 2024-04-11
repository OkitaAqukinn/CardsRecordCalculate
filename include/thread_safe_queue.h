#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H
#include <condition_variable>
#include <initializer_list>
#include <mutex>
#include <queue>
/*
 * 线程安全队列
 * T为队列元素类型
 * 因为有std::mutex和std::condition_variable类成员,所以此类不支持复制构造函数也不支持赋值操作符(=)
 * */
template <typename T, size_t MAX_NUM>
class threadsafe_queue {
   private:
    // data_queue访问信号量
    mutable std::mutex mut;
    mutable std::condition_variable data_cond;
    using queue_type = std::queue<T>;
    queue_type data_queue;

   public:
    using value_type = typename queue_type::value_type;
    using container_type = typename queue_type::container_type;
    threadsafe_queue() = default;
    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;
    /*
     * 使用迭代器为参数的构造函数,适用所有容器对象
     * */
    template <typename _InputIterator>
    threadsafe_queue(_InputIterator first, _InputIterator last) {
        for (auto itor = first; itor != last; ++itor) {
            data_queue.push(*itor);
        }
    }
    explicit threadsafe_queue(const container_type& c) : data_queue(c) {}
    /*
     * 使用初始化列表为参数的构造函数
     * */
    threadsafe_queue(std::initializer_list<value_type> list)
        : threadsafe_queue(list.begin(), list.end()) {}
    /*
     * 将元素加入队列
     * */
    void push(const value_type& new_value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.size() >= MAX_NUM) {
            data_queue.pop();
        }
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }
    /*
     * 从队列中弹出一个元素,如果队列为空就阻塞
     * */
    value_type wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !this->data_queue.empty(); });
        auto value = std::move(data_queue.front());
        data_queue.pop();
        return value;
    }
    /*
     * 从队列中弹出一个元素,如果队列为空就阻塞不超过指定最大时间
     * */
    bool wait_and_pop(value_type& value, unsigned int timeout_ms) {
        std::unique_lock<std::mutex> lk(mut);
        bool is_wait_not_timeout =
            data_cond.wait_for(lk, std::chrono::milliseconds(timeout_ms),
                               [this] { return !this->data_queue.empty(); });
        if (is_wait_not_timeout) {
            value = std::move(data_queue.front());
            data_queue.pop();
            return true;
        }

        return false;
    }
    /*
     * 从队列中弹出一个元素,如果队列为空返回false
     * */
    bool try_pop(value_type& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
    /*
     * 从队列中弹出一个元素,如果队列为空返回false
     * */
    bool try_get(value_type& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = data_queue.front();
        return true;
    }
    /*
     * 返回队列是否为空
     * */
    auto empty() const -> decltype(data_queue.empty()) {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
    /*
     * 返回队列中元素数个
     * */
    auto size() const -> decltype(data_queue.size()) {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.size();
    }
}; /* threadsafe_queue */

#endif /* THREAD_SAFE_QUEUE_H */