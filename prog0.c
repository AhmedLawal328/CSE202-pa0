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
    
    while ((mask & x) == 0) {
        mask >>= 1;
    }
    return mask;
}
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n){
    // 0x02001210 << 
    unsigned left_mask = x << n;
    unsigned right_mask = x >> 32 - n;
    return left_mask | right_mask;

}
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n){

    unsigned right_mask = x >> n;
    unsigned left_mask = x << 32-n;
    return right_mask | left_mask;

}
// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y){

    int sum = x + y;
    
    // Check if they're both positive and sum became nonnegative
    if ((x >> 31) == 0 && (y >> 31) == 0 && (sum >> 31) != 0) {
        return 0x7fffffff;   // TMAX
    }

    /// Check if they're both negative and sum became positve   
    if ((x >> 31) != 0 && (y >> 31) != 0 && (sum >> 31) == 0) {
        return 0x80000000;   // TMIN
    }
    return sum;
}


// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f){

    //seperate the exp
    unsigned exp = (f & 0x7f800000) >> 23;

    // Check if exp is all 1s
    if (exp == 0xff) {
        return f;
    }

    // Normalized number
    return f + 0x00800000;

}
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f);

int main(int argc, char **argv) {

    if (argc != 3 && argc != 4) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    union value v;  
    char *end;

    v.uval = strtoul(argv[2], &end, 16);

    if (*end != '\0') {
        printf("Invalid hex value\n");
        return 1;
    }

    if (strcmp(argv[1], "even") == 0) {

        int num = any_even_one(v.uval);
        printf("%s\n", num ? "True" : "False");

    } else if (strcmp(argv[1], "left") == 0) {

        unsigned num = leftmost_one(v.uval);
        printf("%x\n", num);

    } else if (strcmp(argv[1], "rrotate") == 0) {

        // Get digit by subtracting '0'
        char *end2;
        int n = strtoul(argv[3], &end2, 10);

        if (*end2 != '\0' || n < 0 || n > 31) {
            printf("Invalid number of shift positions\n");
            return 1;
        }

        unsigned num = rotate_right(v.uval, n);
        printf("%x\n", num);

    } else if (strcmp(argv[1], "lrotate") == 0) {

        // Get digit by subtracting '0'
        char *end2;
        int n = strtoul(argv[3], &end2, 10);

        if (*end2 != '\0' || n < 0 || n > 31) {
            printf("Invalid number of shift positions\n");
            return 1;
        }

        unsigned num = rotate_left(v.uval, n);
        printf("%x\n", num);

    } else if (strcmp(argv[1], "saturate") == 0) {

        char *end2;
        int sec_arg = (int) strtoul(argv[3], &end2, 16);

        if (*end2 != '\0') {
            printf("Invalid hex value\n");
            return 1;
        }

        int num = saturating_add((int) v.uval, sec_arg);
        printf("%d\n", num);

    } else if (strcmp(argv[1], "twice") == 0) {

        unsigned num = float_twice(v.uval);
        printf("%x\n", num);

    } else if (strcmp(argv[1], "half") == 0) {


    } else if (strcmp(argv[1], "add") == 0) {


    } else {
        printf("Invalid operation\n");
        return 1;
    }

    return 0;
}