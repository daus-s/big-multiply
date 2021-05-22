#include <stdint.h>
#include <stdio.h>

//
// Created by Daus Carmichael on 5/11/21.
//

/**
 * @param as - array-number 1
 * @param bs - array-number 2
 * @param sz_a - number of digits in as
 * @param sz_b - number of digits in bs
 * @return c - carry value
 *
 * compute the value of as += bs, filling as, return a carry "bit"
 * */
uint8_t addto32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b)
{
    // Assume that sz_b <= sz_a
    // Compute as += bs
    int i;
    uint32_t c = 0;
    uint64_t s;

    for (i=0; i< sz_b; i++)
    {
        s = (uint64_t) as[i] + (uint64_t) bs[i] + (uint64_t) c; // s is a 33 bit value
        c = s >> 32;
        as[i] = (uint32_t) s;
    }
    for ( ; i< sz_a; i++)
    {
        s = (uint64_t) as[i] + (uint64_t) c; // s is a 33 bit value
        c = s >> 32;
        as[i] = (uint32_t) s;
    }
    return c;
}

void partialprod32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b, uint32_t d)
{
    for (int i = 0; i < sz_b; ++i)
    {
        uint64_t p = bs[i]*d; //product which is 2 digits long; see decimal analogy
        //we need to add p to as[] in the correct spot
        //we could use a populated with zeroesand then set but that is inefficent
        //so lets split product into 2
        uint32_t lo = (uint32_t)p;
        uint32_t hi = p >> 32;
        //great now where and how do we add these

        //break it down
        uint32_t added[2] = {lo, hi};
        uint32_t add_to[2] = {as[i], as[i+1]};

        uint32_t c = addto32(add_to, 2, added, 2);

        as[i] = add_to[0];
        as[i+1] = add_to[1];

        if (c) //this is ok it wont overflow see proof #2
        {
            as[i+2] += c;
        }
    }
}
//as[] = bs[] * cs[]
void bigmul64(uint64_t a[], int sz_a, uint64_t b[], sz_b, uint64_t c[], int sz_c)
{
    uint32_t as[2*sz_a];
    uint32_t bs[2*sz_b];
    uint32_t cs[2*sz_c];

    for (int i = 0; i < sz_a; ++i)
    {
        as[2*i]=(uint32_t)a[i];
        as[2*i+1]=a[i]>>32;
    }
    for (int i = 0; i < sz_c; ++i)
    {
        cs[2*i]=(uint32_t)c[i];
        cs[2*i+1]=c[i]>>32;
    }
    for (int i = 0; i < sz_b; ++i)
    {
        bs[2*i]=(uint32_t)b[i];
        bs[2*i+1]=b[i]>>32;
    }

    sz_a *= 2;
    sz_b *= 2;
    sz_c *= 2;
    //convert to  uint32_t


    //sz_a > sz_b + sz_c
    for (int i = 0; i < sz_c; ++i)
    {
        //find product
        uint32_t prod[sz_b+1];
        partialprod32(prod, sz_b+1, bs, sz_bs, cs[i]);
        prod[sz_b] += c;

        uint32_t add[sz_b+1+i];
        for (int j = sz_b+1+i; j > 0; --j)
        {
            if (j>i)
            {
               add[j] = prod[j-i];
            }
            else
            {
                add[j]=0;
            }
        }
        c = addto32(as, sz_a, add,  sz_b+1+i);
    }

    for (int i = 0; i < sz_a; i+=2)
    {
        uint64_t lo = as[i];
        uint64_t hi = as[i+1];

        hi >>= 32;
        uint64_t temp =  lo + hi;
        a[i/2] = temp;
    }

}


void main(char** argv, int argc)
{
    //anylengths st sz_a >= sz_b + sz_c
    uint64_t a[48];
    uint64_t b[32];
    uint64_t c[16];

    //populate b,c
    bigmul64(a, 48, b, 32, c, 16);
    for (int i = 0; i < 48; ++i)
    {
        printf("[%d],", a[i]);
    }


}
