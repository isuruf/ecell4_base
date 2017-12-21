#ifndef ECELL4_LATTICE_SPACE_HPP
#define ECELL4_LATTICE_SPACE_HPP

#include <vector>
#include <set>
#include <map>
#include <stdexcept>

#include "Shape.hpp"
#include "Space.hpp"
#include "Integer3.hpp"
#include "get_mapper_mf.hpp"

#ifdef WITH_HDF5
#include "LatticeSpaceHDF5Writer.hpp"
#endif

#include "VoxelPool.hpp"
#include "Voxel.hpp"

namespace ecell4
{

#ifdef WIN32_MSC
double rint(const double x);
double round(const double x);
#endif

class LatticeSpace
    : public Space
{
public:

    typedef Voxel::coordinate_type coordinate_type;
    typedef VoxelPool::coordinate_id_pair_type coordinate_id_pair_type;

public:

    // SpaceTraits

    /**
     * static members
     */

    static inline Real calculate_voxel_volume(const Real r)
    {
        return 4.0 * sqrt(2.0) * r * r * r;
    }

    static inline Real3 calculate_hcp_lengths(const Real voxel_radius)
    {
        return Real3(
            voxel_radius / sqrt(3.0), // HCP_L
            voxel_radius * sqrt(8.0 / 3.0), // HCP_X
            voxel_radius * sqrt(3.0)); // HCP_Y
    }

    static inline Integer3 calculate_shape(const Real3& edge_lengths, const Real& voxel_radius, const bool is_periodic)
    {
        const Real3 hcpLXY = calculate_hcp_lengths(voxel_radius);
        const Real lengthX = edge_lengths[0];
        const Real lengthY = edge_lengths[1];
        const Real lengthZ = edge_lengths[2];

        Integer col_size = (Integer)rint(lengthX / hcpLXY[1]) + 1;
        Integer layer_size = (Integer)rint(lengthY / hcpLXY[2]) + 1;
        Integer row_size = (Integer)rint((lengthZ / 2) / voxel_radius) + 1;

        if (is_periodic)
        {
            // The number of voxels in each axis must be even for a periodic boundary.
            col_size = (col_size % 2 == 0 ? col_size : col_size + 1);
            layer_size = (layer_size % 2 == 0 ? layer_size : layer_size + 1);
            row_size = (row_size % 2 == 0 ? row_size : row_size + 1);
        }

        return Integer3(col_size, row_size, layer_size);
    }

    static inline Real calculate_volume(const Real3& edge_lengths, const Real& voxel_radius, const bool is_periodic)
    {
        const Integer3 shape = calculate_shape(edge_lengths, voxel_radius, is_periodic);
        return static_cast<Real>(shape[0] * shape[1] * shape[2]) * calculate_voxel_volume(voxel_radius);
    }

    // Member function

    virtual std::vector<Species> list_species() const = 0;

    virtual Integer num_voxels_exact(const Species& sp) const = 0;
    virtual Integer num_voxels(const Species& sp) const = 0;
    virtual Integer num_voxels() const = 0;
    virtual bool has_voxel(const ParticleID& pid) const = 0;

    virtual std::vector<std::pair<ParticleID, Voxel> > list_voxels() const = 0;
    virtual std::vector<std::pair<ParticleID, Voxel> > list_voxels(const Species& sp) const = 0;
    virtual std::vector<std::pair<ParticleID, Voxel> > list_voxels_exact(const Species& sp) const = 0;

    virtual std::pair<ParticleID, Voxel> get_voxel(const ParticleID& pid) const = 0;

    /*
     * find_voxel_pool
     */
    virtual VoxelPool* find_voxel_pool(const Species& sp) = 0;
    virtual const VoxelPool* find_voxel_pool(const Species& sp) const = 0;

    virtual bool has_molecule_pool(const Species& sp) const = 0;

    virtual MoleculePool* find_molecule_pool(const Species& sp) = 0;
    virtual const MoleculePool* find_molecule_pool(const Species& sp) const = 0;

};

} // ecell4

#endif /* ECELL4_LATTICE_SPACE_HPP */
