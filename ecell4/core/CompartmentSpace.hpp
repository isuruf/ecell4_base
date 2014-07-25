#ifndef __ECELL4_COMPARTMENT_SPACE_HPP
#define __ECELL4_COMPARTMENT_SPACE_HPP

#include "get_mapper_mf.hpp"
#include "types.hpp"
#include "exceptions.hpp"
#include "Species.hpp"
#include "Space.hpp"
#include "CompartmentSpaceHDF5Writer.hpp"


namespace ecell4
{

class CompartmentSpace
    : public Space
{
public:

    // CompartmentSpaceTraits

    virtual const Position3& edge_lengths() const
    {
        throw NotImplemented("edge_lengths() not implemented");
    }

    virtual void set_edge_lengths(const Position3& edge_lengths)
    {
        throw NotImplemented(
            "set_edge_lengths(const Position3&) not implemented");
    }

    /**
     * get volume.
     * this function is a part of the trait of CompartmentSpace.
     * @return a volume (m^3) Real
     */
    virtual const Real volume() const
    {
        throw NotImplemented("volume() not implemented");
    }

    /**
     * get the number of molecules
     * this function is a part of the trait of CompartmentSpace.
     * @param sp a species
     * @return a number of molecules Integer
     */
    virtual Integer num_molecules(const Species& sp) const
    {
        throw NotImplemented("num_molecules(const Species&) not implemented");
    }

    /**
     * get all species whitin the space.
     * this function is a part of the trait of CompartmentSpace.
     * @return a list of species
     */
    virtual std::vector<Species> list_species() const
    {
        throw NotImplemented("list_species() not implemented");
    }

    // CompartSpace member functions

    /**
     * set volume.
     * this function is a member of CompartmentSpace.
     * @param volume a nonzero positive Real value
     */
    virtual void set_volume(const Real& volume) = 0;

    /**
     * increase the number of molecules.
     * this function is a member of CompartmentSpace.
     * @param sp a species
     * @param num a number of molecules
     */
    virtual void add_molecules(const Species& sp, const Integer& num) = 0;

    /**
     * decrease the number of molecules.
     * this function is a member of CompartmentSpace.
     * @param sp a species
     * @param num a number of molecules
     */
    virtual void remove_molecules(const Species& sp, const Integer& num) = 0;

    // Optional members

    virtual void save(H5::Group* root) const = 0;
    virtual void load(const H5::Group& root) = 0;
};

class CompartmentSpaceVectorImpl
    : public CompartmentSpace
{
protected:

    typedef std::vector<Integer> num_molecules_container_type;
    typedef std::vector<Species> species_container_type;
    typedef utils::get_mapper_mf<
        Species, num_molecules_container_type::size_type>::type species_map_type;

public:

    CompartmentSpaceVectorImpl(const Position3& edge_lengths)
    {
        set_edge_lengths(edge_lengths);
    }

    const Position3& edge_lengths() const
    {
        return edge_lengths_;
    }

    void set_edge_lengths(const Position3& edge_lengths)
    {
        for (Position3::size_type dim(0); dim < 3; ++dim)
        {
            if (edge_lengths[dim] <= 0)
            {
                throw std::invalid_argument("the edge length must be positive.");
            }
        }

        edge_lengths_ = edge_lengths;
        volume_ = edge_lengths[0] * edge_lengths[1] * edge_lengths[2];
    }

    // CompartmentSpaceTraits

    const Real volume() const;
    Integer num_molecules(const Species& sp) const;

    // CompartmentSpace member functions

    void set_volume(const Real& volume);
    void add_molecules(const Species& sp, const Integer& num);
    void remove_molecules(const Species& sp, const Integer& num);

    // Optional members

    std::vector<Species> list_species() const;

    void save(H5::Group* root) const
    {
        save_compartment_space<
            CompartmentSpaceVectorImpl, H5DataTypeTraits_uint32_t>(*this, root);
    }

    void load(const H5::Group& root)
    {
        clear();
        load_compartment_space<
            CompartmentSpaceVectorImpl, H5DataTypeTraits_uint32_t>(root, this);
    }

protected:

    void reserve_species(const Species& sp);
    void release_species(const Species& sp);
    void clear();

protected:

    Position3 edge_lengths_;
    Real volume_;

    num_molecules_container_type num_molecules_;
    species_container_type species_;
    species_map_type index_map_;
};

} // ecell4

#endif /* __ECELL4_COMPARTMENT_SPACE_HPP */