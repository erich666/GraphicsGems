/***********************************************************
Given an s-simplex (with s+1 vertexes) in n dimensions,
calculate the vertexes of the kth (0 <= k < (1<<s))
subsimplex in the recursive subdivision of the simplex.

Several implementations of the bitCount() function are
described in "Of Integers, Fields, and Bit Counting",
Paeth and Schilling, Graphics Gems II.

Entry:
  src_vtx - list of the vertexes of the original simplex
  n - each n consecutive floats represents one vertex
  s - there are s+1 vertexes in the s-simplex
  k - identifies which subsimplex is to be generated
Exit:
  dst_vtx - list of the vertexes of the kth subsimplex
  
***********************************************************/
void rec_subsimplex(register float* dst_vtx,
                    const float* const src_vtx,
                    int n,
                    int s,
                    int k)
{
  int id[2];
  id[1] = n*bitCount(k);
  id[0] = -id[1];
  
  for (int j = 0; j <= s; ++j)
    {
      for (int i = 0; i < n; ++i)
        *dst_vtx++ = (src_vtx[i-id[0]] + src_vtx[i+id[1]]) / 2.0;
      id[ (k&1)] += n;
      k >>= 1;
    }
}
