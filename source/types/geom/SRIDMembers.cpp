#include <meos/types/geom/SRIDMembers.hpp>

extern int const SRID_DEFAULT = 0;

int SRIDMembers::srid() const { return this->m_srid; }

bool SRIDMembers::has_srid() const { return this->m_srid != SRID_DEFAULT; }
