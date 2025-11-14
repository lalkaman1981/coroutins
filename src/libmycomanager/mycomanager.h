//
// Created by julfy on 11/14/25.
//
#pragma once

#include "libmycotask/mycotask.h"

#ifndef COROUTINES_MYCOMANAGER_H
#define COROUTINES_MYCOMANAGER_H


class mycomanager {
private:
    std::vector<mycotask> mycotasks_;

    size_t max_number_of_tasks_;

public:
    /**
     * This constructor automatically assigns a default priority to each created task.
     */
    template <typename... Args>
    requires (std::conjunction_v<std::is_same<Args, mycotask&>...>)
    explicit mycomanager(Args&&... args) {
        mycotasks_ = { std::forward<Args>(args)... };
    }

    /**
     * Create an empty coroutine manager
     *
     */
    explicit mycomanager(size_t max_number_of_tasks = 1000)
    : max_number_of_tasks_(max_number_of_tasks),
    mycotasks_() {};
    /**
     * This function normalizes the priority values.
     * For example, if the current priorities are <1, 2, 3> and a new task is added with priority 69,
     * the resulting priorities will be <1, 2, 3, 4> instead of <1, 2, 3, 69>.
     *
     * Essentially, priorities are **truncated or compressed** to maintain a contiguous sequence.
     */
    void add_task(mycotask task);

    /**
     * Change the priorities of the tasks
     */
    void change_task_priority(int old_priority, int new_priority);

    /**
     * Run previously created tasks in order
     */
    void run();


};


#endif //COROUTINES_MYCOMANAGER_H