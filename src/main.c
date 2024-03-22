#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// use this or trle.c
#define _putc(__ch, __out) *__out++ = (__ch)
#define _getc(in, in_) (in<in_?(*in++):-1)
#define _rewind(in,_in) in = _in

// Compression function
int mrlec(unsigned char *in,  int inlen, unsigned char *out) {
  unsigned char *ip = in, *in_ = in+inlen,*op = out; int i;
  int c, pc = -1;  			// current and last char
  long long t[256]={0};  	// byte -> savings
  long long run = 0;  		// current run length
							// Pass 1: determine which chars will compress
  while((c = _getc(ip, in_)) != -1) { 
    if (c == pc) t[c] += (++run%255)!=0;
    else --t[c], run=0;
    pc = c;
  }
  for (i = 0; i < 32; ++i) { int j;
    c = 0;
    for (j=0; j<8; ++j) c += (t[i*8+j]>0)<<j;
    _putc(c, op);
  }
  _rewind(ip,in);  c = pc=-1;  run=0;
  do { c = _getc(ip,in_);
    if(c == pc) ++run;
    else if (run>0 && t[pc]>0) {  _putc(pc, op); 
	  for (; run>255; run-=255){ _putc(255, op);}  _putc(run-1, op); run=1; 
	} else for (++run; run>1; --run) _putc(pc, op);
    pc = c;
  } while(c!=-1);

  return op - out;
}


int main() {
    // Define input values
    double in_values[] = {
        9.739696383476257324e-01,
        9.132320880889892578e-01,
        8.655097484588623047e-01,
        8.232104182243347168e-01,
        7.462038993835449219e-01,
        6.420824527740478516e-01,
        5.477223396301269531e-01,
        4.262472987174987793e-01,
        3.253796100616455078e-01,
        2.798264622688293457e-01,
        2.288503199815750122e-01,
        1.366594433784484863e-01,
        4.772234335541725159e-02,
        1.084598712623119354e-02,
        1.008676812052726746e-01,
        1.887201666831970215e-01,
        2.277657240629196167e-01,
        2.407809048891067505e-01,
        2.570498883724212646e-01,
        2.635574936866760254e-01
    
        
    };

    const int num_elements = sizeof(in_values) / sizeof(double);

    // Convert double values to uint32_t
    uint32_t *uint_data = (uint32_t *)malloc(num_elements * sizeof(uint32_t));
    for (int i = 0; i < num_elements; ++i) {
        uint_data[i] = (uint32_t)(in_values[i] * UINT32_MAX);
    }

    // Compress data
    unsigned char *in = (unsigned char*)uint_data;
    unsigned char *out = (unsigned char*)malloc(num_elements * sizeof(double) * 4 / 3);
    int compressed_size = mrlec(in, num_elements * sizeof(uint32_t), out);

    // Calculate compression rate
    double compression_rate = (double)compressed_size / (num_elements * sizeof(uint32_t)) * 100;

    printf("Compression Rate: %.2f%%\n", compression_rate);

    // Free allocated memory
    free(uint_data);
    free(out);

    return 0;
}