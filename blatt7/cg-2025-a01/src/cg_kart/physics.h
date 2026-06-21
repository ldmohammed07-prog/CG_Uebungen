#pragma once
#include <numeric>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "common.h"
#include "uniform_grid.h"
#include "rigid_body.h"
#include "scene.h"
#include "kart.h"

namespace mario_kart { 

    class Physics {
    public:
        Physics(const Scene &scene) : static_bodies(scene.scene_bounds, { 512, 512, 512 }), scene(scene) {}

        void apply_gravity(rigid_body &rb) {
        }

        struct Collision {
            glm::vec3 contact_normal;
            glm::vec3 contact_point;
        };
        std::vector<Collision> collisions;

        void update(float dt) {
            if(!dt)
                return;
            for (Kart* dynamic_object : dynamic_objects) {

                collisions.clear();
                dynamic_object->rb.grounded = false;

                //Calculate rigidbody updates
                dynamic_object->rb.integrate(dt);
                dynamic_object->update(dt);


                //Find any collisions
                AABB bounds;
                bounds.grow(dynamic_object->rb.position - glm::vec3(dynamic_object->bounds.extents()));
                bounds.grow(dynamic_object->rb.position + glm::vec3(dynamic_object->bounds.extents()));
                    const std::vector<RichGridCell*> collided_cells = static_bodies.find_potential_collisions(bounds);

                for (const RichGridCell *cell : collided_cells) {
                    for (const auto *triangle: cell->overlapping_triangles) {
                        const auto &vertices = triangle->parent->vertices;
                        const auto &a = vertices[triangle->indices.x];
                        const auto &b = vertices[triangle->indices.y];
                        const auto &c = vertices[triangle->indices.z];
                        auto [intersection, closest_point] = sphere_triangle_intersection(dynamic_object->colliders[0].transformed_center, dynamic_object->colliders[0].radius, a, b, c);
                        if (glm::length(closest_point) > 0.0f) {

                            Collision col;
                            col.contact_point = closest_point;
                            col.contact_normal = normalize(dynamic_object->colliders[0].transformed_center - closest_point);
                            collisions.push_back(col);

                        }
                    }
                }

                if (collisions.empty())
                    continue;

                dynamic_object->rb.grounded = true;

                for (int iteration = 0; iteration < 99; ++iteration) {
                    //Find most important Collision
                    float max = std::numeric_limits<float>::max();
                    int max_index = -1;
                    for (int i = 0; i < collisions.size(); ++i) {
                        float separating_velocity = dot(dynamic_object->rb.velocity, collisions[i].contact_normal);
                        if (separating_velocity < max && separating_velocity < 0)
                        {
                            max = separating_velocity;
                            max_index = i;
                        }
                    }
                    if (max_index == -1) {
                        dynamic_object->rb.calculate_derived_data();
                        dynamic_object->update_collider_positions();
                        break;
                    }

                    //Resolve Collision
                    
                    //Resolve Velocity
                    float separating_velocity = max;
                    float new_separating_velocity = -separating_velocity * 0.0f; //Restitution
                    glm::vec3 acceleration_caused_velocity = dynamic_object->rb.const_acceleration;
                    float acceleration_caused_separating_velocity = dot(acceleration_caused_velocity, collisions[max_index].contact_normal) * dt;

                    if (acceleration_caused_separating_velocity < 0) {
                        new_separating_velocity += 0.0f * acceleration_caused_separating_velocity; //Restitution
                        if (new_separating_velocity < 0) new_separating_velocity = 0;
                    }

                    float delta_velocity = new_separating_velocity - separating_velocity;
                    float impulse = delta_velocity / dynamic_object->rb.inverse_mass;
                    glm::vec3 impulse_per_inverse_mall = collisions[max_index].contact_normal * impulse;

                    dynamic_object->rb.velocity += impulse_per_inverse_mall * dynamic_object->rb.inverse_mass;

                    //Resolve Interpenetration
                    float penetration = dynamic_object->colliders[0].radius - distance(collisions[max_index].contact_point, dynamic_object->colliders[0].transformed_center);
                    if (penetration <= 0)
                        continue;

                    glm::vec3 movement_per_inverse_mass = collisions[max_index].contact_normal * (penetration/dynamic_object->rb.inverse_mass);
                    dynamic_object->rb.position += movement_per_inverse_mass * dynamic_object->rb.inverse_mass;

                    collisions.erase(collisions.begin() + max_index);

                    dynamic_object->rb.calculate_derived_data();
                    dynamic_object->update_collider_positions();

                }
            }
        }

        void add_static_mesh(const Mesh &mesh) {
            static_bodies.add(mesh.triangles);
        }

        void add_dynamic_mesh(Kart *kart) {
            dynamic_objects.push_back(kart);
        }

        void gravity(bool on) {
            gravitational_acceleration = on ? gravitational_acceleration_constant : glm::vec3(0.f);
        }

        static constexpr glm::vec3 gravitational_acceleration_constant = glm::vec3(0, -9.81, 0);
        glm::vec3 gravitational_acceleration = gravitational_acceleration_constant;
        
        float force_scale = 0.001f;
        std::vector<Kart*> dynamic_objects;
        UniformGrid<RichGridCell> static_bodies;
        const Scene& scene;
    };
}
