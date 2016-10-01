/**
 * @file Definitions.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_DEFINITIONS_H
#define KRAKEN_DEFINITIONS_H

namespace Kraken
{
    /**
     * The type of a file descriptor.
     */
    using fd_t = int;
}

/* Features */
#if defined(KRAKEN_OPT_PRINT_ON_ERROR)
# include<stdio.h>
# define KRAKEN_PRINT(msg, ...) fprintf(stderr, "[%s:%s:%u]" msg, __FILE__, __FUNCTION__, __LINE__,##__VA_ARGS__)
#else
# define KRAKEN_PRINT(msg, ...)
#endif /* KRAKEN_OPT_PRINT_ON_ERROR */

#endif //KRAKEN_DEFINITIONS_H
