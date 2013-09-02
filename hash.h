/*
 * Copyright (c) 2008-2012 Daniel Corbe
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors, copyright holders or the contributors
 *    may be used to endorese or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS, AUTHORS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS OR CONTRIBUTORS BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, COPYRIGHT ENFRINGEMENT, LOSS
 * OF PROFITS, REVENUE, OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef HASH_H
#define HASH_H

#define DEBUG

#define HASHSIZE 8192

/* Types we work with */
typedef struct hashkey
{
        char *s;             /* stirng */
        size_t l;            /* length, to avoid having to call strlen() */
} hashkey_t;

typedef struct hashnode
{
        hashkey_t key;          /* Node key */
        uint32_t hkey;		/* Hashed version of the node 
				   key (for comparison purposes) */
        void *value;            /* Node value, let the application 
				   decide the type */
        struct hashnode *next;  /* Used for chaining collisions together */
        uint8_t depth;          /* Chain depth for this node */
} hash_t;

/* Function prototypes */
int main(int argc, char **argv);
unsigned int MurmurHash2(const void * key, int len);
void hash_init(hash_t **hash);
hash_t *hash_insert(char *k, hash_t **hash, void *val, size_t kl);
hash_t *hash_insert_byhkey(char *k, hash_t **hash, void *val, size_t kl);
inline hash_t *hash_lookup(char *k, hash_t **hash, size_t kl);
inline hash_t *hash_lookup_byhkey(char *k, hash_t **hash, size_t kl);
void hash_remove(char *k, hash_t **hash, size_t kl);
void hash_remove_byhkey(char *k, hash_t **hash, size_t kl);
#ifdef DEBUG
void hash_dump(hash_t **hash);
#endif /* DEBUG */

#endif /* HASH_H */
