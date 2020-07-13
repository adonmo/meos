#ifndef MEOS_TYPES_GEOM_SRIDMEMBERS_HPP
#define MEOS_TYPES_GEOM_SRIDMEMBERS_HPP

#include <type_traits>

/**
 * Declares an empty type
 */
#define Empty tuple<>

/**
 * This template is used to define member fields and functions when using
 * template specialization
 *
 * Currently this being used to enable SRID related members on Temporal<T> class
 * when specilizing for the Geometry class. i.e, Temporal<Geometry> class
 */
template <typename, typename, typename E> struct if_ { typedef E type; };

template <typename T, typename E> struct if_<std::true_type, T, E> {
  typedef T type;
};

extern int const SRID_DEFAULT;

/**
 * SRIDMembers is to be used by the classes Temporal and STBox
 */
struct SRIDMembers {
  int m_srid = SRID_DEFAULT;

  SRIDMembers() {}
  SRIDMembers(int srid) : m_srid(srid) {}

  /**
   * @return true if SRID != SRID_DEFAULT
   */
  bool has_srid() const;

  /**
   * Get SRID of the coordinates
   *
   * Only present when base type is Geometry
   */
  int srid() const;
};

#endif
