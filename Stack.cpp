#include <mutex>
#include <stack>

using namespace std;

template<class T>
class Stack {
public:
 Stack(){} ;
 void push(const T &value);
 T& pop();
 bool empty();
 size_t size();
private:
 mutex _mutex;
 condition_variable _cv;
 stack<T> _stack;
};


template<class T>
inline void Stack<T>::push(const T &value) {
 unique_lock < mutex > lock(_mutex);
 _stack.push(value);
 _cv.notify_one();
 return;
}

template<class T>
inline T& Stack<T>::pop() {
 unique_lock < mutex > lock(_mutex);
 _cv.wait(lock, [this] {
 return not _stack.empty();});
 T& value = _stack.top();
 _stack.pop();
 return value;
}

template<class T>
inline bool Stack<T>::empty() {
 unique_lock < mutex > lock(_mutex);
 return _stack.empty();
}

template<class T>
inline size_t Stack<T>::size() {
  unique_lock < mutex > lock(_mutex);
  return _stack.size();
}
