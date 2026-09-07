#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
// Union to store 4 bytes as an array of bytes, an unsigned, signed, or float number
union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};
// reads 8 hex characters from string input and stores it in the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input);

// converts the ASCII hex character c to binary
// returns the hex value of c if c is a valid hex digit, -1 otherwise
char hexDigit(char c);

// returns true if x has any even bit equal to 1, 0 otherwise
int any_even_one(unsigned x){
    unsigned mask = 0x55555555;
    return mask & x;
}
// returns a mask indicating the position of the left most one in x
int leftmost_one(unsigned x){
    unsigned mask = 0x80000000;
    if (x == 0) {
        return 0;
    }
    while ((mask & x) == 0) {
        mask >>= 1;
    }
    return mask;
}
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n){
    if (n == 0) {
        return x;
    }

    unsigned left_mask = x << n;
    unsigned right_mask = x >> (32 - n);
    return left_mask | right_mask;

}
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n){
    if (n == 0) {
        return x;
    }

    unsigned right_mask = x >> n;
    unsigned left_mask = x << (32-n);
    return right_mask | left_mask;

}

// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y) {

    unsigned ux = (unsigned) x;
    unsigned uy = (unsigned) y;

    unsigned sum = ux + uy;

    unsigned sign_mask = 0x80000000;

    // // Check if they're both positive and sum became nonnegative
    if (!(ux & sign_mask) &&
        !(uy & sign_mask) &&
        (sum & sign_mask)) {

        return INT_MAX;
    }

    /// Check if they're both negative and sum became positve   
    if ((ux & sign_mask) &&
        (uy & sign_mask) &&
        !(sum & sign_mask)) {

        return INT_MIN;
    }

    return (int) sum;
}

// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f) {

    unsigned sign = f & 0x80000000;
    unsigned exp  = f & 0x7f800000;
    unsigned frac = f & 0x007fffff;

    // NaN or infinity
    if (exp == 0x7f800000) {
        return f;
    }

    // denormalized
    if (exp == 0) {
        return sign | ((f & 0x7fffffff) << 1);
    }

    // normalized
    exp += 0x00800000;

    // Overflow to infinity
    if (exp == 0x7f800000) {
        frac = 0;
    }

    return sign | exp | frac;
}


// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f){
    unsigned sign = f & 0x80000000;
    unsigned exp  = f & 0x7f800000;
    unsigned frac = f & 0x007fffff;

    // NaN or infinity
    if (exp == 0x7f800000) {
        return f;
    }

    // denormalized
    if (exp <= 0x00800000) {

        unsigned magnitude = f & 0x7fffffff;

        unsigned rounded =
            (magnitude >> 1) + ((magnitude & 3) == 3);

        return sign | rounded;
    }

    // normalized
    exp -= 0x00800000;

    // Overflow to infinity
    if (exp == 0x7f800000) {
        frac = 0;
    }

    return sign | exp | frac;
}

int main(int argc, char **argv) {

    if (argc != 3 && argc != 4) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    union value v;

    //end to error check if strtoul parses correctly
    char *end;

    v.uval = strtoul(argv[2], &end, 16);

    if (*end != '\0' || end == argv[2]) {
        printf("Invalid hex value\n");
        return 1;
    }

    if (strcmp(argv[1], "even") == 0) {

        int num = any_even_one(v.uval);
        //convert to true/false
        printf("%s\n", num ? "True" : "False");


    } else if (strcmp(argv[1], "left") == 0) {

        unsigned num = leftmost_one(v.uval);

        printf("%08x\n", num);


    } else if (strcmp(argv[1], "rrotate") == 0) {

        if (argc != 4) {
            printf("Invalid number of arguments\n");
            return 1;
        }

        char *end2;

        long n = strtol(argv[3], &end2, 10);

        //Make sure n is valid
        if (*end2 != '\0' ||
            end2 == argv[3] ||
            n < 0 ||
            n > 31) {

            printf("Invalid number of shift positions\n");
            return 1;
        }

        unsigned num = rotate_right(v.uval, (int)n);

        printf("%08x\n", num);


    } else if (strcmp(argv[1], "lrotate") == 0) {

        if (argc != 4) {
            printf("Invalid number of arguments\n");
            return 1;
        }

        char *end2;

        long n = strtol(argv[3], &end2, 10);

        if (*end2 != '\0' ||
            end2 == argv[3] ||
            n < 0 ||
            n > 31) {

            printf("Invalid number of shift positions\n");
            return 1;
        }

        unsigned num = rotate_left(v.uval, (int)n);

        printf("%08x\n", num);


    } else if (strcmp(argv[1], "saturate") == 0) {

        if (argc != 4) {
            printf("Invalid number of arguments\n");
            return 1;
        }

        char *end2;

        unsigned second = strtoul(argv[3], &end2, 16);

        if (*end2 != '\0' || end2 == argv[3]) {
            printf("Invalid hex value\n");
            return 1;
        }

        int num = saturating_add((int)v.uval, (int)second);

        printf("%08x %d\n", (unsigned)num, num);


    } else if (strcmp(argv[1], "twice") == 0) {

        unsigned num = float_twice(v.uval);

        union value result;
        result.uval = num;

        printf("%08x %e\n", num, result.fval);


    } else if (strcmp(argv[1], "half") == 0) {

        unsigned num = float_half(v.uval);

        union value result;
        result.uval = num;

        printf("%08x %e\n", num, result.fval);


    } else {

        printf("Invalid operation\n");
        return 1;
    }

    return 0;
}