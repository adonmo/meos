#ifndef MEOS_TYPES_GEOM_SRIDMEMBERS_HPP
#define MEOS_TYPES_GEOM_SRIDMEMBERS_HPP

extern int const SRID_DEFAULT;

/**
 * SRIDMembers is to be used by the classes Temporal and STBox
 *
 * In case of Temporal<BaseType> class, we inherit from this conditionally,
 * specifically when BaseType=Geometry. i.e, Temporal<Geometry> class
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
