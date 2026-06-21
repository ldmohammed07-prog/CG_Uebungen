#pragma once
#include <vector>
#include "common.h"
#include <iostream>
#include <fstream>

namespace mario_kart {

    struct GridCell {
        GridCell() {}
        void on_overlapping_triangle(const Triangle &triangle) {
            overlapping_triangles.push_back(&triangle);
        }
        std::vector<const Triangle*> overlapping_triangles; 
    };

    struct RichGridCell : public GridCell {
        void on_overlapping_triangle(const Triangle &triangle) {
            GridCell::on_overlapping_triangle(triangle);
            const auto &vertices = triangle.parent->vertices;
            max_height = std::max(vertices[triangle.indices.x].y, std::max(vertices[triangle.indices.y].y, std::max(vertices[triangle.indices.z].y, max_height)));
            accumulated_normal += triangle.normal();
            accumulated_height += (vertices[triangle.indices.x].y + vertices[triangle.indices.y].y + vertices[triangle.indices.z].z) / 3.0f;

        }
        float max_height = -FLT_MAX;
        float accumulated_height = 0;
        glm::vec3 accumulated_normal = glm::vec3{0, 0, 0};
    };

    
    template <typename T = GridCell>
    class UniformGrid {
        public:
        UniformGrid(const AABB &grid_bounds, int x, int y, int z)
            : UniformGrid(grid_bounds, glm::ivec3(x, y, z)) {}

        UniformGrid(const AABB &grid_bounds, const glm::ivec3 &grid_dimensions)
            : bounds(grid_bounds),
              dimensions(grid_dimensions),
              cells(grid_dimensions.x * grid_dimensions.y * grid_dimensions.z) {}

        const std::vector<const Triangle*>* get_triangles_in_cell(const glm::vec3 &v) {
            const int linear_cell_index = linear_index(containing_cell_index(v));
            if (linear_cell_index < 0 || linear_cell_index >= dimensions.x * dimensions.y * dimensions.z) {
                return nullptr;
            }
            return &cells[linear_cell_index].overlapping_triangles;
        }

        void export_to_obj(const std::string &path) {
            const glm::vec3 voxel_extents = bounds.extents() / glm::vec3(dimensions);
            
            std::ofstream out(path);

            int vertex_index = 1;
            
            const auto write_vertex = [&out] (const glm::vec3 &v) {
                out << "v " << v.x << " " << v.y << " " << v.z << std::endl;
            };

            const auto write_triangle = [&] (const Triangle &t) {
                const auto &vertices = t.parent->vertices;
                write_vertex(vertices[t.indices.x]);
                write_vertex(vertices[t.indices.y]);
                write_vertex(vertices[t.indices.z]);

                out << "f " << vertex_index << " " << vertex_index + 1 << " " << vertex_index + 2 << std::endl;
                vertex_index += 3;
            };

            for (int x = 0; x < dimensions.x; ++x) {
                for (int y = 0; y < dimensions.y; ++y) {
                    for (int z = 0; z < dimensions.z; ++z) {
                        glm::vec3 bottom_left_front(bounds.min.x + x * voxel_extents.x,
                                                    bounds.min.y + y * voxel_extents.y,
                                                    bounds.min.z + z * voxel_extents.z);
                        glm::vec3 bottom_right_front = bottom_left_front + glm::vec3(voxel_extents.x, 0, 0);
                        glm::vec3 bottom_left_back = bottom_left_front + glm::vec3(0, 0, voxel_extents.z);
                        glm::vec3 bottom_right_back = bottom_left_front + glm::vec3(voxel_extents.x, 0, voxel_extents.z);
                        
                        glm::vec3 top_left_front  = bottom_left_front + glm::vec3(0, voxel_extents.y, 0);
                        glm::vec3 top_left_back   = top_left_front + glm::vec3(0, 0, voxel_extents.z);
                        glm::vec3 top_right_front = top_left_front + glm::vec3(voxel_extents.x, 0, 0);
                        glm::vec3 top_right_back  = top_left_front + glm::vec3(voxel_extents.x, 0, voxel_extents.z);
                        
                        write_vertex(bottom_left_front);
                        write_vertex(bottom_right_front);
                        write_vertex(bottom_left_back);
                        write_vertex(bottom_right_back);

                        write_vertex(top_left_front);
                        write_vertex(top_right_front);
                        write_vertex(top_left_back);
                        write_vertex(top_right_back);
                        
                        out << "o voxel_" << x << "_" << y << "_" << z << std::endl;

                        out << "l " << vertex_index << " " << vertex_index + 1 << std::endl;
                        out << "l " << vertex_index << " " << vertex_index + 2 << std::endl;
                        out << "l " << vertex_index << " " << vertex_index + 4 << std::endl; 
                        out << "l " << vertex_index + 3 << " " << vertex_index + 1 << std::endl;
                        out << "l " << vertex_index + 3 << " " << vertex_index + 2 << std::endl;
                        out << "l " << vertex_index + 3 << " " << vertex_index + 7 << std::endl;
                        
                        out << "l " << vertex_index + 4 << " " << vertex_index + 5 << std::endl;
                        out << "l " << vertex_index + 4 << " " << vertex_index + 6 << std::endl;
                        out << "l " << vertex_index + 7 << " " << vertex_index + 6 << std::endl;
                        out << "l " << vertex_index + 7 << " " << vertex_index + 5 << std::endl;
                        
                        out << "l" << vertex_index + 2 << " " << vertex_index + 6 << std::endl;
                        out << "l" << vertex_index + 1 << " " << vertex_index + 5 << std::endl;
                         
                                  
                        vertex_index += 8;

                        const int linear_grid_index = linear_index({x, y, z});

                        for (const Triangle *triangle: cells[linear_grid_index].overlapping_triangles) {
                            write_triangle(*triangle);    
                        }
                    }
                }
            }

        }

