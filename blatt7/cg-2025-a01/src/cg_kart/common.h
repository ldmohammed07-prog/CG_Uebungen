#pragma once
#include <glm/common.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <functional>

inline std::ostream& operator << (std::ostream &out, const glm::vec3 &v) {
    return out << v.x << " " << v.y << " " << v.z;
}

namespace mario_kart {

    struct iAABB {
        iAABB(): min(INT_MAX), max(INT_MIN) {}

        void grow(const glm::ivec3 &v) {
            min.x = std::min(min.x, v.x);
            min.y = std::min(min.y, v.y);
            min.z = std::min(min.z, v.z);

            max.x = std::max(max.x, v.x);
            max.y = std::max(max.y, v.y);
            max.z = std::max(max.z, v.z);
        }

        glm::ivec3 min;
        glm::ivec3 max;
    };

    struct AABB {
        AABB() : min(FLT_MAX), max(FLT_MIN) {}
        
        glm::vec3 extents() const {
            return max - min;
        }

        AABB transform(const glm::mat4 &mat) const {
            AABB transformed;
        
            transformed.min = mat * glm::vec4(min, 1);
            transformed.max = mat * glm::vec4(max, 1);
            return transformed;
        }

        void grow(const AABB &box) {
            min.x = std::min(min.x, box.min.x);
            min.y = std::min(min.y, box.min.y);
            min.z = std::min(min.z, box.min.z);

            max.x = std::max(max.x, box.max.x);
            max.y = std::max(max.y, box.max.y);
            max.z = std::max(max.z, box.max.z);
        }

        void grow(const glm::vec3 &v) {
            min.x = std::min(min.x, v.x);
            min.y = std::min(min.y, v.y);
            min.z = std::min(min.z, v.z);

            max.x = std::max(max.x, v.x);
            max.y = std::max(max.y, v.y);
            max.z = std::max(max.z, v.z);
        }

        glm::vec3 center() const {
            return (min + max) * 0.5f;
        }

        friend std::ostream& operator << (std::ostream &out, const AABB &box) {
            return out << "(("<< box.min.x << ", " << box.min.y << ", " << box.min.z << "), " <<  "("<< box.max.x << ", " << box.max.y << ", " << box.max.z << "))";
        }

        bool contains(const glm::vec3 &point) {
            return min.x <= point.x && point.x <= max.x &&
                   min.y <= point.y && point.y <= max.y &&
                   min.z <= point.z && point.z <= max.z; 
        }

        glm::vec3 min;
        glm::vec3 max;
    };

    struct Mesh;
    struct Triangle {
        Triangle(const glm::ivec3 &indices, const Mesh *parent) : indices(indices), parent(parent) {}
        glm::ivec3 indices;
        const mario_kart::Mesh *parent;
        glm::vec3 normal() const;
        glm::vec3 center() const;
    };


    struct Mesh {
        Mesh() {}
        Mesh(const Mesh &other) : bounds(other.bounds), vertices(other.vertices), triangles(other.triangles) {
            std::for_each(triangles.begin(), triangles.end(), [this] (Triangle &t) {t.parent = this;});
        }
        Mesh& operator=(const Mesh &other) {
            bounds = other.bounds;
            vertices = other.vertices;
            triangles = other.triangles;
            std::for_each(triangles.begin(), triangles.end(), [this] (Triangle &t) {t.parent = this;});
            return *this;
        } 
        AABB bounds;
        std::vector<glm::vec3> vertices;
        std::vector<Triangle> triangles;
        
    };
    
    inline glm::vec3 Triangle::normal() const {
        const std::vector<glm::vec3> &vertices = parent->vertices;
        return glm::normalize(glm::cross(vertices[indices.y] - vertices[indices.x],vertices[indices.z] - vertices[indices.x]));
    }

    inline glm::vec3 Triangle::center() const {
        constexpr float one_third = 1.0f / 3.0f;
        const std::vector<glm::vec3> &vertices = parent->vertices;
        return vertices[indices.x] + vertices[indices.y] + vertices[indices.z] * one_third; 
    }

