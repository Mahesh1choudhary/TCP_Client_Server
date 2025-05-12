#include <mutex>
#include <condition_variable>
#include <memory>



template<typename T> class thread_safe_queue {
    public:
        thread_safe_queue() :head(new node), tail(head.get()){

        }
        thread_safe_queue(const thread_safe_queue&) = delete;
        thread_safe_queue& operator=(const thread_safe_queue&) = delete;

        void push(T new_value){
            unique_ptr<node> new_node(new node);
            shared_ptr<T> new_data(make_shared<T>(move(new_value)));

            {
                
            }




    private:
        struct node{
            shared_ptr<T> data;
            unique_ptr<node> next;
        };

        unique_ptr<node<T>> head;
        node<T>* tail;
        mutex head_mutex, tail_mutex;
        condition_variable data_cond;



}