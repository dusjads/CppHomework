#include "set.h"
#include <iostream>

const set::value_type inf = 1e9;

set::node::node(set::value_type new_val){
	val = new_val;
	left = nullptr;
	right = nullptr;
	parent = nullptr;
}

// Элемент на который сейчас ссылается итератор.
// Разыменование итератора end() неопределено.
// Разыменование невалидного итератора неопределено.
set::value_type const& set::iterator::operator*() const{
	return cur->val;
}

// Переход к элементу со следующим по величине ключом.
// Инкремент итератора end() неопределен.
// Инкремент невалидного итератора неопределен.
set::iterator& set::iterator::operator++(){
	if (is_end)
		return *this;
	if (cur->right){
		cur = cur->right;
		while (cur->left){
			stack.push_back(cur);
			cur = cur->left;
		}
		return *this;
	}
	while (stack.size() != 0 && stack.back()->right == cur){
		cur = stack.back();
		stack.pop_back();
	}
	if (stack.size() == 0){
		is_end = true;
		return *this;
	}
	cur = stack.back();
	stack.pop_back();
	return *this;
}

set::iterator set::iterator::operator++(int){
	iterator prev = *this;
	++*this;
	return prev;
}

// Переход к элементу с предыдущим по величине ключом.
// Декремент итератора begin() неопределен.
// Декремент невалидного итератора неопределен.
set::iterator& set::iterator::operator--(){
	if (is_end){
		is_end = false;
		return *this;
	}
	if (cur->left){
		cur = cur->left;
		while (cur->right){
			stack.push_back(cur);
			cur = cur->right;
		}
		return *this;
	}
	while (stack.size() != 0 && stack.back()->left == cur){
		cur = stack.back();
		stack.pop_back();
	}
	if (stack.size() == 0){
		return *this;
	}
	cur = stack.back();
	stack.pop_back();
	is_end = false;
	return *this;

}
set::iterator set::iterator::operator--(int){
	iterator next = *this;
	--*this;
	return next;
}

set::set() {
	head = nullptr;
}

// Создает копию указанного set-а.
set::set(set const& source){
	head = source.head;
}

// Изменяет this так, чтобы он содержал те же элементы, что и rhs.
// Инвалидирует все итераторы, принадлежащие set'у this, включая end().
set& set::operator=(set const& rhs){
	head = rhs.head;
}

// Деструктор. Вызывается при удалении объектов set.
// Инвалидирует все итераторы ссылающиеся на элементы этого set
// (включая итераторы ссылающиеся на элемент следующий за последним).
set::~set(){
	delete head;
}

set::iterator set::appr_find(set::value_type x) const{
	iterator new_it;
	new_it.cur = head;
	if (!head)
		return new_it;

	node* it_node = new_it.cur;
	while (((it_node->left && it_node->val > x) || (it_node->right && it_node->val < x))){
		while (it_node->left && it_node->val > x){
			new_it.stack.push_back(it_node);
			it_node = it_node->left;
		}
		while (it_node->right && it_node->val < x){
			new_it.stack.push_back(it_node);
			it_node = it_node->right;
		}
	}
	new_it.cur = it_node;
	return new_it;
}


// Поиск элемента.
// Возвращает итератор на найденный элемент, либо end(), если элемент
// с указанным значением отсутвует.
set::iterator set::find(set::value_type x) const{
	iterator new_it = appr_find(x);
	node* it_node = new_it.cur;
	if (x == inf){
		new_it.is_end = true;
		return new_it;
	}
	if (it_node->val != x)
		return end();
}

// Вставка элемента.
// 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
//    на уже присутствующий элемент и false.
// 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
//    элемент и true.
std::pair<set::iterator, bool> set::insert(set::value_type x){
	if (!head){
		head = new node(x);
		iterator new_it;
		new_it.cur = head;
		return std::make_pair(new_it, true);
	}
	iterator new_it = appr_find(x);
	node* it_node = new_it.cur;
	if (it_node->val == x){
		return std::make_pair(new_it, false);
	}
	new_it.stack.push_back(it_node);
	if (it_node->val > x){
		it_node->left = new node(x);
		it_node->left->parent = it_node;
		it_node = it_node->left;
	}
	else{
		it_node->right = new node(x);
		it_node->right->parent = it_node;
		it_node = it_node->right;
	}
	new_it.cur = it_node;
	return std::make_pair(new_it, true);
}


void set::erase_node(node* old){
	if (old == head){
		head = nullptr;
		return;
	}
	if (!old->left && !old->right){
		if (old->parent->left == old){
			old->parent->left = nullptr;
			return;
		}
		old->parent->right = nullptr;
		return;
	}
	bool is_left = (old == old->parent->left);
	if (!old->left){
		if (is_left){
			old->right->parent = old->parent;
			old->parent->left = old->right;
		}
		else{	
			old->right->parent = old->parent;
			old->parent->right = old->right;
		}
		return;
	}
	if (!old->right){
		if (is_left){
			old->left->parent = old->parent;
			old->parent->left = old->left;
		}
		else{	
			old->left->parent = old->parent;
			old->parent->right = old->left;
		}
		return;
	}
	node* min_right = old->right;
	while (min_right->left){
		min_right = min_right->left;
	}
	set:value_type key = min_right->val;
	erase_node(min_right);
	node* new_node = new node(key);
	new_node->right = old->right;
	new_node->left = old->left;
	new_node->parent = old->parent;
	old->right->parent = new_node;
	if (is_left)
		old->parent->left = new_node;
	else
		old->parent->right = new_node;


}


// Удаление элемента.
// Инвалидирует все итераторы, принадлежащие set'у this, включая end().
void set::erase(iterator it){
	erase_node(it.cur);
}

// Возващает итератор на элемент с минимальный ключом.
set::iterator set::begin() const{
	return appr_find(-inf);
}
// Возващает итератор на элемент следующий за элементом с максимальным ключом.
set::iterator set::end() const{
	return find(inf);
}

bool set::iterator::equal(set::iterator const right){
	if (is_end || right.is_end)
		return is_end && right.is_end;
	return cur == right.cur;
}

bool operator==(set::iterator left, set::iterator right){
	return left.equal(right);
}

bool operator!=(set::iterator left, set::iterator right){
	return !left.equal(right);
}

set::value_type main()
{
    set c;
    for (int i = 0; i < 20; i+=5)
    	c.insert(i);
    for (int i = 0; i < 20; i+=2)
    	c.insert(i);
    for (int i = 0; i < 20; i+=3)
    	c.insert(i);
    for(auto it = c.begin(); it != c.end(); it++)
    	std::cout << *it << ' ';
    std::cout << '\n';
   
    for(auto it = c.begin(); it != c.end(); it++)
    	if (*it % 2) {
    		c.erase(it);
    	}

    for(auto it = c.begin(); it != c.end(); it++)
    	std::cout << *it << ' ';
    std::cout << '\n';
 	for(auto it = --c.end(); it != c.begin(); it--)
    	std::cout << *it << ' ';
    std::cout << *c.begin();
       
}