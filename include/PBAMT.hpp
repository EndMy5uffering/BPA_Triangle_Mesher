#ifndef PBAMT_HPP
#define PBAMT_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

#include <list>
#include <array>
#include <bitset>
#include <optional>

#include <glm.hpp>
#include <Grid.hpp>
#include <BugTools.hpp>
#include <ThreadPool.hpp>


#include <random>
#include <algorithm>

//https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp#L104
//https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d


namespace PBAMT
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

    static bool find_seed_triangle(glm::ivec3& cellidx, Grid& grid, float radius, float margin, glm::vec3* sphere_pos, Geometry::Face* f)
    {
        //from
        //https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp

        Cell& cell = grid.cell(cellidx);

        LOG("Looking for seed");
        const auto avgNormal = glm::normalize(std::accumulate(begin(cell), end(cell), glm::vec3{}, [](glm::vec3 acc, const Geometry::Vertex* p) { return acc + p->normal; }));
            for (Geometry::Vertex* p1 : cell) {
                ASSERT(p1 != nullptr, "p1 was null")
                if(p1->used) {continue;}
                auto neighborhood = grid.sphericalNeighborhood(p1->position, {p1->position});
                std::sort(
                    begin(neighborhood), end(neighborhood), [&](Geometry::Vertex* a, Geometry::Vertex* b) { return glm::length(a->position - p1->position) < glm::length(b->position - p1->position); });

                for (Geometry::Vertex* p2 : neighborhood) {
                    ASSERT(p2 != nullptr, "p2 was null")
                    if(p2->used) {continue;}
                    for (Geometry::Vertex* p3 : neighborhood) {
                        ASSERT(p3 != nullptr, "p3 was null")
                        if(p3->used) {continue;}
                        if (p2 == p3) continue;
                        if(
                            !grid.isPointWithinCellWithMargin(p1->position, cellidx, margin) ||
                            !grid.isPointWithinCellWithMargin(p2->position, cellidx, margin) ||
                            !grid.isPointWithinCellWithMargin(p3->position, cellidx, margin)) continue;

                        (*f) = {p1, p2, p3};
                        if (glm::dot(f->normal(), avgNormal) < 0) continue;

                        if (calc_sphere_center(p1->position, p2->position, p3->position, f->normal(), radius, sphere_pos) && Geometry::ballIsEmpty(*sphere_pos, neighborhood, radius)) {
                            
                            LOG("Found seed")
                            return true;
                        }
                    }
                }
            }
        
        LOG("No seed");
        return false;
    }

    struct PivotResult
    {
        Geometry::Vertex* vert;
        glm::vec3 spherePos;
        bool valid = false;
        bool outsideMargin = false;
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

    static PivotResult ball_pivote(Grid& grid, glm::ivec3 cellidx, Geometry::Edge* edge, float rad, float margin)
    {
        Geometry::Vertex* v0 = edge->start;
        Geometry::Vertex* v1 = edge->end;

        ASSERT(v0 != nullptr, "v0 was null")
        ASSERT(v1 != nullptr, "v1 was null")

        ASSERT(grid.isPointWithinCellWithMargin(v0->position, cellidx, margin), "Start point not in cell");
        ASSERT(grid.isPointWithinCellWithMargin(v1->position, cellidx, margin), "End point not in cell");

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
        ASSERT(result.vert != nullptr, "result vert was null")
        result.outsideMargin = result.valid && !grid.isPointWithinCellWithMargin(result.vert->position, cellidx, margin);

        return result;
    }

    template <typename T>
    static std::vector<T> getRandomSubset(const std::vector<T>& vec, size_t subsetSize) {
        if (subsetSize > vec.size()) {
            throw std::runtime_error("Subset size cannot be larger than the original vector size!");
        }

        std::vector<T> shuffledVec = vec; // Copy the vector to avoid modifying the original
        static std::random_device rd; 
        static std::mt19937 gen(rd()); 

        std::shuffle(shuffledVec.begin(), shuffledVec.end(), gen); // Shuffle elements

        return std::vector<T>(shuffledVec.begin(), shuffledVec.begin() + subsetSize);
    }


     /**
     * @brief Takes in a ref to a model and fills out the face data
     * 
     * @param model point cloud data set
     * @param p size of the p-ball
     */
    static Geometry::Mesh PivotBall(VertList& point_data, float p, float margin, size_t threads)
    {

        Grid grid{point_data, p};
        Geometry::Mesh m;

        std::array<std::vector<glm::ivec3>, 8> idxlist = grid.getThreadCellList();

        ThreadPool tpool{threads};
        
        auto task = [&grid, &m, margin, p](glm::ivec3 cellidx){
            Front F;
            bool found_seed = true;
            while(found_seed)
            {
                found_seed = false;
                glm::vec3 sphere_center;
                {
                    Geometry::Face face;
                    if(find_seed_triangle(cellidx, grid, p, margin, &sphere_center, &face))
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
                    PivotResult pvRes = ball_pivote(grid, cellidx, e_ij, p, margin);
                    ASSERT(pvRes.vert != nullptr, "pvRes was null")
                    if(pvRes.valid && !pvRes.outsideMargin && (!pvRes.vert->used || on_front(pvRes.vert)))
                    {
                        Geometry::Vertex* s_k = pvRes.vert;
                        ASSERT(s_k != nullptr, "s_k was null");
                        ASSERT(s_j != nullptr, "s_j was null");
                        ASSERT(s_i != nullptr, "s_i was null");
                        ASSERT(grid.isPointWithinCellWithMargin(s_i->position, cellidx, margin), "Point_s_i not in cell");
                        ASSERT(grid.isPointWithinCellWithMargin(s_j->position, cellidx, margin), "Point_s_j not in cell");
                        ASSERT(grid.isPointWithinCellWithMargin(s_k->position, cellidx, margin), "Point_s_k not in cell");

                        output_triangle({s_i, s_k, s_j}, m, pvRes.spherePos);
                        auto [e_ik, e_kj] = join(e_ij, s_k, F, m.m_edges, pvRes.spherePos);
                        if(auto e_ki = edge_rev_in_front(e_ik)) { glue(e_ik, e_ki); }
                        if(auto e_jk = edge_rev_in_front(e_kj)) { glue(e_kj, e_jk); }
                        LOG("Found Triangles: " << m.m_triangles.size())
                    }
                    else if(pvRes.outsideMargin && pvRes.valid) 
                    { 
                        LOG("Valid but outside of margin")
                        e_ij->state = Geometry::FROZEN;
                    }
                    else
                    {
                        e_ij->state = Geometry::BOUNDARY;
                        LOG("Found Boundary")
                    }
                }

                
            }

        };

        int k = 2;
        for(size_t i = 0; i < 8; ++i)
        {
           for(auto& idx : idxlist[i])
           {
               //tpool.enqueue(
               //    task , idx
               //);
               task(idx);
               k--;
               if(k <= 0) break;
           }
           break;
        }

        /* for(auto& idx : getRandomSubset(idxlist[0], 8))
            {
                tpool.enqueue(
                    task , idx
                );
            } */
        
        //tpool.stop();

        glm::ivec3 dims = grid.getDims();
        LOG("GRID: DIM [ " << dims.x << "; " << dims.y << "; " << dims.z << " ]");

        return m;
    }

}

#endif //PBAMT_HPP