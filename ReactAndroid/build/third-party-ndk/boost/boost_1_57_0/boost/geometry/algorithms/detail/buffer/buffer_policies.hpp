// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2012-2014 Barend Gehrels, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFER_POLICIES_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFER_POLICIES_HPP


#include <cstddef>

#include <boost/range.hpp>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/detail/overlay/backtrack_check_si.hpp>
#include <boost/geometry/algorithms/detail/overlay/turn_info.hpp>

#include <boost/geometry/strategies/buffer.hpp>


namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace buffer
{


enum intersection_location_type
{
    location_ok, inside_buffer, inside_original
};

class backtrack_for_buffer
{
public :
    typedef detail::overlay::backtrack_state state_type;

    template <typename Operation, typename Rings, typename Turns, typename Geometry, typename RobustPolicy>
    static inline void apply(std::size_t size_at_start,
                Rings& rings, typename boost::range_value<Rings>::type& ring,
                Turns& turns, Operation& operation,
                std::string const& /*reason*/,
                Geometry const& ,
                Geometry const& ,
                RobustPolicy const& ,
                state_type& state
                )
    {
#if defined(BOOST_GEOMETRY_COUNT_BACKTRACK_WARNINGS)
extern int g_backtrack_warning_count;
g_backtrack_warning_count++;
#endif
//std::cout << "!";
//std::cout << "WARNING " << reason << std::endl;

        state.m_good = false;

        // Make bad output clean
        rings.resize(size_at_start);
        ring.clear();

        // Reject this as a starting point
        operation.visited.set_rejected();

        // And clear all visit info
        clear_visit_info(turns);
    }
};

// Should follow traversal-turn-concept (enrichment, visit structure)
// and adds index in piece vector to find it back
template <typename Point, typename SegmentRatio>
struct buffer_turn_operation
    : public detail::overlay::traversal_turn_operation<Point, SegmentRatio>
{
    int piece_index;
    int index_in_robust_ring;

    inline buffer_turn_operation()
        : piece_index(-1)
        , index_in_robust_ring(-1)
    {}
};

// Version for buffer including type of location, is_opposite, and helper variables
template <typename Point, typename RobustPoint, typename SegmentRatio>
struct buffer_turn_info
    : public detail::overlay::turn_info
        <
            Point,
            SegmentRatio,
            buffer_turn_operation<Point, SegmentRatio>
        >
{
    typedef Point point_type;
    typedef RobustPoint robust_point_type;

    int turn_index; // TODO: this might go if partition can operate on non-const input

    RobustPoint robust_point;
#if defined(BOOST_GEOMETRY_BUFFER_ENLARGED_CLUSTERS)
    // Will (most probably) be removed later
    RobustPoint mapped_robust_point; // alas... we still need to adapt our points, offsetting them 1 integer to be co-located with neighbours
#endif


    inline RobustPoint const& get_robust_point() const
    {
#if defined(BOOST_GEOMETRY_BUFFER_ENLARGED_CLUSTERS)
        return mapped_robust_point;
#endif
        return robust_point;
    }


    intersection_location_type location;

    int count_within;
    int count_on_offsetted;
    int count_on_helper;
    int count_within_near_offsetted;

    bool remove_on_multi;

    // Obsolete:
    int count_on_occupied;
    int count_on_multi;

    inline buffer_turn_info()
        : turn_index(-1)
        , location(location_ok)
        , count_within(0)
        , count_on_offsetted(0)
        , count_on_helper(0)
        , count_within_near_offsetted(0)
        , remove_on_multi(false)
        , count_on_occupied(0)
        , count_on_multi(0)
    {}
};

struct buffer_operation_less
{
    template <typename Turn>
    inline bool operator()(Turn const& left, Turn const& right) const
    {
        segment_identifier const& sl = left.seg_id;
        segment_identifier const& sr = right.seg_id;

        // Sort them descending
        return sl == sr
            ? left.fraction < right.fraction
            : sl < sr;
    }
};

}} // namespace detail::buffer
#endif // DOXYGEN_NO_DETAIL


}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFER_POLICIES_HPP
