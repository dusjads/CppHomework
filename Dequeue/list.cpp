#include "list.h"
#include <iostream>
#include <cassert>

const int inf = 1e9 + 7;

list::node::node(list::value_type new_val, int version){
	val = new_val;
	node_version = version;
	next = nullptr;
	prev = nullptr;
}

list::list(){
	head = new node(inf, 0);
	tail = new node(-inf, 0);
	head->next = tail;
	tail->prev = head;
}

list::iterator::iterator(list& lst, node* node, int version_) : owner(&lst), cur(node), version(version_){}

// Элемент на который сейчас ссылается итератор.
// Разыменование итератора end() неопределено.
// Разыменование невалидного итератора неопределено.
list::value_type const& list::iterator::operator*() const{
	assert(cur != nullptr && version == cur->node_version && cur != owner->tail);
	return cur->val;
}

// Переход к элементу со следующим по величине ключом.
// Инкремент итератора end() неопределен.
// Инкремент невалидного итератора неопределен.
list::iterator& list::iterator::operator++(){
	assert(cur != owner->tail && version == cur->node_version);
	cur = cur->next;
	return *this;
}

list::iterator list::iterator::operator++(int){
	iterator prev = *this;
	++*this;
	return prev;
}

// Переход к элементу с предыдущим по величине ключом.
// Декремент итератора begin() неопределен.
// Декремент невалидного итератора неопределен.
list::iterator& list::iterator::operator--(){
	assert(cur != owner->head->next && version == cur->node_version);
	cur = cur->prev;
	return *this;
}

list::iterator list::iterator::operator--(int){
	iterator next = *this;
	--*this;
	return next;
}

bool list::iterator::equal(list::iterator const right){
	assert(version == cur->node_version && right.version == cur->node_version);
	return cur == right.cur;
}

bool operator==(list::iterator left, list::iterator right){
	return left.equal(right);
}

bool operator!=(list::iterator left, list::iterator right){
	return !left.equal(right);
}


list::list(list const& source){
	head = new node(inf, 0);
	node* cur = head;
	node* sou_cur = source.head;
	while (sou_cur != source.tail){
		cur->next = new node(sou_cur->next->val, 0);
		cur->next->prev = cur;
		cur = cur->next;
		sou_cur = sou_cur->next;
	}
	tail = cur;
}

// Изменяет this так, чтобы он содержал те же элементы, что и rhs.
// Инвалидирует все итераторы, принадлежащие list'у this, включая end().
list& list::operator=(list const& rhs){
	head->node_version++;
	node* cur = head;
	node* sou_cur = rhs.head;
	while (sou_cur != rhs.tail){
		cur->next = new node(sou_cur->next->val, head->node_version);
		cur->next->prev = cur;
		cur = cur->next;
		sou_cur = sou_cur->next;
	}
	tail = cur;
	return *this;
}

list::~list(){
	delete head;
	node* cur = head;
	while (cur != tail){
		node* next = cur->next;
		delete cur;
		cur = next;
	}
}

// Вставка элемента.
list::iterator list::insert(iterator it, value_type x){
	assert(it.version == it.cur->node_version);
	node* new_node = new node(x, head->node_version);
	node* cur = it.cur;
	new_node->prev = cur->prev;
	new_node->next = cur;
	cur->prev->next = new_node;
	cur->prev = new_node;
	iterator new_it(*this, new_node, head->node_version);
	return new_it;
}

list::iterator list::push_back(value_type x){
	iterator it(*this, tail, head->node_version);
	return insert(it, x);
}

list::iterator list::push_front(value_type x){
	iterator it(*this, head->next, head->node_version);
	return insert(it, x);
}

// Удаление элемента.
// Инвалидирует только итераторы на удаленный элемент.
void list::erase(list::iterator it){
	assert(it.version == it.cur->node_version);
	it.cur->prev->next = it.cur->next;
	it.cur->next->prev = it.cur->prev;
	it.cur->node_version = -1;
}

// Удаление элемента.
void list::pop_back(){
	assert(tail->prev != head);
	iterator it(*this, tail->prev, head->node_version);
	erase(it);
}

void list::pop_front(){
	assert(tail->prev != head);
	iterator it(*this, head->next, head->node_version);
	erase(it);
}

    
list::iterator list::begin(){
	assert(tail->prev != head);
	iterator it(*this, head->next, head->node_version);
	return it;
}

list::iterator list::end(){
	assert(tail->prev != head);
	iterator it(*this, tail, head->node_version);
	return it;
}

list::value_type list::front(){
	assert(tail->prev != head);
	return head->next->val;	
}

list::value_type list::back(){
	assert(tail->prev != head);
	return tail->prev->val;
}	

int main(){

	list lst;
	for (int i = 0; i < 10; i++){
		if (i % 2)
			lst.push_front(i);
		else
			lst.push_back(i);
	}
	for(auto it = lst.begin(); it != lst.end(); it++)
    	std::cout << *it << ' ';
    std::cout << '\n';
    std::cout << lst.front() << ' ' << lst.back() << '\n';
    auto it = lst.end();
    for(int i = 0; i < 10; i++)
    	lst.pop_back();
    lst.push_front(5);
    it--;
    std::cout << *it << '\n';
    /*auto it2 = lst.begin();
    lst.push_front(6);
    std::cout << *it << '\n';
    it--;
    lst.erase(it2);
    std::cout << *it << '\n';
    lst.pop_front();
    lst.push_front(6);*/
    //std::cout << *it2 << '\n';
    int i = 0;
    /*for (auto it = lst.begin(); it != lst.end(); it++){
    	std::cout << i++ << '\n';
    	lst.erase(it);
    }*/

//    std::cout << *it << '\n';
    
	return 0;
}





