#ifndef PBA_HPP
#define PBA_HPP

#include <Collider.hpp>
#include <Geometry.hpp>
#include <MLib.hpp>
#include <ObjLoader.hpp>

#include <list>
#include <array>

//#define ASSERT(x) { if (!(x)) __debugbreak(); }
#define ASSERT(x, m) { if (!(x)) {std::cout << "[DEBUG_ASSERT]: " << m << "\n"; __debugbreak();} }
#define LOG(m) {std::cout << "[DEBUG_LOG (" << __LINE__ << " | " << __FILE__ <<")]: " << m << "\n";}

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
        int across;
        EdgeState state;
        MLib::Vec3 sphere_center;

        Edge(int _start, int _end, int _across, EdgeState _state, MLib::Vec3 _sphere_center)
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
                (end == other.end) &&
                (state == other.state) &&
                (across == other.across) &&
                (sphere_center == other.sphere_center);
        }
    };

    typedef std::list<Edge> Front;
    typedef std::list<Geometry::Triangle> Mesh;
    typedef std::vector<Geometry::Vertex> VertList;

    static bool IsSeedTriangle(const Geometry::Vertex& a, const Geometry::Vertex& b, const Geometry::Vertex& c, MLib::Vec3& sphere_center, double p)
    {
        double r2 = p * p;
        MLib::Vec3 p_1 = (a.position + c.position) * 0.5;
        MLib::Vec3 p_2 = (b.position + c.position) * 0.5;

        MLib::Vec3 n = MLib::cross((a.position-c.position),(b.position-c.position)).norm();

        if(n.dot(((a.normal + b.normal + c.normal)*(1.0/3.0))) < 0.0) n = n * (-1.0);

        MLib::Vec3 ca = a.position - c.position;
        MLib::Vec3 cao{-ca.y, ca.x, ca.z};
        cao = (cao - n*(MLib::dot(cao, n)/MLib::dot(n,n)) - ca*(MLib::dot(cao,ca)/MLib::dot(ca,ca))).norm();
        

        MLib::Vec3 cb = b.position - c.position;
        MLib::Vec3 cbo{-cb.y, cb.x, cb.z};
        cbo = (cbo - n*(MLib::dot(cbo, n)/MLib::dot(n,n)) - cb*(MLib::dot(cbo,cb)/MLib::dot(cb,cb))).norm();

        MLib::Vec3 poi;
        if(!MLib::LineLineIntersection(p_1, cao, p_2, cbo, poi)) return false;

        double dist = (poi - a.position).length2();

        if(dist > r2) return false;

        double ss = sqrt(r2 - dist);

        sphere_center = poi + n * ss;

        return true;
    }

    static bool ball_pivote(const VertList& point_data, const Edge* e, int& o_s_k, MLib::Vec3& o_sphere_center, double p) 
    {
        //https://gamedev.stackexchange.com/questions/75756/sphere-sphere-intersection-and-circle-sphere-intersection

        MLib::Vec3 c_c = (point_data[e->start].position + point_data[e->end].position)*0.5;
        double r_c = (e->sphere_center - c_c).length();

        MLib::Vec3 n_c = (point_data[e->start].position - point_data[e->end].position).norm();

        std::vector<size_t> verts; // spacial query to find 2p neighbourhood
        for(size_t i = 0; i < point_data.size(); ++i)
        {
            if((e->sphere_center - point_data[i].position).length() <= 2*p)
            {
                verts.push_back(i);
            }
        }

        int idx_smalest = -1;
        MLib::Vec3 n_sphere_center;
        double l_dot = DBL_MIN;
        for(size_t i : verts)
        {
            if(i == e->start || i == e->end || i == e->across) continue;

            double d = n_c * (c_c - point_data[i].position);
            if(std::abs(d) > p) continue;

            MLib::Vec3 c_p = point_data[i].position + (n_c*d);

            double r_p = sqrt(p*p - d*d);
            double h = 0.5 + ((r_c * r_c) - (r_p * r_p))/(2.0 * (c_c - c_p).length2());
            double r_i = sqrt(r_c*r_c - h*h*d*d);
            MLib::Vec3 c_i = c_c + ((c_p - c_c)*h);

            MLib::Vec3 t = (c_p - c_c).cross(n_c).norm();

            MLib::Vec3 p_0 = c_i - t * r_i;
            MLib::Vec3 p_1 = c_i + t * r_i;

            double dot0 = (p_0 - c_c) * (e->sphere_center - c_c);
            double dot1 = (p_1 - c_c) * (e->sphere_center - c_c);

            if(dot0 < l_dot && dot1 < l_dot) continue;
            if(dot0 > dot1)
            {
                n_sphere_center = p_0;
                l_dot = dot0;
            }
            else
            {
                n_sphere_center = p_1;
                l_dot = dot0;
            }
            idx_smalest = i;
        }

        if(idx_smalest == -1) return false;

        for(size_t i : verts)
        {
            if(i == e->start || i == e->end || i == idx_smalest) continue;
            double len = (n_sphere_center - point_data[i].position).length();
            bool sm = len < p;
            if(sm) return false;
        }

        o_sphere_center = n_sphere_center;
        o_s_k = idx_smalest;

        return true;
    }

    static bool get_active_edge(Front& F, Edge*& e) 
    {
        if(F.empty()) return false;

        for(Front::iterator it = F.begin(); it != F.end(); ++it)
        {
            if(it->state == ACTIVE) 
            {
                e = &(*it); // cursed
                return true;
            }
        }
        return false;
    }

    static bool not_used(uint32_t *usedIndex, int s)
    {
        return ((usedIndex[s / 32] >> (s % 32)) & 1) == 0;
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

    static void join(Edge* e_ij, int s_k, Front& F, MLib::Vec3 sphere_center) 
    {
        //add edges e_ik, e_kj
        //remove edge e_ij <- its no longer used as we found a new triangle
        //e_ij is enclosed in the two triangles and no longer of interest
        
        Edge e_ik{e_ij->start, s_k, e_ij->end, ACTIVE, sphere_center};
        Edge e_kj{s_k, e_ij->end, e_ij->start, ACTIVE, sphere_center};
        ASSERT(s_k >= 0, "s_k was not > 0")
        ASSERT(e_ik.start >= 0, "e_ik.start was not > 0")
        ASSERT(e_ik.end >= 0, "e_ik.end was not > 0")
        ASSERT(e_kj.start >= 0, "e_kj.start was not > 0")
        ASSERT(e_kj.end >= 0, "e_kj.start was not > 0")
        F.push_back(e_ik);
        F.push_back(e_kj);
        F.remove(*e_ij);
    }
    
    static void glue(Edge& e, Front& F) 
    {
        Edge reverse{e.end, e.start, e.across, e.state, e.sphere_center};
        F.remove(e);
        F.remove(reverse);
    }
    
    static void mark_as_boundary(Edge* e) { e->state = BOUNDARY; }
    
    static std::vector<int> find2pNeighbourhood(const VertList& verts, uint32_t* usedIndex, int pos, double p)
    {
        std::vector<int> neighbourhood;
        double p2 = 2.0*p;
        MLib::Vec3 spos = verts[pos].position;
        for(size_t i = 0; i < verts.size(); ++i)
        {
            double dist = verts[i].position.dist(spos);
            if(dist <= p2 && dist > 0 && not_used(usedIndex, i))
            {
                neighbourhood.push_back(i);
            }
        }
        return neighbourhood;
    }

    static bool find_seed_triangle(const VertList& verts, const double p, uint32_t* usedIndex, int& s0, int& s1, int& s2, MLib::Vec3& sphere_center) 
    {
        //Spacial query needed

        std::vector<int> p2Neighbourhood;

        for(size_t i = 0; i < verts.size(); ++i)
        {
            if(!not_used(usedIndex, i)) continue;

            p2Neighbourhood = find2pNeighbourhood(verts, usedIndex, i, p);

            if(p2Neighbourhood.size() < 2) continue;

            for(int m = 0; m < p2Neighbourhood.size(); ++m)
            {
                for(int n = 0; n < p2Neighbourhood.size(); ++n)
                {
                    if(n == m) continue;

                    const Geometry::Vertex& v0 = verts[i];
                    const Geometry::Vertex& v1 = verts[p2Neighbourhood[m]];
                    const Geometry::Vertex& v2 = verts[p2Neighbourhood[n]];

                    if(IsSeedTriangle(v0, v1, v2, sphere_center, p))
                    {
                        s0 = i;
                        s1 = p2Neighbourhood[m];
                        s2 = p2Neighbourhood[n];
                        return true;
                    }
                }
            }
        }

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
        
        m.emplace_back(Geometry::Triangle{s0, s1, s2});

        std::cout << "[Log]: Found triangle ( " << s0 << ", " << s1 << "," << s2 << " )\n";
    }


     /**
     * @brief Takes in a ref to a model and fills out the face data
     * 
     * @param model point cloud data set
     * @param p size of the p-ball
     */
    static OBJ::Model PivotBall(const VertList& point_data, double p)
    {

        size_t verts = point_data.size();
        size_t buckets = verts/32;
        if(buckets == 0) buckets = 1;
        uint32_t *usedIndex = (uint32_t*)malloc((buckets)*sizeof(uint32_t));
        for(size_t i = 0; i < buckets; ++i) usedIndex[i] = 0;

        Front F;
        Mesh m;

        bool found_seed = true;
        

        while(true && found_seed)
        {
            found_seed = false;
            MLib::Vec3 sphere_center;
            {
                int s_i = -1, s_j = -1, s_k = -1;
                if(find_seed_triangle(point_data, p, usedIndex, s_i, s_j, s_k, sphere_center))
                {
                    output_triangle(s_i, s_j, s_k, m, usedIndex);
                    Edge e_ij{s_i, s_j, s_k, ACTIVE, sphere_center};
                    Edge e_jk{s_j, s_k, s_i, ACTIVE, sphere_center};
                    Edge e_ki{s_k, s_i, s_j, ACTIVE, sphere_center};
                    insert_edge(e_ij, F);
                    insert_edge(e_jk, F);
                    insert_edge(e_ki, F);
                    found_seed = true;
                }
            }

            Edge* e_ij = nullptr;
            while(get_active_edge(F, e_ij))
            {
                ASSERT(e_ij != nullptr, "e_ij as null")
                int s_k = -1; // index of vertex s_k in point cloud data
                int s_i = e_ij->start;
                int s_j = e_ij->end;
                if(ball_pivote(point_data, e_ij, s_k, sphere_center, p) && (not_used(usedIndex, s_k) || on_front(F, s_k)))
                {
                    output_triangle(s_i, s_k, s_j, m, usedIndex);
                    join(e_ij, s_k, F, sphere_center);
                    Edge e_ki{s_k, s_i, s_j, ACTIVE, sphere_center};
                    Edge e_jk{s_j, s_k, s_i, ACTIVE, sphere_center};
                    if(edge_in_front(e_ki, F)) glue(e_ki, F);
                    if(edge_in_front(e_jk, F)) glue(e_jk, F);
                }
                else
                {
                    //mark_as_boundary(e_ij);
                    e_ij->state = BOUNDARY;
                }
            }

            
        }

        free(usedIndex);

        return {};
    }


}

#endif //PBA_HPP