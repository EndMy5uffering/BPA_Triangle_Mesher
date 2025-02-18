#ifndef PBA_HPP
#define PBA_HPP

#include <Collider.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

#include <list>
#include <array>
#include <bitset>
#include <optional>

#include <glm.hpp>
#include <Grid.hpp>

//#define ASSERT(x) { if (!(x)) __debugbreak(); }
#define ASSERT(x, m) { if (!(x)) {std::cout << "[DEBUG_ASSERT]: " << m << "\n"; __debugbreak();} }
#define LOG(m) {std::cout << "[DEBUG_LOG (" << __LINE__ << " | " << __FILE__ <<")]: " << m << "\n";}

//https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp#L104
//https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d


namespace PBA
{

    typedef std::list<Geometry::Edge*> Front;
    typedef std::vector<Geometry::Vertex> VertList;

    static bool get_active_edge(Front& F, Geometry::Edge** e) 
    {
        while(!F.empty())
        {
            if((*e = F.back())->state == Geometry::ACTIVE)
                return true;
            F.pop_back();
        }
        return false;
    }

    static bool on_front(Geometry::Vertex* s)
    {
        for(const Geometry::Edge* e : s->edges)
        {
            if(e->state == Geometry::ACTIVE) return true;
        }
        return false;
    }

    static std::tuple<Geometry::Edge*, Geometry::Edge*> join(Geometry::Edge* e_ij, Geometry::Vertex* s_k, Front& F, std::list<Geometry::Edge>& edges, glm::vec3 sphere_center) 
    {
        //add edges e_ik, e_kj
        //remove edge e_ij <- its no longer used as we found a new triangle
        //e_ij is enclosed in the two triangles and no longer of interest
        
        auto& e_ik = edges.emplace_back(Geometry::Edge{e_ij->start, s_k, e_ij->end, Geometry::ACTIVE, sphere_center});
        auto& e_kj = edges.emplace_back(Geometry::Edge{s_k, e_ij->end, e_ij->start, Geometry::ACTIVE, sphere_center});

        e_ij->start->edges.push_back(&e_ik);
        e_ij->end->edges.push_back(&e_kj);

        s_k->edges.push_back(&e_ik);
        s_k->edges.push_back(&e_kj);

        F.push_back(&e_ik);
        F.push_back(&e_kj);

        e_ij->state = Geometry::INNER;
        return {&e_ik, &e_kj};
    }
    
    static void glue(Geometry::Edge* e0, Geometry::Edge* e1) 
    {
        e0->state = Geometry::INNER;
        e1->state = Geometry::INNER;
    }
    
    static Geometry::Edge* edge_rev_in_front(const Geometry::Edge* e) 
    {
        for(Geometry::Edge* i : e->start->edges)
        {
            if(i->end == e->start && i->start == e->end) return i;
        }
        return nullptr;
    }


    static std::tuple<Geometry::Edge*, Geometry::Edge*, Geometry::Edge*> output_triangle(Geometry::Face face, Geometry::Mesh& m, glm::vec3 sphere_center) 
    {
        face[0]->used = true;
        face[1]->used = true;
        face[2]->used = true;
        //Write that to the model
        m.m_triangles.emplace_back(face);
        Geometry::Edge& e0 = m.m_edges.emplace_back(Geometry::Edge{face[0], face[1], face[2], Geometry::ACTIVE, sphere_center});
        Geometry::Edge& e1 = m.m_edges.emplace_back(Geometry::Edge{face[1], face[2], face[0], Geometry::ACTIVE, sphere_center});
        Geometry::Edge& e2 = m.m_edges.emplace_back(Geometry::Edge{face[2], face[0], face[1], Geometry::ACTIVE, sphere_center});
        //std::cout << "[Log]: Found triangle ( " << s0 << ", " << s1 << ", " << s2 << " )\n";
        return {&e0, &e1, &e2};
    }

    static bool calc_sphere_center(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 normDir, float rad, glm::vec3* nspherePos)
    {
        //https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d
        glm::vec3 ac = c - a;
        glm::vec3 ab = b - a;
        glm::vec3 abXac = glm::cross(ab, ac);

        glm::vec3 toCircumsphereCenter = (glm::cross(abXac, ab) * glm::length2(ac) + glm::cross(ac, abXac) * glm::length2(ab)) / (2.f*glm::length2(abXac));
        glm::vec3 circumsphereCenter = a + toCircumsphereCenter;

        float h2 = rad * rad - glm::length2(toCircumsphereCenter);
        if (h2 < 0) return false;

        (*nspherePos) = circumsphereCenter + glm::normalize(normDir) * std::sqrt(h2);
        return true; 
    }

