/*
 * A hash table implementation based on the MurmurHash 2.0 hash function
 *
 * Copyright (c) 2008-2013 Daniel Corbe
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hash.h"

#ifdef DEBUG
#include <sys/time.h>
uint32_t collisions;           /* Used to track the number of hash collisions */
struct timeval *start;         /* Used for calculating collisions per second */
#endif /* DEBUG */

void hash_init(hash_t **hash)
{
        int i, j;
        hash_t *h;

#ifdef DEBUG
        gettimeofday(start, NULL);
        collisions = 0;
#endif /* DEBUG */

        for (i = 0; i < HASHSIZE; i++)
        {
                hash[i] = malloc(sizeof(struct hashnode));
                h = hash[i];

                h->key.s = NULL;
                h->key.l = 0;
                h->value = NULL;
                h->depth = 6;

                /* Initialize the chain */
                /* Assume a default chain depth of 6 */
                for (j = 0; j < 6; j++)
                {
                        h->next = malloc(sizeof(struct hashnode));
                        h = h->next;
                        h->key.s = NULL;
                        h->key.l = 0;
                        h->value = NULL;
                        h->next = NULL;
                }
        }
}

hash_t *hash_insert(char *k, hash_t **hash, void *val, size_t kl)
{
        uint32_t hk;            /* Hash key */
        hash_t *h;              /* Hash pointer */

        if (!hash_lookup(k, hash, kl))
        {
                /* not found, insert key */
                hk = MurmurHash2(k, kl);
                h = hash[hk % HASHSIZE];
                while (h->hkey)
                {
#ifdef DEBUG
                        collisions++;
#endif /* DEBUG */
                        h = h->next;
                }
                h->hkey = hk;
                h->key.s = k;
                h->key.l = kl;
                h->value = val;
                return(h);
        }
        else
        {
                /* found, duplicate key */
                return(NULL);
        }
}

hash_t *hash_insert_byhkey(char *k, hash_t **hash, void *val, size_t kl)
{
        uint32_t hk;            /* Hash key */
        hash_t *h;              /* Hash pointer */

        if (!hash_lookup_byhkey(k, hash, kl))
        {
                hk = MurmurHash2(k, kl);
                h = hash[hk % HASHSIZE];
                while (h->hkey)
                {
#ifdef DEBUG
                        collisions++;
#endif /* DEBUG */
                        h = h->next;
                }
                h->hkey = hk;
                h->key.s = k;
                h->key.l = kl;
                h->value = val;
                return(h);
        }
        else
        {
                /* found, duplicate key */
                return(NULL);
        }

}

inline hash_t *hash_lookup(char *k, hash_t **hash, size_t kl)
{
        uint32_t hk;            /* Hash key */
        hash_t *h;              /* Hash pointer */
        int i;

        /* Calculate hash key and point h to the hash node */
        hk = MurmurHash2(k, kl);
        h = hash[hk % HASHSIZE];

        /* Calculate hash key and point h to the hash node */
        hk = MurmurHash2(k, kl);
        h = hash[hk % HASHSIZE];

        for (i = 0; i < h->depth; i++)
        {
                /* Compare stored key (and size) with supplied key */
                if (h->key.l > 0 && strncmp(h->key.s, k, h->key.l) == 0)
                {
                        return(h);
                }
                h = h->next;
        }

        /* If we get down here we didn't find a matching key */
        return(NULL);
}

inline hash_t *hash_lookup_byhkey(char *k, hash_t **hash, size_t kl)
{
        uint32_t hk;            /* Hash key */
        hash_t *h;              /* Hash pointer */
        int i;

        /* Calculate hash key and point h to the hash node */
        hk = MurmurHash2(k, kl);
        h = hash[hk % HASHSIZE];

        for (i = 0; i < h->depth; i++)
        {
                if (hk == h->hkey)
                {
                        return(h);
                }
                h = h->next;
        }

        /* If we get down here we didn't find a matching key */
        return(NULL);
}

void hash_remove(char *k, hash_t **hash, size_t kl)
{
        hash_t *h;              /* Hash pointer */
        if ((h = hash_lookup(k, hash, kl)))
        {
                h->hkey = 0;
                h->key.l = 0;
                h->key.s = NULL;
                h->value = NULL;
        }
}

void hash_remove_byhkey(char *k, hash_t **hash, size_t kl)
{
        hash_t *h;              /* Hash pointer */
        if ((h = hash_lookup_byhkey(k, hash, kl)))
        {
                h->hkey = 0;
                h->key.l = 0;
                h->key.s = NULL;
                h->value = NULL;
        }
}

#ifdef DEBUG
void hash_dump(hash_t **hash)
{
        hash_t *h;
        int i, j;

        for (i = 0; i < HASHSIZE; i++)
        {
                h = hash[i];
                for (j = 0; j < h->depth; j++)
                {
                        if (h->hkey)
                        {
                                printf("Key: %.*s Value Pointer: 0x%x\n", h->key.l, h->key.s, h->value);
                                h = h->next;
                        }
                }
        }
}
#endif /* DEBUG */

/*-----------------------------------------------------------------------------
 MurmurHash2, originally by Austin Appleby

 Note - This code makes a few assumptions about how your machine behaves -

 1. We can read a 4-byte value from any address without crashing
 2. sizeof(int) == 4

 And it has a few limitations -

 1. It will not work incrementally.
 2. It will not produce the same results on little-endian and big-endian
    machines.
 */
/*
 * Since we're using this to generate hashes suitable for use in hash tables,
 * they no longer need to be cryptographically secure, therefore the "seed" 
 * is now static and the function no longer expects the caller to seed the 
 * hash.               -Daniel Corbe (December 08)
 */
unsigned int MurmurHash2(const void *key, int len)
{
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.

        const unsigned int m = 0x5bd1e995;
        const int r = 24;
        const unsigned int seed = 1228458515;

        // Initialize the hash to a 'random' value

        unsigned int h = seed ^ len;

        // Mix 4 bytes at a time into the hash

        const unsigned char * data = (const unsigned char *)key;

        while(len >= 4)
        {
                unsigned int k = *(unsigned int *)data;

                k *= m; 
                k ^= k >> r; 
                k *= m; 

                h *= m; 
                h ^= k;

                data += 4;
                len -= 4;
        }

        // Handle the last few bytes of the input array

        switch(len)
        {
                case 3: h ^= data[2] << 16;
                case 2: h ^= data[1] << 8;
                case 1: h ^= data[0];
                        h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
} 
