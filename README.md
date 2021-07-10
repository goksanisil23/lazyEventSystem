# lazyEventSystem
A minimal event system that can be used to communicate between different classes, without the classes knowing about each other. 

2 implementations are provided.

1) **EventSystem_FuncPtr** utilizes raw function pointers, which requires the templated member function handler class to template the class which owns the member functions, as well as templating the event type.

2) **EventSystem** utilizes std::function from C++11, which simplifies the signature of the member function handler class, by only templating the Event type, since the function object that is passed on to the member function handler class in construction already has an object.

## Build
```
g++ main.cpp EventSystem_FuncPtr.hpp -o event
```
or
```
g++ main_func_ptr.cpp EventSystem_FuncPtr.hpp -o event_func_ptr
```

## Usage
- Create your event struct which inherits from the abstract Event class:
```
struct CollisionEvent : public Event {
    CollisionEvent(uint8_t ent_1, uint8_t ent_2) 
        : entity_1{ent_1}, entity_2{ent_2} {}

    uint8_t entity_1;
    uint8_t entity_2;
};
```

- Initialize the global `EventBus` object, which is a medium where concrete events are matched with concrete callback functions. Therefore it's the main element enabling the communication between classes.
```
EventBus event_bus;
```

- Inside the class that is responsible of triggering the event, use the `publish` method of `EventBus`:
```
class PhysicsSystem {
    ...
    void CollisionUpdate() {
        CollisionEvent col_event(...);
        event_bus.publish<CollisionEvent>(col_event);
    }
    ...
};
```

- Inside the class that is interested in listening to the event above; 

    a) create the callback function that will be called once the event is triggered

    b) subscribe to the concrete event with the callback function created above.
```
class WarningSystem {
    ...
    void OnCollisionEvent(const CollisionEvent& collision) {...}

    void Init() {
        event_bus.subscribe<CollisionEvent>(std::bind(&WarningSystem::OnCollisionEvent, this, std::placeholders::_1));
    }
    ...
};
```