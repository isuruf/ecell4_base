#ifndef ECELL4_SGFRD_PAIR_DOMAIN
#define ECELL4_SGFRD_PAIR_DOMAIN
#include <ecell4/sgfrd/Shell.hpp>
#include <ecell4/sgfrd/DomainID.hpp>
#include <ecell4/core/Particle.hpp>
#include <ecell4/core/Identifier.hpp>
#include <ecell4/core/Circle.hpp>
#include <ecell4/core/Cone.hpp>
#include <boost/variant.hpp>

namespace ecell4
{
namespace sgfrd
{

template<typename T_polygon_traits>
class Pair
{
  public:

    typedef T_polygon_traits polygon_traits;
    typedef typename polygon_traits::face_id_type face_id_type;

    typedef DomainID   identifier_type;
    typedef Particle   particle_type;
    typedef ParticleID particle_id_type;
    typedef std::pair<ParticleID, Particle>   particle_id_pair;
    typedef boost::array<particle_id_pair, 2> particle_array_type;
    typedef Shell<ecell4::Circle, face_id_type> circular_shell;
    typedef boost::variant<circular_shell> storage_type;

  public:
    Pair(): dt_(0.), last_time_(0.){}
    ~Pair(){}

    Pair(identifier_type const& id, circular_shell const& sh)
        : id_(id), shell_(sh)
    {}
    Pair(identifier_type const& id,
         particle_id_pair const& p0, particle_id_pair const& p1)
        : id_(id)
    {
        if(p0.second.D() < p1.second.D())
        {
            new(&particles_[0]) particle_id_pair(p0);
            new(&particles_[1]) particle_id_pair(p1);
        }
        else
        {
            new(&particles_[0]) particle_id_pair(p1);
            new(&particles_[1]) particle_id_pair(p0);
        }
    }

    identifier_type&       id()       {return id_;}
    identifier_type const& id() const {return id_;}
    storage_type&       shell()       {return shell_;}
    storage_type const& shell() const {return shell_;}
    Real& dt()       {return dt_;}
    Real  dt() const {return dt_;}
    Real& last_time()       {return last_time_;}
    Real  last_time() const {return last_time_;}

    particle_array_type&       particles()       {return particles_;}
    particle_array_type const& particles() const {return particles_;}

    std::size_t num_shells()   const {return 1;}
    std::size_t multiplicity() const {return 2;}

  private:

    Real dt_;
    Real last_time_;
    identifier_type     id_;
    particle_array_type particles_;
    storage_type        shell_;
};


} // sgfrd
} // ecell4
#endif /* ECELL4_SGFRD_PAIR_DOMAIN */
