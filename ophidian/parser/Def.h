#ifndef OPHIDIAN_PARSER_DEF_H
#define OPHIDIAN_PARSER_DEF_H

#include <string>
#include <vector>
#include <DEF/include/defrReader.hpp>

namespace ophidian {
    namespace parser {
        /** @brief Def reads a .def file and store necessary data.
         *
         * This is an encapsulation of the DEF library made by 
         * Cadence Design Systems to present the rows, components, 
         * die area and the units of a given circuit in a usable 
         * way.
         */
        class Def {
        public:
            /**
             * Type to represent a bidimentional point
             * @tparam T point coordinate type
             */
            template <class T>
            struct point {
                T x, y;
            };
            
            /** 
             * @brief Type to represent two oposite @c point<int> 
             * 
             * The points @c lower and @c upper represent the 
             * dimentions and location of an area on a block.
             */
            struct dieArea {
                point<int> lower;
                point<int> upper;
            };

            /** 
             * @brief Type to represent a circuit component.
             *
             * This is the data necessary to identify a given
             * component and it's characteristics.
             */
            struct component {
                std::string name; ///< Component's name for identification.
                std::string macro; ///< Component's type, like "NAND2_X1".
                std::string orientation; ///< Component's orientation, like "N" for north.
                point<int> position; ///< Component's lower left corner.
                bool fixed; ///< This determines if the component's position is fixed in space, @c true for fixed. 
            };

            /**
             * @brief Type to represent a circuit row. 
             *
             * This if the data necessary to identify a given
             * row and it's characteristics.
             */
            struct row {
                std::string name; ///< Row's name for identification.
                std::string site; ///< This is the site to be used by the row defined by a LEF file. 
                point<double> origin; ///< Specifies the location of the first site in the row.
                point<double> step; ///< Specifies the spacing between sites in horizontal and vertical rows.
                point<double> num; ////< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
            };

            /** 
             * @brief Constructor.
             *
             * Construct the object by using the DEF lib.
             *
             * @param filename Path to a .def file, if file does not exist the 
             * DEF lib will try to open a null_ptr giving a segmantation fault.
             */
            Def(const std::string& filename);
            ~Def();

            /** 
             * Returns the @c dieArea.
             */
            const dieArea& die() const {
                return m_die;
            }

            /**
             * Returns a @c std::vector<component> with 
             * all components.
             */
            const std::vector<component>& components() const {
                return m_components;
            }

            /**
             * Returns a @c std::vector<row> with all rows. 
             */
            const std::vector<row>& rows() const {
                return m_rows;
            }

            /**
             * Returns the DEF database units.
             */
            double database_units() const {
                return m_units;
            }
        private:
            dieArea m_die;
            double m_units;
            std::vector<component> m_components;
            std::vector<row> m_rows;
        };

    }
}

#endif /*OPHIDIAN_PARSER_DEF_H*/
