#pragma once
#include "double_operations.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <array>
#include <list>
#include <set>

using namespace doperations;
using namespace geometry;


namespace octrees{

const size_t SIZE_OF_PART = (1 << 7);
const int child_num = 8;

struct triag_id_t
{
    triangle_t triag;
    size_t     id;
};

using triag_vector = std::vector<triag_id_t>;

struct node_position
{
    double x_, y_, z_;
    double rad_;

    node_position(double x, double y, double z, double rad) :
    x_(x), y_(y), z_(z), rad_(rad){}

    void print() const
    {
        std::cout << "center = (" << x_ << ", " << y_ << ", " << z_ << ")\nradius = " << rad_ << std::endl;
    }
};


enum cube_positions
{
    ZERO    = 0,
    FRST    = 1,
    SEC     = 2,
    THIRD   = 3,
    FOUR    = 4,
    FIFTH   = 5,
    SIXTH   = 6,
    SEVENTH = 7,
    BORDER  = child_num
};


struct max_min_crds_t
{
    double x_min = std::numeric_limits<double>::infinity();
    double x_max = -x_min;

    double y_min = std::numeric_limits<double>::infinity();
    double y_max = -y_min;

    double z_min = std::numeric_limits<double>::infinity();
    double z_max = -z_min;

    void update(double x, double y, double z)
    {
        if (x < x_min) x_min = x;
        if (x > x_max) x_max = x;
        if (y < y_min) y_min = y;
        if (y > y_max) y_max = y;
        if (z < z_min) z_min = z;
        if (z > z_max) z_max = z;
    }

    double get_meanx() const { return (x_max + x_min)/2; }
    double get_meany() const { return (y_max + y_min)/2; }
    double get_meanz() const { return (z_max + z_min)/2; }

    double get_rad() const
    {
        double xlen = x_max - x_min, ylen = y_max - y_min, zlen = z_max - z_min;
        return triple_max(xlen, ylen, zlen)/2; 
    }

    void print() const
    {
        std::cout << "xmin = " << x_min << "\nxmax = " << x_max << "\n";
        std::cout << "ymin = " << y_min << "\nymax = " << y_max << "\n";
        std::cout << "zmin = " << z_min << "\nzmax = " << z_max << "\n";
    }
};


namespace detail {

class node_t
{
    bool isleaf_ = true;
    node_t* parent_ = nullptr;

    node_position pos_;

    std::array<plane_t, 3> planes_ = {plane_t{{1, 0, 0}, {pos_.x_, 0, 0}},
                                      plane_t{{0, 1, 0}, {0, pos_.y_, 0}},
                                      plane_t{{0, 0, 1}, {0, 0, pos_.z_}}};

    std::array<node_t*, child_num> children_;
    std::array<triag_vector, child_num+1> triangle_vectors_;


public:
    size_t       triag_num_ = 0;
    triag_vector triags_;

/*==========================================================================*/

    node_t(node_t* parent, node_position pos, triag_vector triags, std::list<node_t> &nodes) : parent_(parent), pos_(pos), triags_(triags)
    {
        triag_num_ = triags.size();

        if (triag_num_ < SIZE_OF_PART){ /* print(); */ return; }

        isleaf_ = false;

        isleaf_ = false;

        double next_rad = pos_.rad_ / 2;

        std::vector<node_position> children_pos;

        for (int i = 0; i < child_num; ++i)
            children_pos.push_back({pos_.x_ + ((i & (1 << 0)) ? -next_rad : next_rad),
                                    pos_.y_ + ((i & (1 << 1)) ? -next_rad : next_rad),
                                    pos_.z_ + ((i & (1 << 2)) ? -next_rad : next_rad),
                                    next_rad});

        for (auto it = triags_.begin(), ite = triags_.end(); it != ite; ++it) triag_emplace(*it);

        //print();

        for (int i = 0; i < child_num; ++i)
        {
            nodes.push_back(node_t{this, children_pos[i], triangle_vectors_[i], nodes});
            children_[i] = &(*std::prev(nodes.end()));
        }
    }

/*==========================================================================*/

