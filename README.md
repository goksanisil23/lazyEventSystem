# lazyEventSystem
A minimal event system that can be used to communicate between different classes. 

2 slightly different implementations are provided.
EventSystem_FuncPtr utilizes raw function pointers, which requires the templated member function handler class to template the class which owns the member functions.
EventSystem utilizes std::function from C++11, which simplifies the signature of the member function handler class, by only templating the Event type, since the function object that is passed on to the member function handler class in construction already has an object.

## Usage
```
g++ main.cpp EventSystem_FuncPtr.hpp -o event
```
or
```
g++ main_func_ptr.cpp EventSystem_FuncPtr.hpp -o event_func_ptr
```