//
// Created by julfy on 11/14/25.
//

#pragma once

#include "libmycotask/mycotask.h"
#include <boost/intrusive/circular_slist_algorithms.hpp>

#ifndef COROUTINES_MYCOMANAGER_H
#define COROUTINES_MYCOMANAGER_H

struct MycoNodeTraits {
    using node = mycotask;
    using node_ptr = mycotask*;
    using const_node_ptr = const mycotask*;

    static node_ptr get_next(const_node_ptr p) { return p->next_; }
    static void set_next(node_ptr p, node_ptr n) { p->next_ = n; }
};

using algo = boost::intrusive::circular_slist_algorithms<MycoNodeTraits>;

class mycomanager {
private:
    using NodeTraits = MycoNodeTraits;
    using algo = boost::intrusive::circular_slist_algorithms<NodeTraits>;

    // satisfy compiler with default field
    mycotask head_;
    mycotask* tail_ = &head_;

    size_t max_number_of_tasks_{};

    bool first;

    static mycotask default_task;

public:
    /**
     * This constructor automatically assigns a default priority to each created task.
     */
    template<typename... Tasks>
    explicit mycomanager(Tasks&&... ts) : mycomanager() {
        (append_task(std::move(ts)), ...);
        // first = false;
    }

    /**
     * Create an empty coroutine manager
     */
    explicit mycomanager(const size_t max_number_of_tasks = 1000);

    /**
     * This function normalizes the priority values.
     * For example, if the current priorities are <1, 2, 3> and a new task is added,
     * the resulting priorities will be <1, 2, 3, 4>.
     */
    void append_task(mycotask&& task);

    /**
     * Run previously created tasks in order
     */
    void run();

    // /**
    //  * Change the priorities of the tasks
    //  */
    // void change_task_priority(int old_priority, int new_priority);

};
#endif // COROUTINES_MYCOMANAGER_H
