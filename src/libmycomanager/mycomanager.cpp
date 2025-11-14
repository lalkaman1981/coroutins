//
// Created by julfy on 11/14/25.
//

#include "mycomanager.h"

void mycomanager::add_task(mycotask task) {
    mycotasks_.emplace_back(task);
}

void mycomanager::change_task_priority(int old_priority, int new_priority) {
    if (mycotasks_.size() > max_number_of_tasks_) {
        std::cerr << "mycomanager: the maximum amount of tasks in exceeded " << max_number_of_tasks_ << std::endl;
        exit(EXIT_FAILURE);
    }

    if (old_priority == new_priority) {
        std::cerr << "mycomanager: this action will not change anything!";
    }
    // ...
}