    inline bool box_triangle_intersection(const glm::vec3 &box_min, const glm::vec3 &box_max, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
        glm::vec3 v0, v1, v2, e0, e1, e2, norm;
        
        v0 = (a - box_min) / (box_max - box_min);
        v1 = (b - box_min) / (box_max - box_min);
        v2 = (c - box_min) / (box_max - box_min);
        e0 = v1 - v0;	
        e1 = v2 - v1;
        e2 = v0 - v2;

        norm.x = e0.y * e1.z - e0.z * e1.y;
        norm.y = e0.z * e1.x - e0.x * e1.z;
        norm.z = e0.x * e1.y - e0.y * e1.x;

        if (fabs(norm.x * (0.5f - v0.x) + norm.y * (0.5f - v0.y) + norm.z * (0.5f - v0.z)) > 0.5f * fabs(norm.x) + 0.5f * fabs(norm.y) + 0.5f * fabs(norm.z)) return false;

        glm::vec3 critical_point((norm.x < 0.0f) ? -1.0f : 1.0f, (norm.y < 0.0f) ? -1.0f : 1.0f, (norm.z < 0.0f) ? -1.0f : 1.0f);
        if (-(-e0.y * v0.x + e0.x * v0.y) * critical_point.z + std::max(0.0f, -e0.y * critical_point.z) + std::max(0.0f, e0.x * critical_point.z) < 0.0f) return false;
        if (-(-e1.y * v1.x + e1.x * v1.y) * critical_point.z + std::max(0.0f, -e1.y * critical_point.z) + std::max(0.0f, e1.x * critical_point.z) < 0.0f) return false;
        if (-(-e2.y * v2.x + e2.x * v2.y) * critical_point.z + std::max(0.0f, -e2.y * critical_point.z) + std::max(0.0f, e2.x * critical_point.z) < 0.0f) return false;
        if (-(-e0.x * v0.z + e0.z * v0.x) * critical_point.y + std::max(0.0f, -e0.x * critical_point.y) + std::max(0.0f, e0.z * critical_point.y) < 0.0f) return false;
        if (-(-e1.x * v1.z + e1.z * v1.x) * critical_point.y + std::max(0.0f, -e1.x * critical_point.y) + std::max(0.0f, e1.z * critical_point.y) < 0.0f) return false;
        if (-(-e2.x * v2.z + e2.z * v2.x) * critical_point.y + std::max(0.0f, -e2.x * critical_point.y) + std::max(0.0f, e2.z * critical_point.y) < 0.0f) return false;
        if (-(-e0.z * v0.y + e0.y * v0.z) * critical_point.x + std::max(0.0f, -e0.z * critical_point.x) + std::max(0.0f, e0.y * critical_point.x) < 0.0f) return false;
        if (-(-e1.z * v1.y + e1.y * v1.z) * critical_point.x + std::max(0.0f, -e1.z * critical_point.x) + std::max(0.0f, e1.y * critical_point.x) < 0.0f) return false;
        if (-(-e2.z * v2.y + e2.y * v2.z) * critical_point.x + std::max(0.0f, -e2.z * critical_point.x) + std::max(0.0f, e2.y * critical_point.x) < 0.0f) return false;

        return true;
    }

