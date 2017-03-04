#include "per_set.h"
#include <iostream>

persistent_set::persistent_set() {
	head = nullptr;
}

persistent_set::~persistent_set() {
	if (head != nullptr) {
		head -> parents--;
		if (head -> parents == 0) 
			delete head;
	} 
}

persistent_set::persistent_set(persistent_set const& per_set) {
	if (per_set.head != nullptr) {
		head = per_set.head;
		head -> parents++;
	}
}

persistent_set& persistent_set::operator=(persistent_set const& rhs) {
	if (head != nullptr) {
		head -> parents--;
		if (head -> parents == 0)
			delete head;
	}
	if (rhs.head != nullptr) {
		head = rhs.head;
		head -> parents++;
	}
	return *this;
}

persistent_set::node::node(int new_value) {
	parents = 0;
	left = nullptr;
	right = nullptr;
	value = new_value;
}

persistent_set::node::~node() {
	disconnect_left();
	disconnect_right();
}

void persistent_set::node::disconnect_left() {
	if (left != nullptr) {
		(left -> parents)--;
		if (left -> parents == 0) 
			delete left;
		left = nullptr;
	}
}

void persistent_set::node::disconnect_right() {
	if (right != nullptr) {
		(right -> parents)--;
		if (right -> parents == 0)
			delete right;
		right = nullptr;
	}
}

void persistent_set::node::connect_left(node *new_left) {
	if (new_left == left) return;
	if (left != nullptr) 
		disconnect_left();
	left = new_left;
	new_left -> parents++;
}
void persistent_set::node::connect_right(node *new_right) {
	if (new_right == right) return;
	if (right != nullptr) 
		disconnect_right();
	right = new_right;
	new_right -> parents++;
}

persistent_set::iterator persistent_set::begin() const {
	iterator result;
	if (head == nullptr) {
		result.is_end = true;
		return result;
	}
	node *now = head;
	while (now -> left != nullptr) {
		result.stack.push_back(now);
		now = now -> left;
	}
	result.now = now;
	return result;
}

persistent_set::iterator persistent_set::end() const {
	iterator result;
	result.now = head;
	result.is_end = true;
	return result;
}

persistent_set::value_type const& persistent_set::iterator::operator*() const {
	return now -> value;
}

persistent_set::iterator persistent_set::iterator::operator++(int) {
    iterator current = *this;
    ++*this;
    return current;
}

persistent_set::iterator persistent_set::iterator::operator--(int) {
    iterator current = *this;
    --*this;
    return current;
}

persistent_set::iterator& persistent_set::iterator::operator++() {
	if (is_end)
		 return *this;
	if (now -> right != nullptr) {
		stack.push_back(now);
		now = now -> right;
		while (now -> left != nullptr) {
			stack.push_back(now);
			now = now -> left;
		}
		return *this;
	}
	if (stack.size() != 0 && stack.back() -> left == now) {
		now = stack.back();
		stack.pop_back();
		return *this;
	}

	while (stack.size() != 0 && stack.back() -> right == now) {
		now = stack.back();
		stack.pop_back();
	}
	if (stack.size() == 0)
		is_end = true;
	else {
		now = stack.back();
		stack.pop_back();
	}
	return *this;
}

persistent_set::iterator& persistent_set::iterator::operator--() {
	if (is_end) {
		stack.clear();
		while (now -> right != nullptr) {
			stack.push_back(now);
			now = now -> right;
		}
		is_end = false;
		return *this;
	}

	if (now -> left != nullptr) {
		stack.push_back(now);
		now = now -> left;
		while (now -> right != nullptr) {
			stack.push_back(now);
			now = now -> right;
		}
		return *this;
	}
	while (stack.size() != 0 && stack.back() -> left == now) {
		now = stack.back();
		stack.pop_back();
	}
	if (stack.size() != 0) {
		now = stack.back();
		stack.pop_back();
	}
	return *this;	
}

bool persistent_set::iterator::is_end_get() {
	return is_end;
}

bool persistent_set::iterator::not_equals(persistent_set::iterator a) {
	if (a.is_end && is_end) return false;
	return (a.is_end != is_end) || (a.now -> value != now -> value);
}

bool operator!=(persistent_set::iterator a, persistent_set::iterator b)
{
    return a.not_equals(b);
}

bool operator==(persistent_set::iterator a, persistent_set::iterator b)
{
    return !a.not_equals(b);
}