    static bool find_seed_triangle(Grid& grid, float radius, glm::vec3* sphere_pos, Geometry::Face* f)
    {
        //from
        //https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp
        std::cout << "Looking for seed\n";
        for(auto& cell : grid.getCells())
        {
            const auto avgNormal = glm::normalize(std::accumulate(begin(cell), end(cell), glm::vec3{}, [](glm::vec3 acc, const Geometry::Vertex* p) { return acc + p->normal; }));
            for (auto& p1 : cell) {
                if(p1->used) {continue;}
                auto neighborhood = grid.sphericalNeighborhood(p1->position, {p1->position});
                std::sort(
                    begin(neighborhood), end(neighborhood), [&](Geometry::Vertex* a, Geometry::Vertex* b) { return glm::length(a->position - p1->position) < glm::length(b->position - p1->position); });

                for (auto& p2 : neighborhood) {
                    if(p2->used) {continue;}
                    for (auto& p3 : neighborhood) {
                        if(p3->used) {continue;}
                        if (p2 == p3) continue;
                            
                        (*f) = {p1, p2, p3};
                        if (glm::dot(f->normal(), avgNormal) < 0) continue;

                        if (calc_sphere_center(p1->position, p2->position, p3->position, f->normal(), radius, sphere_pos) && Geometry::ballIsEmpty(*sphere_pos, neighborhood, radius)) {
                            
                            std::cout << "Found seed\n";
                            return true;
                        }
                    }
                }
            }
        }
        
        std::cout << "No seed\n";
        return false;
    }

    struct PivotResult
    {
        Geometry::Vertex* vert;
        glm::vec3 spherePos;
        bool valid = false;
    };

    static bool vertOnInnerEdge(Geometry::Vertex* vert, Geometry::Edge* edge)
    {
        for(Geometry::Edge* ee : vert->edges)
        {
            Geometry::Vertex* otherPoint = ee->start == vert ? ee->end : ee->start;
            if(ee->state == Geometry::INNER && (otherPoint == edge->start || otherPoint == edge->end))
            {
                return true;
            }
        }
        return false;
    }

    static PivotResult ball_pivote(Grid& grid, Geometry::Edge* edge, float rad)
    {
        Geometry::Vertex* v0 = edge->start;
        Geometry::Vertex* v1 = edge->end;
        glm::vec3 middle = (v0->position + v1->position) / 2.0f;
        std::vector<Geometry::Vertex*> neighbourhood = grid.sphericalNeighborhood(middle, {v0->position, v1->position, edge->across->position});

        glm::vec3 n_midSphere = glm::normalize(edge->sphere_center - middle);

        float smallest_angle = std::numeric_limits<float>::max();
        PivotResult result;
        for(Geometry::Vertex* vert : neighbourhood)
        {
            glm::vec3 fnorm = Geometry::Face{v1, v0, vert}.normal();

            if(glm::dot(fnorm, vert->normal) < 0) continue;

            glm::vec3 cc{0,0,0};
            if(!calc_sphere_center(v0->position, v1->position, vert->position, fnorm, rad, &cc))
                continue;
            
            glm::vec3 midNewCenter = glm::normalize(cc - middle);
            if(glm::dot(midNewCenter, fnorm) < 0) continue;

            if(vertOnInnerEdge(vert, edge)) continue;

            float angle = std::acos(std::clamp(glm::dot(n_midSphere, midNewCenter), -1.0f, 1.0f));
            if (glm::dot(glm::cross(midNewCenter, n_midSphere), edge->start->position - edge->end->position) < 0)
                angle += 3.14159274101257324219f;
            
            if(angle < smallest_angle)
            {
                smallest_angle = angle;
                result.vert = vert;
                result.spherePos = cc;
            }
            
        }

        if(smallest_angle != std::numeric_limits<float>::max() && 
            Geometry::ballIsEmpty(result.spherePos, neighbourhood, rad))
        {
            result.valid = true;    
        }

        return result;
    }


     /**
     * @brief Takes in a ref to a model and fills out the face data
     * 
     * @param model point cloud data set
     * @param p size of the p-ball
     */
    static Geometry::Mesh PivotBall(VertList& point_data, float p)
    {

        Grid grid{point_data, p};

        Front F;
        Geometry::Mesh m;

        bool found_seed = true;

        while(found_seed)
        {
            found_seed = false;
            glm::vec3 sphere_center;
            {
                Geometry::Face face;
                if(find_seed_triangle(grid, p, &sphere_center, &face))
                {
                    auto [e0, e1, e2] = output_triangle(face, m, sphere_center);
                    F.push_back(e0);
                    F.push_back(e1);
                    F.push_back(e2);
                    found_seed = true;
                }
            }

            Geometry::Edge* e_ij = nullptr;
            while(get_active_edge(F, &e_ij))
            {
                ASSERT(e_ij != nullptr, "e_ij was null")
                Geometry::Vertex* s_i = e_ij->start;
                Geometry::Vertex* s_j = e_ij->end;
                PivotResult pvRes = ball_pivote(grid, e_ij, p);
                ASSERT(pvRes.vert != nullptr, "pvRes was null")
                if(pvRes.valid && (!pvRes.vert->used || on_front(pvRes.vert)))
                {
                    Geometry::Vertex* s_k = pvRes.vert;
                    output_triangle({s_i, s_k, s_j}, m, pvRes.spherePos);
                    auto [e_ik, e_kj] = join(e_ij, s_k, F, m.m_edges, pvRes.spherePos);
                    if(auto e_ki = edge_rev_in_front(e_ik)) { glue(e_ik, e_ki); }
                    if(auto e_jk = edge_rev_in_front(e_kj)) { glue(e_kj, e_jk); }
                    std::cout << "Found Triangles: " << m.m_triangles.size() << "\n";
                }
                else
                {
                    e_ij->state = Geometry::BOUNDARY;
                    std::cout << "Found Boundary\n";
                }
            }

            
        }

        return m;
    }


}

#endif //PBA_HPP