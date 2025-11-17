#include <boost/intrusive/circular_slist_algorithms.hpp>
#include <iostream>

struct Node {
  Node *next = nullptr;
  int value = 0;
};

struct NodeTraits {
  using node = Node;
  using node_ptr = Node *;
  using const_node_ptr = const Node *;

  static node_ptr get_next(const_node_ptr p) { return p->next; }
  static void set_next(node_ptr p, node_ptr n) { p->next = n; }
};

using algo = boost::intrusive::circular_slist_algorithms<NodeTraits>;

int main() {
  Node head;
  algo::init_header(&head);

  Node a{nullptr, 10};
  Node b{nullptr, 20};
  Node c{nullptr, 30};

  algo::init(&a);
  algo::init(&b);
  algo::init(&c);

  algo::link_after(&head, &a);
  algo::link_after(&a, &b);
  algo::link_after(&head, &c);

  Node *cur = NodeTraits::get_next(&head);
  while (cur != &head) {
    std::cout << cur->value << "\n";
    cur = NodeTraits::get_next(cur);
  }
  cur = NodeTraits::get_next(cur);
  while (cur != &head) {
    std::cout << cur->value << "\n";
    cur = NodeTraits::get_next(cur);
  }
}
