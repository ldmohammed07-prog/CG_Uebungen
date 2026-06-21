#pragma once
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

namespace mario_kart {

    struct SceneObject {
        SceneObject(const std::string &name) : name(name) {}
        SceneObject(const SceneObject &other): name(other.name), mesh(other.mesh) {}
        std::string name;
        Mesh mesh;
    };

    class Scene {

    public:
        Scene() {}
        Scene(const Scene &other) = delete;
        SceneObject* find_object_by_name(const std::string &name) {
            const auto it = std::find_if(scene_objects.begin(), scene_objects.end(), 
                [&name](const SceneObject& scene_object) { 
                    return scene_object.name == name;
                }
            );

            if (it == scene_objects.end())
                return nullptr;
            return it.base();
        }

        void load_model_from_file(const std::string &path) {
            draw_elements = MeshLoader::load(path);

            Assimp::Importer importer;
            const aiScene *model = importer.ReadFile(path, aiProcess_Triangulate);
            
            scene_objects.reserve(model->mNumMeshes);
            SceneObject *s_ptr = scene_objects.data();
            for (unsigned int mesh_index = 0; mesh_index < model->mNumMeshes; ++mesh_index) {
                const aiMesh *ai_mesh = model->mMeshes[mesh_index];
                SceneObject &scene_object = scene_objects.emplace_back(ai_mesh->mName.C_Str());
                scene_object.mesh.vertices.reserve(ai_mesh->mNumVertices);
                
                for (unsigned int vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index) {
                    const aiVector3D vertex = ai_mesh->mVertices[vertex_index];
                    const glm::vec3 v(vertex.x, vertex.y, vertex.z);
                    scene_object.mesh.vertices.push_back(v);
                    scene_object.mesh.bounds.grow(v);
                }
                scene_bounds.grow(scene_object.mesh.bounds);
                
                scene_object.mesh.triangles.reserve(ai_mesh->mNumFaces);
                Triangle* ptr = scene_object.mesh.triangles.data();
                for (unsigned int face_index = 0; face_index < ai_mesh->mNumFaces; ++face_index) {
                    const aiFace &face = ai_mesh->mFaces[face_index];
                    assert(face.mNumIndices == 3);
                    assert(face.mIndices[0] < scene_object.mesh.vertices.size());
                    assert(face.mIndices[1] < scene_object.mesh.vertices.size());
                    assert(face.mIndices[2] < scene_object.mesh.vertices.size());
                    
                    scene_object.mesh.triangles.emplace_back(
                        glm::ivec3(face.mIndices[0], face.mIndices[1], face.mIndices[2]), 
                        &scene_object.mesh);
                    assert(ptr == scene_object.mesh.triangles.data());
                }
                assert(s_ptr == scene_objects.data());
                           
            }
            /*for (auto &obj: scene_objects){
                for (auto &tri: obj.mesh.triangles) {
                    std::cout << tri.parent->vertices.size();
                }
            }*/
        }

        AABB scene_bounds;
        std::vector<drawelement_ptr> draw_elements;
        std::vector<SceneObject> scene_objects;
    };
}
