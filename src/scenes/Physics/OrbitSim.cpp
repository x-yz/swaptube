#pragma once

using namespace std;

#include <list>
#include <vector>
#include <glm/vec3.hpp> // Assuming you are using glm for vector operations
#include <algorithm>

class Object {
public:
    int color;
    float opacity;
    bool fixed;

    Object(int col, float op, bool fix) : color(col), opacity(op), fixed(fix) {}
};

class FixedObject : public Object {
public:
    const string dag_name;
    FixedObject(int col, float op, const string& dn)
        : Object(col, op, true), dag_name(dn) {}

    glm::vec3 get_position(const Dagger& dag) const {
        return glm::vec3(dag[dag_name + ".x"],
                         dag[dag_name + ".y"],
                         dag[dag_name + ".z"]);
    }
};

class MobileObject : public Object {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    MobileObject(const glm::vec3& pos, int col, float op)
        : Object(col, op, false), position(pos), velocity(0) {}
};

class OrbitSim {
public:
    float force_constant = 0.0001f;
    float collision_threshold = 0.02f;
    float drag = 0.99f;
    vector<FixedObject> fixed_objects;
    list<MobileObject> mobile_objects;
    bool mobile_interactions = true;

    void add_fixed_object(int color, float opacity, const string& dag_name) {
        fixed_objects.push_back(FixedObject(color, opacity, dag_name));
    }

    void add_mobile_object(const glm::vec3& position, int color, float opacity) {
        mobile_objects.push_back(MobileObject(position, color, opacity));
    }

    void iterate_physics(int multiplier, const Dagger& dag) {
        for (int step = 0; step < multiplier; ++step) iterate_physics_once(dag);
    }

    int predict_fate_of_object(const glm::vec3& o, const Dagger& dag) {
        MobileObject obj1(o, 0, 0);
        while(true){
            for (const auto& fixed_obj : fixed_objects) {
                glm::vec3 direction = fixed_obj.get_position(dag) - obj1.position;
                float distance = glm::length(direction);
                if (distance < collision_threshold) {
                    return fixed_obj.color;
                } else {
                    glm::vec3 acceleration = glm::normalize(direction) * magnitude_force_given_distance(distance);
                    obj1.velocity += acceleration;
                }
            }

            obj1.velocity *= drag;
            obj1.position += obj1.velocity;
        }
    }

private:
    void iterate_physics_once(const Dagger& dag) {
        // Interactions between fixed objects and mobile objects
        for (auto it = mobile_objects.begin(); it != mobile_objects.end(); /*it is incremented elsewhere*/) {
            auto& obj1 = *it;
            bool deleted = false;

            for (const auto& fixed_obj : fixed_objects) {
                glm::vec3 direction = fixed_obj.get_position(dag) - obj1.position;
                float distance = glm::length(direction);
                if (distance < collision_threshold) {
                    it = mobile_objects.erase(it);
                    deleted = true;
                    break;
                } else {
                    glm::vec3 acceleration = glm::normalize(direction) * magnitude_force_given_distance(distance);
                    obj1.velocity += acceleration;
                }
            }

            if (!deleted) {
                if (mobile_interactions) {
                    // Interactions between two mobile objects
                    for (auto it2 = std::next(it); it2 != mobile_objects.end(); it2++) {
                        auto& obj2 = *it2;
                        glm::vec3 direction = obj2.position - obj1.position;
                        float distance = glm::length(direction);
                        if (distance > 0) {
                            glm::vec3 acceleration = glm::normalize(direction) * magnitude_force_given_distance(distance);
                            obj1.velocity += acceleration;
                            obj2.velocity -= acceleration;
                        }
                    }
                }
                it++;
            }
        }

        for (auto& object : mobile_objects){
            object.velocity *= drag;
            object.position += object.velocity;
        }
    }

    float magnitude_force_given_distance(float d){
        return force_constant/(.1+d*d);
    }
};