    void print() const
    {
        std::cout << "isleaf = " << isleaf_ << std::endl;
        std::cout << "parent = " << parent_ << " this = " << this << std::endl;
        std::cout << "number of elements = " << triag_num_ << std::endl;
        std::cout << "numer of triags in border = " << triangle_vectors_[child_num].size() << std::endl;

        std::cout << "\n\n";
    }


    void triag_emplace(triag_id_t triag)
    {
        cube_positions triag_pos = check_triangle(triag);

        triangle_vectors_[triag_pos].push_back(triag);
    }

/*==========================================================================*/

    cube_positions check_triangle(triag_id_t triag) const
    {
        std::array<std::array<double, 3>, 3> res;

        for (int i = 0; i < 3; i++)
        {
            res[i][0] = planes_[i].calc_point(triag.triag.getA());
            res[i][1] = planes_[i].calc_point(triag.triag.getB());
            res[i][2] = planes_[i].calc_point(triag.triag.getC());
        }

        std::array<bool, 3> pos_neg_positions;
        for (int i = 0; i < 3; ++i)
        {
            if (all_positive(res[i][0], res[i][1], res[i][2]))
                pos_neg_positions[i] = true;

            else if (all_negative(res[i][0], res[i][1], res[i][2]))
                pos_neg_positions[i] = false;

            else return BORDER;
        }

        int ret = 0;
        for (int i = 0; i < 3; ++i)
            if (!pos_neg_positions[i]) ret |= (1 << i);

        return static_cast<cube_positions>(ret);
    }

/*==========================================================================*/

    void get_collisions(std::vector<bool> &answer)
    {
        if (isleaf_)
        {
            for (auto it = triags_.begin(), ite = triags_.end(); it != ite; ++it) {
                for (auto jt = std::next(it), jte = triags_.end(); jt != jte; ++jt) {

                    if (it->triag.intersects(jt->triag)) {
                        answer[it->id] = true;
                        answer[jt->id] = true;
                    }
                }
            } return;
        }

        for (int i = 0; i < child_num; i++)
            children_[i]->get_collisions(answer);

        for (auto it = triangle_vectors_[child_num].begin(), ite = triangle_vectors_[child_num].end(); it != ite; ++it) {
            for (auto jt = triags_.begin(), jte = triags_.end(); jt != jte; ++jt) {
                if (it->id == jt->id) continue;

                if (it->triag.intersects(jt->triag)) {
                    answer[it->id] = true;
                    answer[jt->id] = true;
                }
            }
        }
    }
};

}


class octree_t
{
    detail::node_t* root_ = nullptr;
    std::list<detail::node_t> nodes_;

    triag_vector all_triags_;

public:
    std::set<triag_id_t> border_triags_;


    octree_t(triag_vector all_triags) : all_triags_(all_triags)
    {
        max_min_crds_t min_max{};

        for (auto it = all_triags_.begin(); it != all_triags_.end(); it++)
        {
            point_t A{it->triag.getA()};
            point_t B{it->triag.getB()};
            point_t C{it->triag.getC()};

            min_max.update(A.get_x(), A.get_y(), A.get_z());
            min_max.update(B.get_x(), B.get_y(), B.get_z());
            min_max.update(C.get_x(), C.get_y(), C.get_z());
        }

        node_position pos{min_max.get_meanx(), min_max.get_meany(), min_max.get_meanz(), min_max.get_rad()};

        nodes_.push_back(detail::node_t{nullptr, pos, all_triags_, nodes_});
        root_ = &(*std::prev(nodes_.end()));
    }

    void print() const { root_->print(); }
    void get_collisions(std::vector<bool> &answer) { root_->get_collisions(answer); }
};

}
