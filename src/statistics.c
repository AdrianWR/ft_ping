#include <math.h>

float *ping_statistics(float rtt, int n)
{
  // Return an array of floats describing the main round-trip ping statistics:
  // [0] Minimum round-trip time
  // [1] Average round-trip time
  // [2] Maximum round-trip time
  // [3] Stddev round-trip time
  // The measurements are calculated from the given round-trip time.

  static float stats[4];

  if (n == 1)
  {
    stats[0] = rtt;
    stats[1] = rtt;
    stats[2] = rtt;
    stats[3] = 0;
    return stats;
  }

  stats[0] = stats[0] < rtt ? stats[0] : rtt;
  stats[1] = (stats[1] * (n - 1) + rtt) / n;
  stats[2] = stats[2] > rtt ? stats[2] : rtt;
  stats[3] = sqrt((stats[3] * (n - 1) + pow(rtt - stats[1], 2)) / n);
  return stats;
}