#pragma once
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <bigduckgl/bigduckgl.h>

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     

#include "common.h"
#include "rigid_body.h"

constexpr float pi = M_PI;

namespace mario_kart {

    class Kart;

    struct Sphere_Collider {
        //Position before applying transforms
        glm::vec3 center;
        //Will get updated by the rigidbody transform in the update loop
        glm::vec3 transformed_center;
        float radius;
        //For debugging
        std::vector<drawelement_ptr> draw_elements;
    };

    class Kart {
    public:
        
        Kart(const glm::vec3 &forward_direction) : initial_forward(forward_direction) {
            rb.set_orientation(1.0f, 0.0f, 1.0f, 0.0f);
        }
        
        Kart(const Kart &other) = delete;
        
        void load_model_from_file(const std::string &path) {
            draw_elements = MeshLoader::load(path);

            Assimp::Importer importer;
            const aiScene *model = importer.ReadFile(path, aiProcess_Triangulate);
            meshes.reserve(model->mNumMeshes);
            
            for (unsigned int mesh_index = 0; mesh_index < model->mNumMeshes; ++mesh_index) {
                const aiMesh *ai_mesh = model->mMeshes[mesh_index];
                Mesh &mesh = meshes.emplace_back();
                mesh.vertices.reserve(ai_mesh->mNumVertices);

                for (unsigned int vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index) {
                    const aiVector3D vertex = ai_mesh->mVertices[vertex_index];
                    const glm::vec3 v(vertex.x, vertex.y, vertex.z);
                    mesh.vertices.push_back(v);
                    mesh.bounds.grow(v);
                }

                mesh.triangles.reserve(ai_mesh->mNumFaces);
                for (unsigned int face_index = 0; face_index < ai_mesh->mNumFaces; ++face_index) {
                    const aiFace &face = ai_mesh->mFaces[face_index];
                    assert(face.mNumIndices == 3);
                    mesh.triangles.emplace_back(glm::ivec3(face.mIndices[0], face.mIndices[1], face.mIndices[2]), &mesh);
                }
                bounds.grow(mesh.bounds);
                Sphere_Collider col;
                col.center = bounds.center();
                col.radius = bounds.extents().y / 2.0f;
                colliders.push_back(col);
            }
            int num_vertices = 0;
            glm::vec3 acc(0);
            for (const auto &mesh: meshes) {
                num_vertices += mesh.vertices.size();
                for (const auto &vertex: mesh.vertices) {
                    acc += vertex;
                }
            }
            rb.position = acc / static_cast<float>(num_vertices);
        }
        
        void update(float dt) {
            rb.calculate_derived_data();
            update_collider_positions();
        }

        void update_collider_positions() {
            for (auto& col : colliders) {
                col.transformed_center = rb.transform_matrix * glm::vec4(col.center, 1);
            }
        }

        glm::vec3 dt_aware_lerp(const glm::vec3 &source, const glm::vec3 &target, float t, float dt) {
            float corrected_t = 1 - powf(t, dt);
            return corrected_t * target + (1 - corrected_t) * source;
        }

        void update_attached_camera(float dt) {
            if (attached_camera) {

                if (!camera_initialized){
                    attached_camera->pos = rb.position - rotate(rb.orientation, initial_forward) * camera_z_offset + glm::vec3(0.0, 1.0, 0.0);
                    camera_look_at_target = rb.position + rotate(rb.orientation, initial_forward) * 10.0f + glm::vec3(0.0, 0.3, 0.0);
                    attached_camera->dir = normalize(camera_look_at_target - attached_camera->pos);
                    attached_camera->update();
                    camera_initialized = true;
                    return;
                }
                glm::vec3 target_pos = rb.position - rotate(rb.orientation, initial_forward) * camera_z_offset + glm::vec3(0.0, 1.0, 0.0);
                attached_camera->pos = dt_aware_lerp(attached_camera->pos, target_pos, 0.001, dt);
                glm::vec3 look_at_target = rb.position + rotate(rb.orientation, initial_forward) * 10.0f + glm::vec3(0.0, 0.3, 0.0);
                camera_look_at_target = dt_aware_lerp(camera_look_at_target, look_at_target, 0.001, dt);
                attached_camera->dir = normalize(camera_look_at_target - attached_camera->pos);
                attached_camera->update();
            }
            else {
                camera_initialized = false;
            }
        }

        void attach_camera(std::shared_ptr<Camera> camera) {
            std::swap(mem_default_camera_movement_speed, camera->default_camera_movement_speed);
            
            camera->dir = rotate(rb.orientation, initial_forward);
            camera->pos = rb.position - rotate(rb.orientation, initial_forward) * camera_z_offset;

            attached_camera = camera;
        }

        void detach_camera() {
            if (attached_camera)
                std::swap(mem_default_camera_movement_speed, attached_camera->default_camera_movement_speed);
            attached_camera = nullptr;
        }

        //Physics
        std::vector<Sphere_Collider> colliders;
        rigid_body rb;
        glm::vec3 initial_forward; 

        //Rendering
        AABB bounds;
        std::vector<Mesh> meshes;
        std::vector<drawelement_ptr> draw_elements;
        std::shared_ptr<Camera> attached_camera;
        glm::vec3 camera_look_at_target;
        float mem_default_camera_movement_speed = 0.f;
        float camera_z_offset = 3.f;
        float camera_y_offset = 1.f; 
        bool camera_initialized = false;
    };
}
