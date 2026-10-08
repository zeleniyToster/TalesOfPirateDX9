
#ifndef _FIFO_H_
#define _FIFO_H_

struct fifo_elem
    {
    fifo_elem() {prev = next = NULL;}
    ~fifo_elem() {prev = next = NULL;}

    fifo_elem* prev;
    fifo_elem* next;};

template <class T>
class fifo
    {
public:
    fifo() {_head = _tail = NULL;}
    ~fifo() {_head = _tail = NULL;}

    void push(T* t)
        { // push one element at the tail
        if (t == NULL) return;
        if (_head == NULL)
            { // the fifo is empty
            _head = t;
            _head->prev = NULL;
            _head->next = NULL;
            _tail = _head;}
        else{ // the fifo is not empty
            _tail->next = t;
            t->prev = _tail;
            t->next = NULL;
            _tail = t;}}

    T* pop()
        { // pop one element off the head
        T* tmp;
        if (_head == _tail)
            {
            if (_head == NULL) return NULL; // the fifo is empty
            else{ // exactly one element left in the fifo
                tmp = _head;
                _head = _tail = NULL;
                return tmp;}}
        else{ // more than one element left in the fifo; pop one
            tmp = _head;
            _head = (T *)_head->next;
            _head->prev = NULL;
            return tmp;}}

    int size() const
        { // length of the fifo
        int sz = 0;
        T* curr = _head;
        if (curr == NULL) return sz; // the fifo is empty
        while (curr != _tail)
            { // the fifo is not empty
            ++ sz; curr = (T *)curr->next;}
        return (sz + 1);}

protected:
    T* _head;
    T* _tail;};

#endif // _FIFO_H_
