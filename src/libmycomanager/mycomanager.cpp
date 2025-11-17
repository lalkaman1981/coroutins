//
// Created by julfy on 11/14/25.
//

#include "mycomanager.h"

mycomanager::mycomanager(const size_t max_number_of_tasks)
    : head_(), max_number_of_tasks_(max_number_of_tasks) {
    algo::init(&head_);
    first = true;
}

void mycomanager::append_task(mycotask task) {

    if (first) {
        head_.~mycotask();
        new (&head_) mycotask(std::move(task));

        algo::init(&head_);

        // head_ points to itself
        MycoNodeTraits::set_next(&head_, &head_);

        tail_ = &head_;
        first = false;
        return;
    }

    // allocate new node
    auto* stored = new mycotask(std::move(task));
    algo::init(stored);

    algo::link_after(tail_, stored);

    // stored -> head_
    MycoNodeTraits::set_next(stored, &head_);
    tail_ = stored;
}




void mycomanager::run() {

    if (head_.id_ == 0) {
        std::cerr << "mycomanager: you cannot start an empty coroutines manager! Please, add some tasks!" << std::endl;
        exit(EXIT_FAILURE);
    }
    mycotask* cur = &head_;
    mycotask* prev;
    int count = 0;

    while (true) {
        if (cur->ended_) {
            if (prev->get_id() == 0){
                delete cur;
                return;
            }
            if (prev == cur) {break;}
            mycotask* next = NodeTraits::get_next(cur);
            algo::unlink_after(prev);
            if (cur != &head_) delete cur;
            cur = prev->next();  // move forward
            continue;
        }

        if (!cur->started_) {
            cur->start();
        } else {
            cur->resume();
        }
        prev = cur;
        cur = NodeTraits::get_next(cur);
    }
}
