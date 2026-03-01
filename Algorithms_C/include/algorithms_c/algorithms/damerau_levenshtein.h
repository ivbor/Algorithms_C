#ifndef ALGORITHMS_C_ALGORITHMS_DAMERAU_LEVENSHTEIN_H
#define ALGORITHMS_C_ALGORITHMS_DAMERAU_LEVENSHTEIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file damerau_levenshtein.h
 * @brief Edit-distance helper with
 * insertion/deletion/substitution/transposition.
 *
 * This module translates the educational Damerau-Levenshtein
 * dynamic-programming routine from the Python repository into reusable C
 * library form.
 */

/**
 * @brief Compute Damerau-Levenshtein distance between two UTF-8 byte strings.
 *
 * The algorithm counts the minimum number of elementary edit operations
 * required to transform ``lhs`` into ``rhs`` where allowed operations are
 * insertion, deletion, substitution, and adjacent transposition.
 *
 * @param lhs Source string. Must not be ``NULL``.
 * @param rhs Target string. Must not be ``NULL``.
 * @return Non-negative edit distance on success, or ``-1`` for invalid inputs
 *         or allocation/size overflow failures.
 * @signature int ac_damerau_levenshtein_distance(const char *lhs,
 *                                                const char *rhs)
 */
int ac_damerau_levenshtein_distance(const char *lhs, const char *rhs);

/**
 * @brief Compatibility alias preserving the Python file spelling
 *        ``damerau_levenstein``.
 *
 * Some educational materials and example names use the historic "levenstein"
 * spelling. This wrapper keeps that name available while delegating to
 * ::ac_damerau_levenshtein_distance.
 *
 * @param lhs Source string. Must not be ``NULL``.
 * @param rhs Target string. Must not be ``NULL``.
 * @return Same value and error semantics as
 *         ::ac_damerau_levenshtein_distance.
 * @signature int ac_damerau_levenstein_distance(const char *lhs,
 *                                               const char *rhs)
 */
int ac_damerau_levenstein_distance(const char *lhs, const char *rhs);

#ifdef __cplusplus
}
#endif

#endif
