#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm.hpp>
#include "gtx/norm.hpp"
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <mutex>
#include <BugTools.hpp>

namespace Geometry
{

    struct Edge;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        bool used = false;
        std::vector<Edge*> edges;
    };

    enum EdgeState
    {
        ACTIVE,
        BOUNDARY,
        INNER,
        FROZEN
    };

    struct Face : std::array<Geometry::Vertex*, 3>
    {
        glm::vec3 normal() const 
        { 
            return glm::normalize(glm::cross((*this)[0]->position - (*this)[1]->position, (*this)[0]->position - (*this)[2]->position)); 
        }
    };

    struct Edge
    {
        Geometry::Vertex* start;
        Geometry::Vertex* end;
        Geometry::Vertex* across;
        EdgeState state;
        glm::vec3 sphere_center;

        Edge(Geometry::Vertex* _start, Geometry::Vertex* _end, Geometry::Vertex* _across, EdgeState _state, glm::vec3 _sphere_center)
        :
        start{_start},
        end{_end},
        across{_across},
        state{_state},
        sphere_center{_sphere_center}
        {

        }

        bool operator==(const Edge& other) const
        {
            return (start == other.start) &&
                (end == other.end);
        }
    };

    struct Mesh{
        std::list<Face> m_triangles;
        std::list<Edge> m_edges;
        std::list<Vertex> m_verts;
        std::mutex m_mutex;

        Mesh() = default;
        
        Mesh(const Mesh& other) {
            m_triangles = other.m_triangles;
            m_edges = other.m_edges;
            m_verts = other.m_verts;
        }

        void addFace(const Face& face) {
            LOG("addFace from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            m_triangles.push_back(face);
        }

        void addEdge(const Edge& edge) {
            LOG("addEdge from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            m_edges.push_back(edge);
        }

        void addVertex(const Vertex& vertex) {
            LOG("addVertex from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            m_verts.push_back(vertex);
        }

        std::list<Face> getFaces() {
            LOG("getFaces from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_triangles;
        }

        std::list<Edge> getEdges() {
            LOG("getEdges from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_edges;
        }

        std::list<Vertex> getVertices() {
            LOG("getVertices from " << std::this_thread::get_id());
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_verts;
        }
    };

    static bool ballIsEmpty(glm::vec3 ball, const std::vector<Vertex*> points, float radius) {
        //from
        //https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp
        return !std::any_of(begin(points), end(points), [&](Vertex* p) {
            return glm::length2(p->position - ball) < radius * radius - 1e-4f;
        });
    }

}

#endif //GEOMETRY_HPP