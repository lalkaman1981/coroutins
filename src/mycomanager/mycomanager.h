/**
 * @file mycomanager.h
 * @brief Coroutine scheduler for managing multiple cooperative tasks
 *
 * Implements a round-robin scheduler using Boost intrusive circular singly-linked list
 * to efficiently manage coroutine execution without dynamic memory overhead per node.
 */

/** @addtogroup scheduler
 *  @{
 */

#pragma once
#include "../mycotask/mycotask.h"
#include <boost/intrusive/circular_slist_algorithms.hpp>

#ifndef COROUTINES_MYCOMANAGER_H
#define COROUTINES_MYCOMANAGER_H

/**
 * @struct MycoNodeTraits
 * @brief Traits for Boost intrusive list node operations
 *
 * Defines how mycotask objects behave as nodes in the intrusive circular list.
 * Provides pointer access to the next_ field for list linkage.
 */
struct MycoNodeTraits {
    using node = mycotask;
    using node_ptr = mycotask *;
    using const_node_ptr = const mycotask *;

    static node_ptr get_next(const_node_ptr p) { return p->next_; }
    static void set_next(node_ptr p, node_ptr n) { p->next_ = n; }
};

using algo = boost::intrusive::circular_slist_algorithms<MycoNodeTraits>;

/**
 * @class mycomanager
 * @brief Round-robin scheduler for cooperative multitasking
 *
 * Manages a circular list of coroutines and executes them in round-robin fashion.
 * Each coroutine runs until it yields, then the next coroutine in the list executes.
 * Completed coroutines are automatically removed from the schedule.
 */
class mycomanager {
private:
    using NodeTraits = MycoNodeTraits;
    using algo = boost::intrusive::circular_slist_algorithms<MycoNodeTraits>;

    mycotask head_;
    mycotask *tail_ = &head_;
    size_t max_number_of_tasks_{};
    bool first;
    static mycotask default_task;

public:
    /**
     * @brief Construct manager with variadic list of tasks
     * @tparam Tasks Pack of mycotask rvalue references
     * @param ts Tasks to add to the scheduler
     *
     * Convenience constructor that automatically adds all provided tasks
     * to the manager upon construction.
     */
    template<typename... Tasks>
    explicit mycomanager(Tasks &&...ts) : mycomanager() {
        (append_task(std::move(ts)), ...);
    }

    /**
     * @brief Create an empty coroutine manager
     * @param max_number_of_tasks Maximum tasks supported (default 1000)
     *
     * Initializes an empty scheduler. Tasks must be added via append_task()
     * before calling run().
     */
    explicit mycomanager(const size_t max_number_of_tasks = 1000);

    /**
     * @brief Add a new task to the scheduler
     * @param task Coroutine task to append (moved)
     *
     * Appends the task to the end of the circular list. If this is the first task,
     * it becomes the head. Otherwise, it's linked after the current tail.
     */
    void append_task(mycotask &&task);

    /**
     * @brief Execute all scheduled tasks in round-robin fashion
     *
     * Runs coroutines cooperatively until all complete. Each iteration:
     * 1. Starts unstarted tasks
     * 2. Resumes running tasks
     * 3. Removes completed tasks from the schedule
     *
     * Exits when all tasks have finished execution.
     *
     * @throws Calls exit(EXIT_FAILURE) if run on empty manager
     */
    void run();
};

#endif // COROUTINES_MYCOMANAGER_H

/** @} */