    //Returns the closest point on triangle to the origin
    inline glm::vec3 closest_point_on_triangle(const glm::vec3 &inA, const glm::vec3 &inB, const glm::vec3 &inC) {
        
		glm::vec3 a = inA;
		glm::vec3 c = inC;

        // Calculate normal
		glm::vec3 ab = inB - a;
		glm::vec3 ac = c - a;
		glm::vec3 n = cross(ab, ac);
		float n_len_sq = length(n) * length(n);

		// Check degenerate
		if (n_len_sq < 1.0e-10f) // Square(FLT_EPSILON) was too small and caused numerical problems, see test case TestCollideParallelTriangleVsCapsule
		{
            // Degenerate, fallback to vertices and edges
 
            // Start with vertex C being the closest
            glm::vec3 closest_point = inC;
            float best_dist_sq = length(inC) * length(inC);

            // Try vertex A
            float a_len_sq = length(inA) * length(inA);
            if (a_len_sq < best_dist_sq)
            {
                closest_point = inA;
                best_dist_sq = a_len_sq;
            }

            // Try vertex B
            float b_len_sq = length(inB) * length(inB);
            if (b_len_sq < best_dist_sq)
            {
                closest_point = inB;
                best_dist_sq = b_len_sq;
            }
 
            // Edge AC
            float ac_len_sq = length(ac) * length(ac);
            if (ac_len_sq > FLT_EPSILON * FLT_EPSILON)
            {
                float v = glm::clamp(glm::dot(-a, ac) / ac_len_sq, 0.0f, 1.0f);
                glm::vec3 q = a + v * ac;
                float dist_sq = length(q) * length(q);
                if (dist_sq < best_dist_sq)
                {
                    closest_point = q;
                    best_dist_sq = dist_sq;
                }
            }
 
            // Edge BC
            glm::vec3 bc = inC - inB;
            float bc_len_sq = length(bc) * length(bc);
            if (bc_len_sq > FLT_EPSILON * FLT_EPSILON)
            {
                float v = glm::clamp(dot(-inB, bc) / bc_len_sq, 0.0f, 1.0f);
                glm::vec3 q = inB + v * bc;
                float dist_sq = length(q) * length(q);
                if (dist_sq < best_dist_sq)
                {
                    closest_point = q;
                    best_dist_sq = dist_sq;
                }
            }

            // Edge AB
            ab = inB - inA;
            float ab_len_sq = length(ab) * length(ab);
            if (ab_len_sq > FLT_EPSILON * FLT_EPSILON)
            {
                float v = glm::clamp(dot(-inA,ab) / ab_len_sq, 0.0f, 1.0f);
                glm::vec3 q = inA + v * ab;
                float dist_sq = length(q) * length(q);
                if (dist_sq < best_dist_sq)
                {
                    closest_point = q;
                    best_dist_sq = dist_sq;
                }
            }
 
            return closest_point;
		}

		// Check if P in vertex region outside A
		glm::vec3 ap = -a;
		float d1 = dot(ab, ap);
		float d2 = dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f)
		{
			return a; // barycentric coordinates (1,0,0)
		}

		// Check if P in vertex region outside B
		glm::vec3 bp = -inB;
		float d3 = dot(ab, bp);
		float d4 = dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3)
		{
			return inB; // barycentric coordinates (0,1,0)
		}

		// Check if P in edge region of AB, if so return projection of P onto AB
		if (d1 * d4 <= d3 * d2 && d1 >= 0.0f && d3 <= 0.0f)
		{
			float v = d1 / (d1 - d3);
			return a + v * ab; // barycentric coordinates (1-v,v,0)
		}

		// Check if P in vertex region outside C
		glm::vec3 cp = -c;
		float d5 = dot(ab, cp);
		float d6 = dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6)
		{
			return c; // barycentric coordinates (0,0,1)
		}

		// Check if P in edge region of AC, if so return projection of P onto AC
		if (d5 * d2 <= d1 * d6 && d2 >= 0.0f && d6 <= 0.0f)
		{
			float w = d2 / (d2 - d6);
			return a + w * ac; // barycentric coordinates (1-w,0,w)
		}

		// Check if P in edge region of BC, if so return projection of P onto BC
		float d4_d3 = d4 - d3;
		float d5_d6 = d5 - d6;
		if (d3 * d6 <= d5 * d4 && d4_d3 >= 0.0f && d5_d6 >= 0.0f)
		{
			float w = d4_d3 / (d4_d3 + d5_d6);
			return inB + w * (c - inB); // barycentric coordinates (0,1-w,w)
		}

		// P inside face region.
		// Here we deviate from Christer Ericson's article to improve accuracy.
		// Determine distance between triangle and origin: distance = (centroid - origin) . normal / |normal|
		// Closest point to origin is then: distance . normal / |normal|
		// Note that this way of calculating the closest point is much more accurate than first calculating barycentric coordinates
		// and then calculating the closest point based on those coordinates.
		return n * dot((a + inB + c), n) / (3.0f * n_len_sq);
	} 

    inline std::pair<bool, glm::vec3> sphere_triangle_intersection(const glm::vec3 &center, const float radius, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
        glm::vec3 closest_point = closest_point_on_triangle(a - center, b - center, c - center) + center;
        if (distance(closest_point, center) <= radius)
            return std::make_pair(true, closest_point);
        else
            return std::make_pair(false, glm::vec3(0));
    }
}
