#include "EventSystem_FuncPtr.hpp"

// ****************** Events ****************** //

struct CollisionEvent : public Event {
    CollisionEvent(uint8_t ent_1, uint8_t ent_2) : entity_1{ent_1}, entity_2{ent_2} {}

    uint8_t entity_1;
    uint8_t entity_2;
};


// ****************** EventBus ****************** //

EventBus event_bus; // global event bus

// ****************** Systems ****************** //

class PhysicsSystem {
public:
    PhysicsSystem() = default;

    std::vector<uint8_t> entities;

    void CollisionUpdate() {
        for(int i = 0; i < entities.size() - 1; i++) {
            for(int j = i+1; j < entities.size(); j++) {
                if(entities.at(i) == entities.at(j)) { // check collision between 2 entities
                    // Publish the collision event
                    event_bus.publish(new CollisionEvent(i, j));
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
        event_bus.subscribe(this, &WarningSystem::OnCOllisionEvent);
    }

    // Member (callback) function for a concrete Event of type CollisionEvent
    void OnCOllisionEvent(CollisionEvent* collision) {
        std::cout << "Collision between element " << unsigned(collision->entity_1) << " and " << unsigned(collision->entity_2) << std::endl;
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