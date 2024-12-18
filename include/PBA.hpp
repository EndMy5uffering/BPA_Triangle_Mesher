#ifndef PBA_HPP
#define PBA_HPP

#include <Collider.hpp>
#include <Geometry.hpp>
#include <MLib.hpp>
#include <vector>
#include <iostream>
#include <cmath>

#include <list>
#include <array>
#include <bitset>

//#define ASSERT(x) { if (!(x)) __debugbreak(); }
#define ASSERT(x, m) { if (!(x)) {std::cout << "[DEBUG_ASSERT]: " << m << "\n"; __debugbreak();} }
#define LOG(m) {std::cout << "[DEBUG_LOG (" << __LINE__ << " | " << __FILE__ <<")]: " << m << "\n";}

# define M_PI   3.14159265358979323846  /* pi */

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
                (end == other.end);
        }
    };

    typedef std::list<Edge> Front;
    struct Mesh{
        std::list<Geometry::Triangle> m_triangles;
        std::vector<bool> vertex_used;

        Mesh(){}

        Mesh(Mesh& m) 
        : 
        m_triangles{m.m_triangles}, 
        vertex_used{m.vertex_used}
        {

        }

        Mesh(Mesh&& m) noexcept
        : m_triangles(std::move(m.m_triangles)),
        vertex_used(std::move(m.vertex_used)) 
        {
        }

        Mesh& operator=(Mesh& m)
        {
            m_triangles = m.m_triangles;
            vertex_used = m.vertex_used;
            return *this;
        }

        Mesh& operator=(Mesh&& m)
        {
            std::swap(m_triangles, m.m_triangles);
            std::swap(vertex_used, m.vertex_used);
            return *this;
        }
    };

    typedef std::vector<Geometry::Vertex> VertList;

    static std::vector<int> find2pNeighbourhood(const VertList& verts, MLib::Vec3 pos, double p)
    {
        std::vector<int> neighbourhood;
        double p2 = 2.0*p;
        for(size_t i = 0; i < verts.size(); ++i)
        {
            double dist = verts[i].position.dist(pos);
            if(dist <= p2 && dist > 0)
            {
                neighbourhood.push_back(i);
            }
        }
        return neighbourhood;
    }

    static bool IsSeedTriangle(const Geometry::Vertex& a, const Geometry::Vertex& b, const Geometry::Vertex& c, MLib::Vec3& sphere_center, double p, const PBA::VertList& vlist)
    {
        std::cout << "##### Testing seed triangle: \n\t< " 
        << a.position.x << ", " << a.position.y << ", " << a.position.z << " >"
        << "\n\t< " << b.position.x << ", " << b.position.y << ", " << b.position.z << " >"
        << "\n\t< " << c.position.x << ", " << c.position.y << ", " << c.position.z << " >\n";
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

        std::vector<int> neighbours = find2pNeighbourhood(vlist, sphere_center, p);
        std::cout << ">\tHas neighbours ( " << neighbours.size() << " )!\n";

        for(size_t i = 0; i < neighbours.size(); ++i)
        {
            const Geometry::Vertex& tocheck = vlist[neighbours[i]];
            if(tocheck == a || tocheck == b || tocheck == c) continue;
            double dist = tocheck.position.dist(sphere_center);
            if(dist < p) 
            {
                std::cout << ">\tTo close at sphere center for seed triangle: " << dist << " for sphere with radious " << p <<"\n";
                return false;
            }
        }
        std::cout << "########################\n";
        std::cout << "###### Is seed triangle!\n";
        std::cout << "########################\n";
        return true;
    }

    /**
     * @brief Returns the Angle between two vectors in the range [0, 2PI]
     * 
     * @param v0 First vector
     * @param v1 Second vector
     * @param n Normal on the plane the two points are on
     * @return angle as double between 0 and 2PI
     */
    static double get_rotation(MLib::Vec3 v0, MLib::Vec3 v1, MLib::Vec3 n)
    {
        MLib::Vec3 v0n = v0.norm();
        MLib::Vec3 v1n = v1.norm();
        MLib::Vec3 nn = n.norm();

        double dotuv = v0n.dot(v1n);
        double theta = acos(dotuv);

        MLib::Vec3 cross = v0n.cross(v1n);
        double sign = nn.dot(cross);

        if(sign < 0)
        {
            theta = 2.0 * M_PI - theta;
        }
        return theta;
    }

    static bool ball_pivote(const VertList& point_data, const Edge* e, int& o_s_k, MLib::Vec3& o_sphere_center, double p) 
    {
        //https://gamedev.stackexchange.com/questions/75756/sphere-sphere-intersection-and-circle-sphere-intersection

        MLib::Vec3 c_c = (point_data[e->start].position + point_data[e->end].position)*0.5;
        double r_c = (e->sphere_center - c_c).length();

        MLib::Vec3 vsphcn = (e->sphere_center - c_c).norm();

        MLib::Vec3 n_c = (point_data[e->end].position - point_data[e->start].position).norm();

        MLib::Vec3 norm_at_rot = (point_data[e->start].normal + point_data[e->end].normal) * 0.5;
        MLib::Vec3 corss_direction_normed = (point_data[e->across].position - (point_data[e->end].position + n_c*((point_data[e->across].position - point_data[e->end].position).dot(n_c)))).norm().cross(norm_at_rot).norm();

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
        double smalestAngle = 99999.9999;
        for(size_t i : verts)
        {
            if(i == e->start || i == e->end || i == e->across) continue;

            MLib::Vec3 dir_to_point = point_data[i].position - c_c;
            double d = n_c * (dir_to_point);

            if(std::abs(d) > p) continue;

            MLib::Vec3 c_p = point_data[i].position + (n_c*d);

            double r_p = sqrt(p*p - d*d);
            double h = 0.5 + ((r_c * r_c) - (r_p * r_p))/(2.0 * (c_c - c_p).length2());
            double r_i = sqrt(r_c*r_c - h*h*d*d);
            MLib::Vec3 c_i = c_c + ((c_p - c_c)*h);

            MLib::Vec3 t = (c_p - c_c).cross(n_c).norm();

            MLib::Vec3 p_0 = c_i - t * r_i;
            MLib::Vec3 p_1 = c_i + t * r_i;

            double rot0 = get_rotation(p_0-c_c, vsphcn, n_c);
            double rot1 = get_rotation(p_1-c_c, vsphcn, n_c);
            if(!(rot0 < smalestAngle || rot1 < smalestAngle)) continue;
            if(rot0 < rot1)
            {
                smalestAngle = rot0;
                idx_smalest = i;
                n_sphere_center = p_0;
            }
            else
            {
                smalestAngle = rot1;
                idx_smalest = i;
                n_sphere_center = p_1;
            }

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
    
    static std::vector<int> find2pNeighbourhoodUnused(const VertList& verts, Mesh& mesh, int pos, double p)
    {
        std::vector<int> neighbourhood;
        double p2 = 2.0*p;
        MLib::Vec3 spos = verts[pos].position;
        for(size_t i = 0; i < verts.size(); ++i)
        {
            double dist = verts[i].position.dist(spos);
            if(dist <= p2 && dist > 0 && !mesh.vertex_used[i])
            {
                neighbourhood.push_back(i);
            }
        }
        return neighbourhood;
    }



    static bool find_seed_triangle(const VertList& verts, const double p, Mesh& mesh, int& s0, int& s1, int& s2, MLib::Vec3& sphere_center) 
    {
        //Spacial query needed

        std::vector<int> p2Neighbourhood;

        for(size_t i = 0; i < verts.size(); ++i)
        {
            if(mesh.vertex_used[i]) continue;

            p2Neighbourhood = find2pNeighbourhoodUnused(verts, mesh, i, p);

            if(p2Neighbourhood.size() < 2) continue;

            for(int m = 0; m < p2Neighbourhood.size(); ++m)
            {
                for(int n = 0; n < p2Neighbourhood.size(); ++n)
                {
                    if(n == m) continue;

                    const Geometry::Vertex& v0 = verts[i];
                    const Geometry::Vertex& v1 = verts[p2Neighbourhood[m]];
                    const Geometry::Vertex& v2 = verts[p2Neighbourhood[n]];

                    if(IsSeedTriangle(v0, v1, v2, sphere_center, p, verts))
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

    static void output_triangle(int s0, int s1, int s2, Mesh& m) 
    {
        m.vertex_used[s0] = true;
        m.vertex_used[s1] = true;
        m.vertex_used[s2] = true;

        //Write that to the model

        m.m_triangles.emplace_back(Geometry::Triangle{s0, s1, s2, m.m_triangles.size() < 10});

        std::cout << "[Log]: Found triangle ( " << s0 << ", " << s1 << ", " << s2 << " )\n";
    }


     /**
     * @brief Takes in a ref to a model and fills out the face data
     * 
     * @param model point cloud data set
     * @param p size of the p-ball
     */
    static Mesh PivotBall(const VertList& point_data, double p)
    {

        Front F;
        Mesh m;
        m.vertex_used.resize(point_data.size());

        bool found_seed = true;
        

        while(found_seed)
        {
            found_seed = false;
            MLib::Vec3 sphere_center;
            {
                int s_i = -1, s_j = -1, s_k = -1;
                if(find_seed_triangle(point_data, p, m, s_i, s_j, s_k, sphere_center))
                {
                    output_triangle(s_i, s_j, s_k, m);
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
                if(ball_pivote(point_data, e_ij, s_k, sphere_center, p) && (!m.vertex_used[s_k] || on_front(F, s_k)))
                {
                    output_triangle(s_i, s_k, s_j, m);
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

        return m;
    }


}

#endif //PBA_HPP