#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <numeric>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "common.h"

//Source for most of this: https://www.snowblazed.com/rigidbody
namespace mario_kart {
    struct rigid_body {
        
        rigid_body() {}

        glm::vec3 position = glm::vec3(0);
        glm::vec3 velocity = glm::vec3(0);
        glm::vec3 acceleration = glm::vec3(0);
        glm::vec3 const_acceleration = glm::vec3(0, -9.81f, 0);
        float inverse_mass = 1.0f;

        glm::quat orientation = glm::quat(1,1,0,0);
        glm::vec3 angular_velocity = glm::vec3(0);
        glm::mat4x3 transform_matrix = glm::mat4x3(1);
        glm::mat3x3 inverse_inertia_tensor = glm::mat3x3(1);
        glm::mat3x3 inverse_inertia_tensor_world = glm::mat3x3(1);

        glm::vec3 force_accumulated = glm::vec3(0);
        glm::vec3 torque_accumulated = glm::vec3(0);

        float linear_damping = 0.5;
        float angular_damping = 0.8;

        float grounded = false;

        void set_orientation(const glm::quat& value) {
            orientation = normalize(value);
        }

        void set_orientation(const float w, const float x, const float y, const float z) {
            orientation = normalize(glm::quat(w, x, y, z));
        }

        void calculate_transform() {
            //Indexing: [col][row]
            transform_matrix[0][0] = 1.0f - 2.0f * orientation.y * orientation.y - 2.0f * orientation.z * orientation.z;
            transform_matrix[0][1] = 2.0f * orientation.x * orientation.y + 2.0f * orientation.z * orientation.w;
            transform_matrix[0][2] = 2.0f * orientation.x * orientation.z - 2.0f * orientation.y * orientation.w;

            transform_matrix[1][0] = 2.0f * orientation.x * orientation.y - 2.0f * orientation.z * orientation.w;
            transform_matrix[1][1] = 1.0f - 2.0f * orientation.x * orientation.x - 2.0f * orientation.z * orientation.z;
            transform_matrix[1][2] = 2.0f * orientation.y * orientation.z + 2.0f * orientation.x * orientation.w;

            transform_matrix[2][0] = 2.0f * orientation.x * orientation.z + 2.0f * orientation.y * orientation.w;
            transform_matrix[2][1] = 2.0f * orientation.y * orientation.z - 2.0f * orientation.x * orientation.w;
            transform_matrix[2][2] = 1.0f - 2.0f * orientation.x * orientation.x - 2.0f * orientation.y * orientation.y;

            transform_matrix[3][0] = position.x;
            transform_matrix[3][1] = position.y;
            transform_matrix[3][2] = position.z;
        }

