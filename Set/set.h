#ifndef SET
#define SET

#include <vector>


struct set
{
    // Вы можете определить этот тайпдеф по вашему усмотрению.
    typedef int value_type;

    // Bidirectional iterator.
    struct iterator;
    struct node;

    // Создает пустой set.
    set();

    // Создает копию указанного set-а.
    set(set const&);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие set'у this, включая end().
    set& operator=(set const& rhs);

    // Деструктор. Вызывается при удалении объектов set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого set
    // (включая итераторы ссылающиеся на элемент следующий за последним).
    ~set();

    // Поиск элемента.
    // Возвращает итератор на найденный элемент, либо end(), если элемент
    // с указанным значением отсутвует.
    iterator find(value_type) const;

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    // Если вставка произведена, инвалидирует все итераторы, принадлежащие set'у this, включая end().
    std::pair<iterator, bool> insert(value_type);

    // Удаление элемента.
    // Инвалидирует все итераторы, принадлежащие set'у this, включая end().
    void erase(iterator);

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin() const;
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end() const;

    iterator appr_find(value_type) const;
    void erase_node(node*);
    


private:
    node* head;   
};

struct set::iterator
{

    friend set;
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


private:
    node* cur = nullptr;
    bool is_end = false;
};

struct set::node
{

    value_type val;
    node* left;
    node* right;
    node* parent;
    node(int new_val);
};

// Сравнение. Итераторы считаются эквивалентными если они ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
bool operator==(set::iterator, set::iterator);
bool operator!=(set::iterator, set::iterator);

#endif