std::pair<persistent_set::iterator, bool> persistent_set::persistent_set::insert(value_type new_value) {
	iterator result = find(new_value);

	if (!result.is_end) {
		return std::pair<persistent_set::iterator, bool>(result, false);
	}


	result.is_end = false;

	if (head == nullptr) {
		head = new node(new_value);
		head -> parents++;
		result.now = head;
		return std::pair<persistent_set::iterator, bool>(result, true);
	}

	node *now = head, *old_head = head;
	head = new node(old_head -> value);
	head -> parents++;
	node *copy_node = head;

	while (now != nullptr) {
		result.stack.push_back(copy_node);	
		if (now -> value > new_value) {

			if (now -> right != nullptr) {
				copy_node -> connect_right(now -> right);
			}
			if (now -> left != nullptr) {
				node *new_create_node = new node(now -> left -> value);
				copy_node -> connect_left(new_create_node);
				copy_node = new_create_node;
			}
			now = now -> left;

		} else {
			
			if (now -> left != nullptr) {
				copy_node -> connect_left(now -> left);
			}
			if (now -> right != nullptr) {
				node *new_create_node = new node(now -> right -> value);
				copy_node -> connect_right(new_create_node);
				copy_node = new_create_node;
			}
			now = now -> right;
		}
	}

	(old_head -> parents)--;
	if (old_head -> parents == 0) delete old_head;

	node *new_node = new node(new_value);
	if (copy_node -> value > new_value)
		copy_node -> connect_left(new_node);
	else 
		copy_node -> connect_right(new_node);
	result.now = new_node;
	return std::pair<persistent_set::iterator, bool>(result, true);	
}

void persistent_set::erase(persistent_set::iterator iter) {
	if (head == nullptr) return;
    if (iter.now -> value == 60)
        iter.now = iter.now;
	bool need_to_delete = true;

	node *delete_node = iter.now;
	node *old_head = head;
	node *start_node, *copy_start_node, *copy_start_node_parent;

	if (delete_node == head) {
		if (head -> left == nullptr && head -> right == nullptr) {
            head -> parents--;
            if (head -> parents == 0) {
				delete head; 
            }
			head = nullptr; 
			return;

		} else if (head -> left == nullptr || head -> right == nullptr) {

			bool is_right = head -> right != nullptr;
			node *old_head = head;
			head = is_right ? head -> right : head -> left;
			head -> parents++;
			old_head -> parents--;
			if (old_head -> parents == 0)
				delete old_head;
			return;

		} else {
			node *new_node = head -> right;
			while (new_node -> left != nullptr) new_node = new_node -> left;
			delete_node = new_node;
			head = new node(new_node -> value);
			head -> connect_left(old_head -> left);
		}
	} else {
		head = new node(old_head -> value);
	}

	start_node 				= old_head;
	copy_start_node 		= head; 
	copy_start_node_parent	= nullptr;
	head -> parents++;
	while (need_to_delete) {
		node *now 				= start_node;
		node *copy_node 		= copy_start_node;
		node *copy_node_parent	= copy_start_node_parent;

		while (delete_node != now) {
			if (now -> value > delete_node -> value) {
				if (now -> right != nullptr) {
					copy_node -> connect_right(now -> right);
				}
				node *new_create_node = new node(now -> left -> value);
				copy_node -> connect_left(new_create_node);
				copy_node_parent = copy_node;
				copy_node = new_create_node;
				now = now -> left;
			} else {
				if (now -> left != nullptr) {
					copy_node -> connect_left(now -> left);
				}
				node *new_create_node = new node(now -> right -> value);
				copy_node -> connect_right(new_create_node);
				copy_node_parent = copy_node;
				copy_node = new_create_node;
				now = now -> right;
			}
		}


		if (now -> left == nullptr && now -> right == nullptr) {
			if (copy_node_parent -> left == copy_node) {
				copy_node_parent -> disconnect_left();
			} else {
				copy_node_parent -> disconnect_right();
			}
			

			need_to_delete = false;
		} else if (now -> left == nullptr || now -> right == nullptr) {
			bool is_right_new = copy_node_parent -> right == copy_node;
			bool is_right_old = now -> right != nullptr;
			if (is_right_new) {
				if (is_right_old) {					
					copy_node_parent -> connect_right(now -> right);
				} else {
					copy_node_parent -> connect_right(now -> left);
				}
			} else {
				if (is_right_old) {
					copy_node_parent -> connect_left(now -> right);
				} else {
					copy_node_parent -> connect_left(now -> left);
				}
			}
			need_to_delete = false;
		} else {
			node *new_node_found = now -> right;
			while (new_node_found -> left != nullptr) new_node_found = new_node_found -> left;
			delete_node = new_node_found;

			node *new_node_for_connect = new node(new_node_found -> value);
			if (copy_node_parent -> right == copy_node) {

				copy_node_parent -> connect_right(new_node_for_connect);
			}
			else {
				copy_node_parent -> connect_left(new_node_for_connect);
			}

			start_node = now;
			copy_start_node = new_node_for_connect;
			copy_start_node_parent = copy_node_parent;
		}
	}

	old_head -> parents--;
	if (old_head -> parents == 0)
		delete old_head;
}

persistent_set::iterator persistent_set::persistent_set::find(value_type find_value) {
	iterator result;
	if (head == nullptr) {
		result.is_end = true;
		return result;
	}

	node *now = head;
	while (now -> value != find_value && !result.is_end) {
		if (find_value < now -> value && now -> left != nullptr) {
			result.stack.push_back(now);
			now = now -> left;
		} else if (find_value > now -> value && now -> right != nullptr) {
			result.stack.push_back(now);
		} else result.is_end = true;
	}
	result.now = now;
	if (result.is_end) {
		result.stack.clear();
		result.now = head;
	}
	return result;
}