        void add(const std::vector<Triangle> &triangles) {
            add(triangles.data(), triangles.size());
        }
        
        void add(const Triangle *triangles, unsigned int num_triangles) {
            const auto clamp_cell = [this] (const glm::vec3 &cell) {
                return glm::vec3(cell.x > dimensions.x - 1 ? dimensions.x - 1 : cell.x,
                                 cell.y > dimensions.y - 1 ? dimensions.y - 1 : cell.y,
                                 cell.z > dimensions.z - 1 ? dimensions.z - 1 : cell.z);
            };

            const glm::vec3 voxel_extents = bounds.extents() / glm::vec3(dimensions);

            for (unsigned int i = 0; i < num_triangles; ++i) {
                const Triangle &triangle = triangles[i];
                const std::vector<glm::vec3> &vertices = triangle.parent->vertices;
                
                iAABB voxel_aabb;
                voxel_aabb.grow(clamp_cell(containing_cell_index(vertices[triangle.indices.x])));
                voxel_aabb.grow(clamp_cell(containing_cell_index(vertices[triangle.indices.y])));
                voxel_aabb.grow(clamp_cell(containing_cell_index(vertices[triangle.indices.z])));

                for (int x = voxel_aabb.min.x; x <= voxel_aabb.max.x; ++x)
                    for (int y = voxel_aabb.min.y; y <= voxel_aabb.max.y; ++y)
                        for (int z = voxel_aabb.min.z; z <= voxel_aabb.max.z; ++z) {
                            const glm::vec3 box_min(bounds.min + glm::vec3(x, y, z) * voxel_extents);
                            if (box_triangle_intersection(box_min, box_min + voxel_extents, vertices[triangle.indices.x],  vertices[triangle.indices.y],  vertices[triangle.indices.z])) {
                                const int linear_grid_index = linear_index({x,y,z});
                                cells[linear_grid_index].on_overlapping_triangle(triangles[i]);
                            }
                        }
            }
        };

        std::vector<T*> find_potential_collisions(const AABB &bounds) {
            
            std::vector<T*> collided_cells;

            iAABB voxel_aabb;
            voxel_aabb.grow(containing_cell_index(bounds.min));
            voxel_aabb.grow(containing_cell_index(bounds.max));
            
            //std::cout << voxel_aabb.min << " " << voxel_aabb.max << std::endl;

            for (int x = voxel_aabb.min.x; x <= voxel_aabb.max.x; ++x)
                for (int y = voxel_aabb.min.y; y <= voxel_aabb.max.y; ++y)
                    for (int z = voxel_aabb.min.z; z <= voxel_aabb.max.z; ++z) {
                        if (x >= 0 && x < dimensions.x && y >= 0 && y < dimensions.y && z >= 0 && z < dimensions.z) {
                            collided_cells.push_back(&cells[linear_index({x,y,z})]);
                        }
                    }
            return collided_cells;
        }

        int linear_index(const glm::ivec3 &grid_index) {
            return grid_index.z * (dimensions.x * dimensions.y) +
                   grid_index.y * dimensions.x + grid_index.x;
        }

        glm::ivec3 containing_cell_index(const glm::vec3 &vertex) {
            const glm::vec3 voxel_extents = bounds.extents() / glm::vec3(dimensions); 
            const glm::vec3 shifted_vertex = vertex - bounds.min;
            return glm::ivec3(shifted_vertex.x / voxel_extents.x,
                              shifted_vertex.y / voxel_extents.y,
                              shifted_vertex.z / voxel_extents.z);       
        }
        
        AABB bounds;
        glm::ivec3 dimensions;
        std::vector<T> cells;
    };
}
