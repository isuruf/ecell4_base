#ifndef __ECELL4_SUBVOLUME_SPACE_HPP
#define __ECELL4_SUBVOLUME_SPACE_HPP

#include "get_mapper_mf.hpp"
#include "types.hpp"
#include "exceptions.hpp"
#include "Species.hpp"
#include "Space.hpp"
#include "Integer3.hpp"
#include "Shape.hpp"

#ifdef WITH_HDF5
#include "SubvolumeSpaceHDF5Writer.hpp"
#endif


namespace ecell4
{

class SubvolumeSpace
    : public Space
{
public:

    typedef Integer coordinate_type;

public:

    SubvolumeSpace()
        : t_(0.0)
    {
    }

    virtual ~SubvolumeSpace()
    {
        ;
    }

    // SpaceTraits

    const Real& t() const
    {
        return t_;
    }

    void set_t(const Real& t)
    {
        if (t < 0.0)
        {
            throw std::invalid_argument("the time must be positive.");
        }
        t_ = t;
    }

    virtual Integer num_molecules(const Species& sp) const
    {
        return num_molecules_exact(sp);
    }

    virtual Integer num_molecules_exact(const Species& sp) const
    {
        throw NotImplemented("num_molecules_exact(const Species&) not implemented");
    }

    virtual Real get_value(const Species& sp) const
    {
        return static_cast<Real>(num_molecules(sp));
    }

    virtual Real get_value_exact(const Species& sp) const
    {
        return static_cast<Real>(num_molecules_exact(sp));
    }

    virtual const Integer3 matrix_sizes() const = 0;
    virtual const Real3 subvolume_edge_lengths() const = 0;
    virtual const Integer num_subvolumes() const = 0;
    virtual const Integer num_subvolumes(const Species& sp) const = 0;
    virtual const Real subvolume() const = 0;

    virtual coordinate_type global2coord(const Integer3& g) const = 0;
    virtual Integer3 coord2global(const coordinate_type& c) const = 0;
    virtual Integer3 position2global(const Real3& pos) const = 0;

    inline Integer position2coordinate(const Real3& pos) const
    {
        return global2coord(position2global(pos));
    }

    virtual Integer num_molecules(
        const Species& sp, const coordinate_type& c) const = 0;
    virtual Integer num_molecules_exact(
        const Species& sp, const coordinate_type& c) const = 0;
    virtual void add_molecules(
        const Species& sp, const Integer& num, const coordinate_type& c) = 0;
    virtual void remove_molecules(
        const Species& sp, const Integer& num, const coordinate_type& c) = 0;
    virtual const std::vector<Species>& species() const = 0;
    virtual std::vector<Species> list_species() const = 0;
    virtual coordinate_type get_neighbor(
        const coordinate_type& c, const Integer rnd) const = 0;

    virtual Integer num_molecules(const Species& sp, const Integer3& g) const
    {
        return num_molecules(sp, global2coord(g));
    }

    virtual Integer num_molecules_exact(const Species& sp, const Integer3& g) const
    {
        return num_molecules_exact(sp, global2coord(g));
    }

    virtual void add_molecules(const Species& sp, const Integer& num, const Integer3& g)
    {
        add_molecules(sp, num, global2coord(g));
    }

    virtual void remove_molecules(const Species& sp, const Integer& num, const Integer3& g)
    {
        remove_molecules(sp, num, global2coord(g));
    }

    virtual std::vector<coordinate_type> list_coordinates(const Species& sp) const = 0;
    virtual std::vector<coordinate_type> list_coordinates_exact(const Species& sp) const = 0;

    virtual void add_structure(
        const Species& sp, const boost::shared_ptr<const Shape>& shape) = 0;
    virtual bool check_structure(
        const Species::serial_type& serial, const coordinate_type& coord) const = 0;
    virtual Real get_volume(const Species& sp) const = 0;
    virtual std::vector<Species::serial_type> list_structures() const = 0;
    virtual void update_structure(
        const Species::serial_type& serial, const coordinate_type& coord,
        const Real& value) = 0;
    virtual bool has_structure(const Species& sp) const = 0;

    virtual Real get_occupancy(const Species::serial_type& serial, const coordinate_type& coord) const = 0;

    inline Real get_occupancy(const Species& sp, const coordinate_type& coord) const
    {
        return get_occupancy(sp.serial(), coord);
    }

    inline Real get_occupancy(const Species& sp, const Integer3& g) const
    {
        return get_occupancy(sp.serial(), global2coord(g));
    }

    // virtual Shape::dimension_kind get_dimension(const Species::serial_type& serial) const = 0;
    // inline Shape::dimension_kind get_dimension(const Species& sp) const
    // {
    //     return get_dimension(sp.serial());
    // }

    // virtual void set_dimension(
    //     const Species::serial_type& serial, const Shape::dimension_kind& ndim);

    inline bool check_structure(const Species::serial_type& serial, const Integer3& g) const
    {
        return check_structure(serial, global2coord(g));
    }

    inline bool check_structure(const Species& sp, const Integer3& g) const
    {
        return check_structure(sp.serial(), g);
    }

    virtual void reset(const Real3& edge_lengths, const Integer3& matrix_sizes) = 0;

#ifdef WITH_HDF5
    virtual void save(H5::Group* root) const = 0;
    virtual void load(const H5::Group& root) = 0;
#endif

protected:

    double t_;
};

class SubvolumeSpaceVectorImpl
    : public SubvolumeSpace
{
public:

    typedef SubvolumeSpace base_type;
    typedef base_type::coordinate_type coordinate_type;

    typedef std::vector<Integer> cell_type;
    typedef utils::get_mapper_mf<Species, cell_type>::type matrix_type;

    // typedef utils::get_mapper_mf<Species::serial_type, Shape::dimension_kind>::type structure_container_type;
    typedef std::vector<Real> structure_cell_type;
    typedef utils::get_mapper_mf<Species::serial_type, structure_cell_type>::type structure_matrix_type;

public:

    SubvolumeSpaceVectorImpl(
        const Real3& edge_lengths, const Integer3 matrix_sizes)
        : base_type()
    {
        matrix_sizes_[0] = matrix_sizes.col;
        matrix_sizes_[1] = matrix_sizes.row;
        matrix_sizes_[2] = matrix_sizes.layer;

        reset(edge_lengths);
    }

    virtual ~SubvolumeSpaceVectorImpl()
    {
        ;
    }

    const Real3& edge_lengths() const
    {
        return edge_lengths_;
    }

    const Integer3 matrix_sizes() const
    {
        return Integer3(matrix_sizes_[0], matrix_sizes_[1], matrix_sizes_[2]);
    }

    const Real3 subvolume_edge_lengths() const
    {
        return Real3(
            edge_lengths_[0] / matrix_sizes_[0],
            edge_lengths_[1] / matrix_sizes_[1],
            edge_lengths_[2] / matrix_sizes_[2]);
    }

    const Real volume() const
    {
        return edge_lengths_[0] * edge_lengths_[1] * edge_lengths_[2];
    }

    const Integer num_subvolumes() const
    {
        return matrix_sizes_[0] * matrix_sizes_[1] * matrix_sizes_[2];
    }

    const Integer num_subvolumes(const Species& sp) const;

    const Real subvolume() const
    {
        return volume() / num_subvolumes();
    }

    coordinate_type global2coord(const Integer3& g) const
    {
        const coordinate_type coord(
            modulo(g.col, matrix_sizes_[0])
                + matrix_sizes_[0] * (modulo(g.row, matrix_sizes_[1])
                    + matrix_sizes_[1] * modulo(g.layer, matrix_sizes_[2])));
        return coord;
    }

    Integer3 coord2global(const coordinate_type& c) const
    {
        const Integer rowcol(matrix_sizes_[0] * matrix_sizes_[1]);
        const Integer layer(static_cast<Integer>(c / rowcol));
        const Integer surplus(c - layer * rowcol);
        const Integer row(static_cast<Integer>(surplus / matrix_sizes_[0]));
        return Integer3(surplus - row * matrix_sizes_[0], row, layer);
    }

    Integer3 position2global(const Real3& pos) const
    {
        return Integer3(
            static_cast<Integer>(floor(pos[0] * matrix_sizes_[0] / edge_lengths_[0])),
            static_cast<Integer>(floor(pos[1] * matrix_sizes_[1] / edge_lengths_[1])),
            static_cast<Integer>(floor(pos[2] * matrix_sizes_[2] / edge_lengths_[2])));
    }

    Real3 coord2position(const coordinate_type& c) const
    {
        const Real3 lengths(subvolume_edge_lengths());
        const Integer3 g(coord2global(c));
        const Real3 center(
            lengths[0] * (g[0] + 0.5),
            lengths[1] * (g[1] + 0.5),
            lengths[2] * (g[2] + 0.5));
        return center;
    }

    Integer num_molecules(const Species& sp) const;
    Integer num_molecules_exact(const Species& sp) const;

    Integer num_molecules(const Species& sp, const coordinate_type& c) const;
    Integer num_molecules_exact(const Species& sp, const coordinate_type& c) const;
    void add_molecules(const Species& sp, const Integer& num, const coordinate_type& c);
    void remove_molecules(const Species& sp, const Integer& num, const coordinate_type& c);

    std::vector<coordinate_type> list_coordinates(const Species& sp) const;
    std::vector<coordinate_type> list_coordinates_exact(const Species& sp) const;

    void add_structure(const Species& sp, const boost::shared_ptr<const Shape>& shape);
    bool check_structure(const Species::serial_type& serial, const coordinate_type& coord) const;
    Real get_volume(const Species& sp) const;
    std::vector<Species::serial_type> list_structures() const;

    void update_structure(
        const Species::serial_type& serial, const coordinate_type& coord,
        const Real& value);

    bool has_structure(const Species& sp) const
    {
        structure_matrix_type::const_iterator i(structure_matrix_.find(sp.serial()));
        if (i == structure_matrix_.end())
        {
            return false;
        }
        for (structure_cell_type::const_iterator j((*i).second.begin());
            j != (*i).second.end(); ++j)
        {
            if ((*j) > 0)
            {
                return true;
            }
        }
        return false;
    }

    inline Real unit_area() const
    {
        const Real3 l(subvolume_edge_lengths());
        return (l[0] * l[1] + l[0] * l[2] + l[1] * l[2]) / (3 * l[0] * l[1] * l[2]);
    }

    Real get_occupancy(const Species::serial_type& serial, const coordinate_type& coord) const
    {
        structure_matrix_type::const_iterator i(structure_matrix_.find(serial));
        if (i == structure_matrix_.end())
        {
            return 0.0;
        }
        return (*i).second[coord];
    }

    // Shape::dimension_kind get_dimension(const Species::serial_type& serial) const
    // {
    //     structure_container_type::const_iterator i(structures_.find(serial));
    //     if (i == structures_.end())
    //     {
    //         throw NotFound("No correspoinding structure was found.");
    //     }
    //     return (*i).second;
    // }

    // void set_dimension(
    //     const Species::serial_type& serial, const Shape::dimension_kind& ndim)
    // {
    //     structure_container_type::iterator i(structures_.find(serial));
    //     if (i == structures_.end())
    //     {
    //         throw NotFound("No correspoinding structure was found.");
    //     }
    //     else
    //     {
    //         (*i).second = ndim;
    //     }
    // }

    coordinate_type get_neighbor(const coordinate_type& c, const Integer rnd) const;

    const std::vector<Species>& species() const
    {
        return species_;
    }

    std::vector<Species> list_species() const
    {
        // std::vector<Species> retval;
        // for (matrix_type::const_iterator i(matrix_.begin()); i != matrix_.end(); ++i)
        // {
        //     retval.push_back((*i).first);
        // }
        // return retval;
        return species_;
    }

#ifdef WITH_HDF5
    void save(H5::Group* root) const
    {
        save_subvolume_space(*this, root);
    }

    void load(const H5::Group& root)
    {
        load_subvolume_space(root, this);
    }
#endif

    void reset(const Real3& edge_lengths)
    {
        reset(edge_lengths, matrix_sizes());
    }

    void reset(const Real3& edge_lengths, const Integer3& matrix_sizes)
    {
        base_type::t_ = 0.0;
        matrix_.clear();
        species_.clear();

        for (Real3::size_type dim(0); dim < 3; ++dim)
        {
            if (edge_lengths[dim] <= 0)
            {
                throw std::invalid_argument("the edge length must be positive.");
            }
        }

        edge_lengths_ = edge_lengths;

        matrix_sizes_[0] = matrix_sizes.col;
        matrix_sizes_[1] = matrix_sizes.row;
        matrix_sizes_[2] = matrix_sizes.layer;
    }

protected:

    void reserve_species(const Species& sp, const coordinate_type& c);

    void add_structure3(const Species& sp, const boost::shared_ptr<const Shape>& shape);
    void add_structure2(const Species& sp, const boost::shared_ptr<const Shape>& shape);
    bool is_surface_subvolume(const coordinate_type& c, const boost::shared_ptr<const Shape>& shape);

protected:

    Real3 edge_lengths_;
    boost::array<Integer, 3> matrix_sizes_;
    matrix_type matrix_;
    std::vector<Species> species_;

    // structure_container_type structures_;
    structure_matrix_type structure_matrix_;
};

} // ecell4

#endif /* __ECELL4_SUBVOLUME_SPACE_HPP */
