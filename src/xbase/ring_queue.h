#ifndef BASE_RING_QUEUE_H_
#define BASE_RING_QUEUE_H_
//参考 kfifo
//使用场景：一个生产者一个消费者
#include<stdlib.h>

namespace x{

#define RING_QUEUE_INIT_CAPACITY 1024
#define RONG_QUEUE_APPEND_CAPACITY 1024

template<class T>
class RingQueue {
public:
	struct Item {
		T value;
		Item* next;
	};

	RingQueue()
	:head_(NULL)
	,tail_(NULL){
		Resize(RING_QUEUE_INIT_CAPACITY);
	}

	~RingQueue(){
	
	}

	T* Top(){
		if(tail_ == head_) return NULL;
		return &(tail_->value);
	}

	T* Bottom(){
		if(head_->next == tail_){
			Resize(RONG_QUEUE_APPEND_CAPACITY);
		}

		if(head_->next == tail_) return NULL;
		return &(head_->value);
	}

	void Push(){
		if(head_->next == tail_) return;
		head_ = head_->next;
	}

	void Pop(){
		if(tail_ == head_) return;
		tail_ = tail_->next;
	}

	void Clear(){
		tail_ = head_;
	}
private:
	void Resize(size_t size){
		Item *head = NULL, *tail = NULL;
		for(size_t i =0; i < size; i++){
			Item* item = new Item();
			if(!head) head = item;
			if(tail) tail->next = item;
			tail = item;
		}

		if(!head_ && !tail_){
			tail->next = head;
			head_ = head;
			tail_ = head;
		}else{
			tail->next = head_->next;
			head_->next = head;
		}
	}
	
	Item* head_;
	Item* tail_;
};
}//namespace x
#endif //BASE_RING_QUEUE_H_

