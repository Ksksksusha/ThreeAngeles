#include "double_operations.hpp"
#include "custom_assert.hpp"
#include "triangle.hpp"
#include "point.hpp"
#include <iostream>
#include <vector>

using namespace geometry;
using namespace doperations;


triangle_t::triangle_t(const point_t &A, const point_t &B, const point_t &C) : A_(A), B_(B), C_(C), type_(get_triag_type())
{
    type_ = get_triag_type();
    center_x3_ = vector_t{A_} + vector_t{B_} + vector_t{C_};

    double lenab = (vector_t{B_} - vector_t{A_}).get_squared_len();
    double lenbc = (vector_t{C_} - vector_t{B_}).get_squared_len();
    double lenca = (vector_t{A_} - vector_t{C_}).get_squared_len();

    double max_sq = triple_max(lenab, lenbc, lenca);

    bounding_rad_sq_ = max_sq;
}


triag_type triangle_t::get_triag_type() const
{
    ASSERT(is_valid());

    if ((A_ == B_) && (B_ == C_)) return POINT;
    if ((vector_t{B_} - vector_t{A_}).vec_product(vector_t{C_} - vector_t{A_}) == NULL_VEC ||
        (vector_t{B_} - vector_t{A_}).vec_product(vector_t{C_} - vector_t{B_}) == NULL_VEC ||
        (vector_t{C_} - vector_t{A_}).vec_product(vector_t{C_} - vector_t{B_}) == NULL_VEC) return SEGMENT;
    return TRIAG;
}


void triangle_t::print() const
{
    std::cout << "\ntriangle: " << "type " << type_ << std::endl;
    std::cout << "A = ";
    A_.print();
    std::cout << "B = ";
    B_.print();
    std::cout << "C = ";
    C_.print();
}


bool triangle_t::intersects(const triangle_t &triag2) const
{
    ASSERT(is_valid());
    ASSERT(triag2.is_valid());

    double distanced_squared_x9 = (center_x3_ - triag2.center_x3_).get_squared_len();
    if (distanced_squared_x9 > brad_coeff * (bounding_rad_sq_ + triag2.bounding_rad_sq_)) return false;


    switch(type_)
    {
        case TRIAG: {
            switch(triag2.type_) {
                case TRIAG:   return intersects_triag_triag(triag2);
                case SEGMENT: return intersects_triag_segment(triag2);
                case POINT:   return intersects_triag_point(triag2);
            }
        }
        break;

        case SEGMENT: {
            switch(triag2.type_) {
                case TRIAG:   return triag2.intersects_triag_segment(*this);
                case SEGMENT: return intersects_segment_segment(triag2);
                case POINT:   return intersects_segment_point(triag2);
            }
        }
        break;

        case POINT: {
            switch(triag2.type_) {
                case TRIAG:   return triag2.intersects_triag_point(*this);
                case SEGMENT: return triag2.intersects_segment_point(*this);
                case POINT:   return intersects_point_point(triag2);
            }
        }
        break;
    }

    return false;
}


bool triangle_t::intersects_point_point(const triangle_t &triag2) const
{
    point_t pnt1 = {center_x3_.get_x() / 3, center_x3_.get_y() / 3, center_x3_.get_z() / 3};
    point_t pnt2 = {triag2.center_x3_.get_x() / 3, triag2.center_x3_.get_y() / 3, triag2.center_x3_.get_z() / 3};

    return (pnt1 == pnt2);
}


bool triangle_t::intersects_segment_point(const triangle_t &triag2) const
{
    segment_t seg1 = get_segment();
    point_t pnt = {triag2.center_x3_.get_x() / 3, triag2.center_x3_.get_y() / 3, triag2.center_x3_.get_z() / 3};

    if ( !seg1.get_seg_line().check_point_belong(pnt) ) return false;

    if ( !seg1.contains_inter_pnt(pnt) ) return false;

    return true;
}


bool triangle_t::intersects_segment_segment(const triangle_t &triag2) const
{
    segment_t seg1 = get_segment();
    segment_t seg2 = triag2.get_segment();

    point_t pnt = seg1.get_seg_line().get_line_intersection(seg2.get_seg_line());

    if (pnt.special_check())
        return seg1.intersects_seg(seg2);

    if (!pnt.is_valid()) return false;

    return seg1.contains_inter_pnt(pnt) && seg2.contains_inter_pnt(pnt);
}


bool triangle_t::intersects_triag_point(const triangle_t &triag2) const
{
    point_t pnt = {triag2.center_x3_.get_x() / 3, triag2.center_x3_.get_y() / 3, triag2.center_x3_.get_z() / 3};

    if (!is_equal(pln_.calc_point(pnt), 0)) return false;
    if (!is_in_triag(pnt)) return false;

    return true;
}


bool triangle_t::intersects_triag_segment(const triangle_t &triag2) const
{
    segment_t seg = triag2.get_segment();

    point_t pnt = pln_.get_line_intersection(seg.get_seg_line());

    if ( pnt.special_check() )
    {
        bool cond = is_in_triag(seg.get_fst()) ||
                    is_in_triag(seg.get_snd());
        if (cond) return true;

        segment_t AB{A_, B_},
                  BC{B_, C_},
                  CA{C_, A_};

        return seg.intersects_seg(AB) ||
               seg.intersects_seg(BC) ||
               seg.intersects_seg(CA);
    }

    if ( !pnt.is_valid() ) return false;

    if ( !is_equal(pln_.calc_point(pnt), 0) ) return false;

    if ( seg.contains_inter_pnt(pnt) && is_in_triag(pnt) ) return true;

    return false;
}


