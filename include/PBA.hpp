#ifndef PBA_HPP
#define PBA_HPP

#include <Collider.hpp>
#include <Geometry.hpp>
#include <MLib.hpp>
#include <ObjLoader.hpp>

#include <list>
#include <array>


namespace PBA
{


    enum EdgeState
    {
        ACTIVE,
        BOUNDARY,
        FROZEN
    };

    struct Edge
    {
        int start;
        int end;
        EdgeState state;

        bool operator==(const Edge& other) const
        {
            return (start == other.start) &&
                (end == other.end) &&
                (state == other.state);
        }
    };

    struct Triangle
    {
        int s0;
        int s1;
        int s2;

        bool operator<<(const int& s) const
        {
            return s0 == s || s1 == s || s2 == s;
        }
    };

    typedef std::list<Edge> Front;
    typedef std::list<Triangle> Mesh;

    static bool IsSeedTriangle(const MLib::Vec3& a, const MLib::Vec3& b, const MLib::Vec3& c, MLib::Vec3& sphere_center, float p)
    {
        double r2 = p * p;
        MLib::Vec3 p_1 = c + (a - c) * 0.5;
        MLib::Vec3 p_2 = c + (b - c) * 0.5;

        MLib::Vec3 n = MLib::cross((a-c),(b-c)).norm();

        MLib::Vec3 ca = a - c;
        MLib::Vec3 cao{-ca.y, ca.x, ca.z};
        cao = (cao - n*(MLib::dot(cao, n)/MLib::dot(n,n)) - ca*(MLib::dot(cao,ca)/MLib::dot(ca,ca))).norm();
        

        MLib::Vec3 cb = b - c;
        MLib::Vec3 cbo{-cb.y, cb.x, cb.z};
        cbo = (cbo - n*(MLib::dot(cbo, n)/MLib::dot(n,n)) - cb*(MLib::dot(cbo,cb)/MLib::dot(cb,cb))).norm();

        MLib::Vec3 poi;
        if(!MLib::LineLineIntersection(p_1, cao, p_2, cbo, poi)) return false;

        double dist = (poi - a).length2();

        if(dist > r2) return false;

        double ss = sqrt(r2 - dist);

        sphere_center = poi + n * ss;

        return true;
    }

    static bool ball_pivote(Edge* e, int& s) 
    {
        return false;
    }

    static bool get_active_edge(Front& F, Edge* e) 
    {
        if(F.empty()) return false;

        for(Edge& i : F)
        {
            if(i.state == ACTIVE) 
            {
                e = &i;
                return true;
            }
        }
        return false;
    }

    static bool not_used(uint32_t *usedIndex, int s)
    {
        return ((usedIndex[s / 32] >> (s % 32)) & 1) == 1;
    }
    
    static void mark_as_used(uint32_t *usedIndex, int s)
    {
        usedIndex[s / 32] |= (1 << (s % 32));
    }

    static bool on_front(Front& F, int s)
    {
        for(const Edge& e : F)
        {
            if(e.start == s || e.end == s) return true;
        }
        return false;
    }

    static void join(Edge* e_ij, int s_k, Front& F) 
    {
        //add edges e_ik, e_kj
        //remove edge e_ij <- its no longer used as we found a new triangle
        //e_ij is enclosed in the two triangles and no longer of interest
        Edge e_ik{e_ij->start, s_k, ACTIVE};
        Edge e_kj{s_k, e_ij->end, ACTIVE};
        F.push_back(e_ik);
        F.push_back(e_kj);
        F.remove(*e_ij);
    }
    
    static void glue(Edge& e, Front& F) 
    {
        Edge reverse{e.end, e.start, e.state};
        F.remove(e);
        F.remove(reverse);
    }
    
    static void mark_as_boundary(Edge* e) { e->state = BOUNDARY; }
    
    static bool find_seed_triangle(int& s0, int& s1, int& s2) 
    {
        return false;
    }

    static bool edge_in_front(const Edge& e, Front& F) 
    {
        for(const Edge& i : F)
        {
            if(i == e) return true;
        }
        return false;
    }


    static void insert_edge(Edge& e, Front& F) 
    {
        e.state = ACTIVE;
        F.push_back(e);
    }

    static void output_triangle(int s0, int s1, int s2, Mesh& m, uint32_t* usedIndex) 
    {
        mark_as_used(usedIndex, s0);
        mark_as_used(usedIndex, s1);
        mark_as_used(usedIndex, s2);

        //Write that to the model
        
        std::cout << "[Log]: Found triangle ( " << s0 << ", " << s1 << "," << s2 << " )\n";
    }


     /**
     * @brief Takes in a ref to a model and fills out the face data
     * 
     * @param model point cloud data set
     * @param p size of the p-ball
     */
    static void PivotBall(OBJ::Model& model, double p)
    {

        size_t verts = model.vertices.size();
        size_t buckets = verts/32;
        if(buckets == 0) buckets = 1;
        uint32_t *usedIndex = (uint32_t*)malloc((buckets)*sizeof(uint32_t));
        for(size_t i = 0; i < buckets; ++i) usedIndex[i] = 0;

        Front F;
        Mesh m;

        while(true)
        {
            Edge* e_ij = nullptr;
            while(get_active_edge(F, e_ij))
            {
                int s_k = -1; // index of vertex s_k in point cloud data
                int s_i = e_ij->start;
                int s_j = e_ij->end;
                if(ball_pivote(e_ij, s_k) && (not_used(usedIndex, s_k) || on_front(F, s_k)))
                {
                    output_triangle(s_i, s_k, s_j, m, usedIndex);
                    join(e_ij, s_k, F);
                    Edge e_ki{s_k, s_i, ACTIVE};
                    Edge e_jk{s_j, s_k, ACTIVE};
                    if(edge_in_front(e_ki, F)) glue(e_ki, F);
                    if(edge_in_front(e_jk, F)) glue(e_jk, F);
                }
                else
                {
                    //mark_as_boundary(e_ij);
                    e_ij->state = BOUNDARY;
                }
            }

            int s_i = -1, s_j = -1, s_k = -1;
            if(find_seed_triangle(s_i, s_j, s_k))
            {
                output_triangle(s_i, s_j, s_k, m, usedIndex);
                Edge e_ij{s_i, s_j, ACTIVE};
                Edge e_jk{s_j, s_k, ACTIVE};
                Edge e_ki{s_k, s_i, ACTIVE};
                insert_edge(e_ij, F);
                insert_edge(e_jk, F);
                insert_edge(e_ki, F);
            }
        }

        free(usedIndex);

    }


}

#endif //PBA_HPP