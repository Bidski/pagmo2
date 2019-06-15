/* Copyright 2017-2018 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#include <stdexcept>
#include <utility>

#include <pagmo/exceptions.hpp>
#include <pagmo/island.hpp>
#include <pagmo/islands/thread_island.hpp>
#include <pagmo/s11n.hpp>
#include <pagmo/threading.hpp>

namespace pagmo
{

/// Run evolve.
/**
 * This method will use copies of <tt>isl</tt>'s
 * algorithm and population, obtained via island::get_algorithm() and island::get_population(),
 * to evolve the input island's population. The evolved population will be assigned to \p isl
 * using island::set_population(), and the algorithm used for the evolution will be assigned
 * to \p isl using island::set_algorithm().
 *
 * @param isl the pagmo::island that will undergo evolution.
 *
 * @throws std::invalid_argument if <tt>isl</tt>'s algorithm or problem do not provide
 * at least the pagmo::thread_safety::basic thread safety guarantee.
 * @throws unspecified any exception thrown by:
 * - island::get_algorithm(), island::get_population(),
 * - island::set_algorithm(), island::set_population(),
 * - algorithm::evolve().
 */
void thread_island::run_evolve(island &isl) const
{
    const auto i_ts = isl.get_thread_safety();
    if (i_ts[0] < thread_safety::basic) {
        pagmo_throw(std::invalid_argument,
                    "the 'thread_island' UDI requires an algorithm providing at least the 'basic' "
                    "thread safety guarantee");
    }
    if (i_ts[1] < thread_safety::basic) {
        pagmo_throw(std::invalid_argument, "the 'thread_island' UDI requires a problem providing at least the 'basic' "
                                           "thread safety guarantee");
    }
    // Get out a copy of the algorithm for evolution.
    auto algo = isl.get_algorithm();
    // Replace the island's population with the evolved population.
    isl.set_population(algo.evolve(isl.get_population()));
    // Replace the island's algorithm with the algorithm used for the evolution.
    // NOTE: if set_algorithm() fails, we will have the new population with the
    // original algorithm, which is still a valid state for the island.
    isl.set_algorithm(algo);
}

/// Serialization support.
/**
 * This class is stateless, no data will be saved to or loaded from the archive.
 */
template <typename Archive>
void thread_island::serialize(Archive &, unsigned)
{
}

} // namespace pagmo

PAGMO_S11N_ISLAND_IMPLEMENT(pagmo::thread_island)
