#include "thread_safe_queue.h"
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>

// simple test cases for thread_safe_queue
void test_single_thread(){
    thread_safe_queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    int value;
    assert(queue.try_pop(value));
    assert(value == 1);
    assert(queue.try_pop(value));
    assert(value == 2);
    assert(queue.try_pop(value));
    assert(value == 3);
    assert(queue.empty());
    assert(!queue.try_pop(value));
}

void test_multiple_threads(){
    thread_safe_queue<int> queue;
    const int producer_threads = 10, consumer_threads = 10, items_per_thread = 100;
    std::vector<std::thread> producers, consumers;

    for(int i=0; i<producer_threads; ++i){
        producers.emplace_back([&queue, &items_per_thread, i](){
            for(int j=0; j<items_per_thread; ++j){
                queue.push(i*items_per_thread + j);
            }
        });
    }

    std::vector<int> results;
    std::mutex results_mutex;
    std::atomic<int> count(0);

    for(int i=0; i<consumer_threads; ++i){
        consumers.emplace_back([&queue, &results, &results_mutex, &count](){
            int value;
            while(count.load() != items_per_thread*producer_threads){
                if(queue.try_pop(value)){
                    std::lock_guard<std::mutex> lock(results_mutex);
                    results.push_back(value);
                    count.fetch_add(1);
                }
            }
        });
    }

    for(auto& producer : producers){
        producer.join();
    }
    for(auto& consumer : consumers){
        consumer.join();
    }

    assert(results.size() == items_per_thread*producer_threads);
}

int main(){
    test_single_thread();
    test_multiple_threads();
    
    std::cout << "All test passed!" << std::endl;

    return 0;


}