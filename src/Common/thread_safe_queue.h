#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>



template<typename T> class thread_safe_queue {
    public:
        thread_safe_queue() :head(new node), tail(head.get()){}
        thread_safe_queue(const thread_safe_queue&) = delete;
        thread_safe_queue& operator=(const thread_safe_queue&) = delete;


        bool empty(){
            std::lock_guard<std::mutex> head_lock(head_mutex);
            return (head.get() == get_tail());
        }

        void push(T new_value){
            std::unique_ptr<node> new_node(new node);
            std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));

            {
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data = new_data;
                node* const new_tail = new_node.get();
                tail->next = std::move(new_node);
                tail = new_tail;
            }
            data_cond.notify_one();
        }

        
        std::shared_ptr<T> wait_and_pop(){
            std::unique_ptr<node> const old_head = wait_pop_head();
            return old_head->data;
        }

        void wait_and_pop(T& value){
            std::unique_ptr<node> const old_head = wait_pop_head(value);
        }

        std::shared_ptr<T> try_pop(){
            std::unique_ptr<node> old_head = try_pop_head();
            if(old_head == nullptr){
                return shared_ptr<T>();
            }
            else return old_head;
        }

        bool try_pop(T& value){
            std::unique_ptr<node> const old_head = try_pop_head(value);
            return old_head != nullptr;
        }


    private:
        struct node{
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };

        std::unique_ptr<node> head;
        node* tail;  // tail will be dummy node
        std::mutex head_mutex, tail_mutex;
        std::condition_variable data_cond;

        node* get_tail(){
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        // not thread safe - it is called only after taking proper locks
        std::unique_ptr<node> pop_head(){
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            return old_head;
        }

        // wait till there are no element in queue.
        std::unique_lock<std::mutex> wait_for_data(){
            std::unique_lock<std::mutex> head_lock(head_mutex);
            data_cond.wait(head_lock, [&]{ return head.get() != get_tail();});
            return std::move(head_lock);
        }

        // wait for an element in queue and pop it
        std::unique_ptr<node> wait_pop_head(){
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            return pop_head();
        }

        // wait for an element in queue and pop it
        std::unique_ptr<node> wait_pop_head(T& value){
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            value = std::move(*head->data);
            return pop_head();
        }

        // if there is element in queue, pop it, otherwise return nullptr
        std::unique_ptr<node> try_pop_head(){
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() == get_tail()){
                return std::unique_ptr<node>();
            }
            return pop_head();
        }

         // if there is element in queue, pop it, otherwise return nullptr
        std::unique_ptr<node> try_pop_head(T& value){
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() == get_tail()){
                return std::unique_ptr<node>();
            }
            value = std::move(*head->data);
            return pop_head();
        }


};
