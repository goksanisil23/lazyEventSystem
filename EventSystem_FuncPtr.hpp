#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <list>

struct Event {
    virtual ~Event() {};
};

// Base class to hold all specializations of MemberFunctionHandler in a container
class FunctionHandlerBase {
private:
    // To be implemented by MemberFunctionHandler
    virtual void call(Event* event) = 0;

public:
    // Call the member function
    void exec(Event* event) {
        call(event);
    }
};

// Template class to handle the member function (callback) in a concrete System, for a concrete Event.
// --------------- 1 MemberFunctionHandler per callback --------------- // 
template<class TSystem, class TEventType>
class MemberFunctionHandler : public FunctionHandlerBase {

public:
    // Generic member function pointer. Takes an Event as input, returns nothing. (Member function to be used as Event callback)
    //  function pointer signature: 
    //  return_type (*func_ptr_name)(input_argument_types)
    typedef void (TSystem::*MemberFunction)(TEventType*);

    // Constructor
    MemberFunctionHandler(TSystem* systemInstance, MemberFunction memberFunc) 
        : system_instance{systemInstance}, member_func{memberFunc} {};


    // Calls the relevant member function (callback) from the instance of the System
    void call(Event* event) override {
        // dereference the function pointer to call the member function, with the concrete Event type that this member function expects
        (system_instance->*member_func)(static_cast<TEventType*>(event));
    }

private:
    TSystem* system_instance; // pointer to System instance
    MemberFunction member_func; // pointer to System's member function to be used as event callback  
};


// ---------------------- EventBus ---------------------- // 
// For each type of Event, we will keep a list of handlers, to call the concrete callback functions in the System for that Event type
typedef std::list<FunctionHandlerBase*> FuncHandlerList;

class EventBus {

public:
    template<class TSystem, class TEventType>
    void subscribe(TSystem* systemInstance, void(TSystem::*memberFunc)(TEventType*)) {
        // we take the concrete System object, and it's associated member (callback) function for this type of Event as input parameters
        FuncHandlerList* func_handlers_per_event = subscriber_map[typeid(TEventType)];

        if(func_handlers_per_event == nullptr) {
            // 1st subscriber to this Event type
            func_handlers_per_event = new FuncHandlerList();
            subscriber_map[typeid(TEventType)] = func_handlers_per_event;
        }

        func_handlers_per_event->push_back(
            new MemberFunctionHandler<TSystem,TEventType>(systemInstance, memberFunc)
        );
    }

    // execute all callback functions that have subscribed to this type of concrete Event
    template<class TEventType>
    void publish(TEventType* event) {
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