        void transform_inertia_tensor() {
        float t4 = transform_matrix[0][0] * inverse_inertia_tensor[0][0] + transform_matrix[1][0] * inverse_inertia_tensor[0][1] + transform_matrix[2][0] * inverse_inertia_tensor[0][2];
        float t9 = transform_matrix[0][0] * inverse_inertia_tensor[1][0] + transform_matrix[1][0] * inverse_inertia_tensor[1][1] + transform_matrix[2][0] * inverse_inertia_tensor[1][2];
        float t14 = transform_matrix[0][0] * inverse_inertia_tensor[2][0] + transform_matrix[1][0] * inverse_inertia_tensor[2][1] + transform_matrix[2][0] * inverse_inertia_tensor[2][2];
        float t28 = transform_matrix[0][1] * inverse_inertia_tensor[0][0] + transform_matrix[1][1] * inverse_inertia_tensor[0][1] + transform_matrix[2][1] * inverse_inertia_tensor[0][2];
        float t33 = transform_matrix[0][1] * inverse_inertia_tensor[1][0] + transform_matrix[1][1] * inverse_inertia_tensor[1][1] + transform_matrix[2][1] * inverse_inertia_tensor[1][2];
        float t38 = transform_matrix[0][1] * inverse_inertia_tensor[2][0] + transform_matrix[1][1] * inverse_inertia_tensor[2][1] + transform_matrix[2][1] * inverse_inertia_tensor[2][2];
        float t52 = transform_matrix[0][2] * inverse_inertia_tensor[0][0] + transform_matrix[1][2] * inverse_inertia_tensor[0][1] + transform_matrix[2][2] * inverse_inertia_tensor[0][2];
        float t57 = transform_matrix[0][2] * inverse_inertia_tensor[1][0] + transform_matrix[1][2] * inverse_inertia_tensor[1][1] + transform_matrix[2][2] * inverse_inertia_tensor[1][2];
        float t62 = transform_matrix[0][2] * inverse_inertia_tensor[2][0] + transform_matrix[1][2] * inverse_inertia_tensor[2][1] + transform_matrix[2][2] * inverse_inertia_tensor[2][2];

        inverse_inertia_tensor_world[0][0] = t4 * transform_matrix[0][0] + t9 * transform_matrix[1][0] + t14 * transform_matrix[2][0];
        inverse_inertia_tensor_world[1][0] = t4 * transform_matrix[0][1] + t9 * transform_matrix[1][1] + t14 * transform_matrix[2][1];
        inverse_inertia_tensor_world[2][0] = t4 * transform_matrix[0][2] + t9 * transform_matrix[1][2] + t14 * transform_matrix[2][2];
        inverse_inertia_tensor_world[0][1] = t28 * transform_matrix[0][0] + t33 * transform_matrix[1][0] + t38 * transform_matrix[2][0];
        inverse_inertia_tensor_world[1][1] = t28 * transform_matrix[0][1] + t33 * transform_matrix[1][1] + t38 * transform_matrix[2][1];
        inverse_inertia_tensor_world[2][1] = t28 * transform_matrix[0][2] + t33 * transform_matrix[1][2] + t38 * transform_matrix[2][2];
        inverse_inertia_tensor_world[0][2] = t52 * transform_matrix[0][0] + t57 * transform_matrix[1][0] + t62 * transform_matrix[2][0];
        inverse_inertia_tensor_world[1][2] = t52 * transform_matrix[0][1] + t57 * transform_matrix[1][1] + t62 * transform_matrix[2][1];
        inverse_inertia_tensor_world[2][2] = t52 * transform_matrix[0][2] + t57 * transform_matrix[1][2] + t62 * transform_matrix[2][2];
        }
        
        void set_inertia_tensor(const glm::mat3x3& value) {
            inverse_inertia_tensor = inverse(value);
        }

        glm::mat3x3 get_inertia_tensor() {
            return inverse(inverse_inertia_tensor);
        }

        void set_inverse_inertia_tensor(const glm::mat3x3& inverse_value) {
            inverse_inertia_tensor = inverse_value;
        }

        glm::mat3x3 get_inertia_tensor_world() {
            return inverse(inverse_inertia_tensor_world);
        }

        void calculate_derived_data() {
            orientation = normalize(orientation);
            calculate_transform();
            transform_inertia_tensor();
        }

        void add_force(const glm::vec3& force) {
            force_accumulated += force;
        }

        void reset_force() {
            force_accumulated = glm::vec3(0);
        }

        void add_torque(const glm::vec3& torque) {
            torque_accumulated = torque;
        }

        void reset_torque() {
            torque_accumulated = glm::vec3(0);
        }

        void clear_accumulators() {
            reset_force();
            reset_torque();
        }

        void add_force_at_world_point(const glm::vec3& force, const glm::vec3& point) {
            glm::vec3 pt = point - position;
            force_accumulated += force;
            torque_accumulated += cross(pt, force);
        }

        void add_force_at_local_point(const glm::vec3& force, const glm::vec3& point) {
            add_force_at_world_point(force, get_local_point_in_worldspace(point));
        }

        glm::vec3 get_local_point_in_worldspace(const glm::vec3& point) {
            return transform_matrix * glm::vec4(point, 1);
        }

        void integrate(float delta_time) {
            if(inverse_mass <= 0.0) return;

            position += velocity * delta_time;
            orientation.x += angular_velocity.x * delta_time;
            orientation.y += angular_velocity.y * delta_time;
            orientation.z += angular_velocity.z * delta_time;

            acceleration = const_acceleration;
            acceleration += force_accumulated * inverse_mass;

            glm::vec3 angular_acceleration = inverse_inertia_tensor_world * torque_accumulated;
            velocity += acceleration * delta_time;
            angular_velocity += angular_acceleration * delta_time;

            velocity *= powf(linear_damping, delta_time);
            angular_velocity *= powf(angular_damping, delta_time);

            
            calculate_derived_data();
            clear_accumulators();
        }
    };
}