segment_t triangle_t::get_segment() const
{
    segment_t AB{A_, B_},
              BC{B_, C_},
              CA{C_, A_};

    double ab = AB.get_dir_vec().get_squared_len();
    double bc = BC.get_dir_vec().get_squared_len();
    double ca = CA.get_dir_vec().get_squared_len();
    double max = triple_max(ab, bc, ca);

    if (is_equal(ab, max)) return AB;
    if (is_equal(bc, max)) return BC;
    if (is_equal(ca, max)) return CA;

    return {NAN_PNT, NAN_PNT};
}


bool triangle_t::intersects_triag_triag(const triangle_t &triag2) const
{
    mutual_pos plane_pos_type = pln_.get_mutual_pos_type(triag2.pln_, A_);

    switch(plane_pos_type)
    {
        case EQUAL:     return check_triags_in_same_plane(triag2);
        case INTERSECT: return check_triags_in_intersect_planes(triag2);
        case PARALLEL:  return false;
        default:        return false;
    }
    return false;
}


bool triangle_t::check_triags_in_same_plane(const triangle_t &triag2) const
{
    bool cond1 = triag2.is_in_triag(A_) ||
                 triag2.is_in_triag(B_) ||
                 triag2.is_in_triag(C_);
    bool cond2 = is_in_triag(triag2.A_) ||
                 is_in_triag(triag2.B_) ||
                 is_in_triag(triag2.C_);
    if (cond1 || cond2) return true;

    std::vector<segment_t> segments{{A_, B_}, {B_, C_}, {C_, A_}, {triag2.A_, triag2.B_}, {triag2.B_, triag2.C_}, {triag2.C_, triag2.A_}};

    for (int i = 0; i < 3; ++i)
        for (int j = 3; j < 6; ++j)
                if (segments[i].intersects_seg(segments[j])) return true;

    return false;
}


bool triangle_t::check_triags_in_intersect_planes(const triangle_t &triag2) const
{
    if (!check_triag_intersect_plane(triag2)) return false;

    line_t inter_line{pln_.get_intersection(triag2.pln_)};
    if ( !inter_line.is_valid() ) return false;

    segment_t seg1{get_triag_intersection(inter_line)},
              seg2{triag2.get_triag_intersection(inter_line)};

    if (!seg1.is_valid() || !seg2.is_valid()) return false;

    return seg1.contains_inter_seg(seg2);
}


bool triangle_t::check_triag_intersect_plane(const triangle_t &triag2) const
{
    double res1 = triag2.pln_.calc_point(A_);
    double res2 = triag2.pln_.calc_point(B_);
    double res3 = triag2.pln_.calc_point(C_);
    if (all_positive(res1, res2, res3) ||
        all_negative(res1, res2, res3)) return false;

    res1 = pln_.calc_point(triag2.A_);
    res2 = pln_.calc_point(triag2.B_);
    res3 = pln_.calc_point(triag2.C_);
    if (all_positive(res1, res2, res3) ||
        all_negative(res1, res2, res3)) return false;

    return true;
}


bool triangle_t::is_in_triag(const point_t &pnt) const
{
    vector_t v1{(vector_t{B_} - vector_t{A_}).vec_product((vector_t{pnt} - vector_t{A_}))};
    vector_t v2{(vector_t{C_} - vector_t{B_}).vec_product((vector_t{pnt} - vector_t{B_}))};
    vector_t v3{(vector_t{A_} - vector_t{C_}).vec_product((vector_t{pnt} - vector_t{C_}))};

    vector_t resv1 = vector_t{pnt} + v1;
    vector_t resv2 = vector_t{pnt} + v2;
    vector_t resv3 = vector_t{pnt} + v3;

    double res1 = pln_.calc_point({resv1.get_x(), resv1.get_y(), resv1.get_z()});
    double res2 = pln_.calc_point({resv2.get_x(), resv2.get_y(), resv2.get_z()});
    double res3 = pln_.calc_point({resv3.get_x(), resv3.get_y(), resv3.get_z()});

    if (all_positive_triag_special(res1, res2, res3) || all_negative_triag_special(res1, res2, res3)) return true;

    return false;
}


segment_t triangle_t::get_triag_intersection(const line_t &line) const
{
    ASSERT(is_valid());
    ASSERT(line.is_valid());

    int valid_cnt = 0;

    segment_t AB{A_, B_},
              BC{B_, C_},
              CA{C_, A_};


    point_t p1{AB.get_line_intersection(line)};

    if (p1.is_valid()) valid_cnt++;
    else if (p1.special_check())
        return AB;


    point_t p2{BC.get_line_intersection(line)};

    if (p2.is_valid()) valid_cnt++;
    else if (p2.special_check())
        return BC;


    point_t p3{CA.get_line_intersection(line)};

    if (p3.is_valid()) valid_cnt++;
    else if (p3.special_check())
        return CA;


    if (valid_cnt < 2) return {NAN_PNT, NAN_PNT};

    if (valid_cnt == 2)
    {
        if (!p1.is_valid()) return {p2, p3};
        if (!p2.is_valid()) return {p1, p3};
        return {p1, p2};
    }

    if (p1 == p2) return {p1, p3};
    return {p1, p2};
}
