
__global__ void cuAutoAdjust_dev(unsigned char *s, unsigned char *d, int count, int *autoTable, bool invert)
{
  int i = blockDim.x * blockIdx.x + threadIdx.x;

  if (i < count)
  {
    int val = s[i];

    val = autoTable[val];

    if (invert)
    {
      val = 255 - val;
    }

    d[i] = val;
  }
}

__global__ void cuProcessImage_dev(unsigned char *s, unsigned char *d, int count, int *contrastTable, int *gammaTable, bool invert, float brightness)
{
  int i = blockDim.x * blockIdx.x + threadIdx.x;

  if (i < count)
  {
    int val = s[i];

    val += brightness;
    if (val < 0)
    {
      val = 0;
    }
    else
    if( val > 255 )
    {
      val = 255;
    }

    val = contrastTable[val];
    val = gammaTable[val];

    val = invert ? 255 - val : val;

    d[i] = val;
  }
}

extern "C" void cuProcessImage(unsigned char *s, unsigned char *d, int count, int *contrastTable, int *gammaTable, bool invert, float brightness)
{
  int threadsPerBlock = 512;
  int blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;

  cuProcessImage_dev<<<blocksPerGrid, threadsPerBlock>>>(s, d, count, contrastTable, gammaTable, invert, brightness);
}


extern "C" void cuAutoAdjust(unsigned char *s, unsigned char *d, int count, int *autoTable, bool invert)
{
  int threadsPerBlock = 512;
  int blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;

  cuAutoAdjust_dev<<<blocksPerGrid, threadsPerBlock>>>(s, d, count, autoTable, invert);
}
