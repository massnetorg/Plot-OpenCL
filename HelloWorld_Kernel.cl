__kernel void helloworld(__global double* in, __global double* out)
{
    int num = get_global_id(0);
    out[num] = in[num] / 2.4 *(in[num]/6) ;
}