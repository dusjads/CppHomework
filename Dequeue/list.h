#ifndef LIST
#define LIST

#include <vector>


struct list
{
    // Вы можете определить этот тайпдеф по вашему усмотрению.
    typedef int value_type;

    // Bidirectional iterator.
    struct iterator;
    struct node;

    // Создает пустой list.
    list();

    // Создает копию указанного list-а.
    list(list const&);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие list'у this, включая end().
    list& operator=(list const& rhs);

    // Деструктор. Вызывается при удалении объектов list.
    // Инвалидирует все итераторы ссылающиеся на элементы этого list
    // (включая итераторы ссылающиеся на элемент следующий за последним).
    ~list();

    // Вставка элемента.
    iterator insert(iterator, value_type);

    iterator push_back(value_type);
    iterator push_front(value_type);

    // Удаление элемента.
    // Инвалидирует все итераторы, принадлежащие list'у this, включая end().
    void erase(iterator);

    // Удаление элемента.
    void pop_back();
    void pop_front();
    
    iterator begin();
    iterator end();

    value_type front();
    value_type back();


private:
    node* head;
    node* tail;
};

struct list::iterator
{

    friend list;
    // Элемент на который сейчас ссылается итератор.
    // Разыменование итератора end() неопределено.
    // Разыменование невалидного итератора неопределено.
    value_type const& operator*() const;

    // Переход к элементу со следующим по величине ключом.
    // Инкремент итератора end() неопределен.
    // Инкремент невалидного итератора неопределен.
    iterator& operator++();
    iterator operator++(int);

    // Переход к элементу с предыдущим по величине ключом.
    // Декремент итератора begin() неопределен.
    // Декремент невалидного итератора неопределен.
    iterator& operator--();
    iterator operator--(int);
    bool equal(iterator const);

    iterator(list&, node*, int);


private:
    list* const owner;
    node* cur = nullptr;
    int version = 0;
};

struct list::node
{
    value_type val;
    node* prev;
    node* next;
    node(value_type new_val, int node_version);
    int node_version;
};

// Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
bool operator==(list::iterator, list::iterator);
bool operator!=(list::iterator, list::iterator);

#endif