/**
 * Copyright (c) 2016 Gilad Naaman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file Features.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   14/10/2016
 */

#ifndef KRAKEN_FEATURES_H
#define KRAKEN_FEATURES_H

/**
 * Available feature removal macros:
 *  - KRAKEN_OPT_DISABLE_READV
 *  - KRAKEN_OPT_DISABLE_PREADV
 *  - KRAKEN_OPT_DISABLE_WRITEV
 *  - KRAKEN_OPT_DISABLE_PWRITEV
 *
 * Available missing feature handlers:
 *  - KRAKEN_OPT_MISSING_FUNC_ABORT
 *  - KRAKEN_OPT_MISSING_FUNC_EMPTY
 */

#if defined(KRAKEN_OPT_MISSING_FUNC_ABORT)
 #define HANDLE_MISSING_FUNCTION(ret_type, name, ...) ret_type name(__VA_ARGS__) { abort(); }
#elif defined(KRAKEN_OPT_MISSING_FUNC_EMPTY)
 #define HANDLE_MISSING_FUNCTION(ret_type, name, ...) ret_type name(__VA_ARGS__) { return {}; }
#else
 #define HANDLE_MISSING_FUNCTION(...)
#endif

#endif //KRAKEN_FEATURES_H
