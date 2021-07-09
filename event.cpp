#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>

struct Event {
    virtual ~Event() {};
};

// Base class to hold all specializations of MemberFunctionHandler in a container
class FunctionHandlerBase {
private:
    // To be implemented by MemberFunctionHandler
    virtual void call(const Event& event) = 0;

public:
    // Call the member function
    void exec(const Event& event) {
        call(event);
    }
};

// Template class to handle the member function (callback) of a concrete System, for a concrete Event.
// Note that MemberFunctionHandler does not have System as a template, since the constructor takes an std::function object,
// which is already tied to a concrete System's method
// --------------- 1 MemberFunctionHandler per callback --------------- // 
template<class TEventType>
class MemberFunctionHandler : public FunctionHandlerBase {

public:
    // Generic member function object. Takes an Event as input, returns nothing. (Member function to be used as Event callback)
    //  std::function signature:
    //  std::function<return_type(input_param_types) functor_name
    typedef std::function<void(const TEventType&)> MemberFunction;

    // Constructor
    MemberFunctionHandler(MemberFunction memberFunc) 
        : member_func{memberFunc} {};


    // Calls the member function (callback) from the instance of the System
    void call(const Event& event) override {
        // call the member function, with the concrete Event type that this member function expects
        member_func(static_cast<const TEventType&>(event));
    }

private:
    MemberFunction member_func; // member function of a concrete System object, to be used as an event callback  
};


// ---------------------- EventBus ---------------------- // 
// For each type of Event, we will keep a list of member func. handlers, to call the concrete callback functions in the System for that Event type
typedef std::vector<FunctionHandlerBase*> FuncHandlerList;

class EventBus {

public:
    template<class TEventType>
    void subscribe(std::function<void(const TEventType&)> memberFunc) {
        // we take the member (callback) function (that is tied to a concrete System object) for this type of Event as input parameter
        FuncHandlerList* func_handlers_per_event = subscriber_map[typeid(TEventType)];

        if(func_handlers_per_event == nullptr) {
            // 1st subscriber to this Event type
            func_handlers_per_event = new FuncHandlerList();
            subscriber_map[typeid(TEventType)] = func_handlers_per_event;
        }

        func_handlers_per_event->push_back(
            new MemberFunctionHandler<TEventType>(memberFunc)
        );
    }

    // execute all callback functions that have subscribed to this type of concrete Event
    template<class TEventType>
    void publish(const TEventType& event) {
        FuncHandlerList* func_handlers_per_event = subscriber_map[typeid(TEventType)];
        
        if(func_handlers_per_event == nullptr) {
            std::__throw_runtime_error("No subscribers for this type of event");
            return;
        }

        for(auto& func_handler : *func_handlers_per_event) {
            if(func_handler != nullptr) {
                func_handler->exec(event);
            }
            else {
                std::__throw_runtime_error("Tried to access null function handler!");
            }
        }
    }
    
private:
    std::unordered_map<std::type_index, FuncHandlerList*> subscriber_map; // set of callback functions, per Event type
};

// ****************** Events ****************** //

struct CollisionEvent : public Event {
    CollisionEvent(uint8_t ent_1, uint8_t ent_2) : entity_1{ent_1}, entity_2{ent_2} {}

    uint8_t entity_1;
    uint8_t entity_2;
};


// ****************** Systems ****************** //

EventBus event_bus; // global event bus

class PhysicsSystem {
public:
    PhysicsSystem() = default;

    std::vector<uint8_t> entities;

    void CollisionUpdate() {
        for(int i = 0; i < entities.size() - 1; i++) {
            for(int j = i+1; j < entities.size(); j++) {
                if(entities.at(i) == entities.at(j)) { // check collision between 2 entities
                    // Publish the collision event
                    event_bus.publish(CollisionEvent(i, j));
                }
            }
        }
    }
};

class WarningSystem {
public:
    WarningSystem() = default;

    // We register this WarningSystem to CollisionEvent subscribers
    void Init() {
        event_bus.subscribe<CollisionEvent>(std::bind(&WarningSystem::OnCollisionEvent, this, std::placeholders::_1));
        ////  OR ////
        // event_bus.subscribe<CollisionEvent>([this](const CollisionEvent& collEvent) {
        //     OnCollisionEvent(collEvent);
        // }
        // );
    }

    // Member (callback) function for a concrete Event of type CollisionEvent
    void OnCollisionEvent(const CollisionEvent& collision) {
        std::cout << "Collision between element " << unsigned(collision.entity_1) << " and " << unsigned(collision.entity_2) << std::endl;
    }
};

// ****************** Application ****************** //


int main() {

    // Create instances of the Systems
    PhysicsSystem physicsSys;
    WarningSystem warningSys;

    warningSys.Init();

    physicsSys.entities = {1,2,3,4,2,3};
    
    std::cout << "Entities:" << std::endl;
    for(const auto& ent : physicsSys.entities) {
        std::cout << unsigned(ent) << " ";
    }
    std::cout << std::endl;

    physicsSys.CollisionUpdate();


    return 0;
}