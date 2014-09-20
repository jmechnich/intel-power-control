#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

const char* drmbasepath = "/sys/class/drm";
const char* cpubasepath = "/sys/devices/system/cpu";

int toggleCPU( const char* cpu)
{
  const char* fn = "online";
  
  size_t buflen = strlen(cpubasepath) + strlen(cpu) + strlen(fn) + 3;
  char* buf = calloc( sizeof(char), buflen);
  snprintf( buf, buflen, "%s/%s/%s", cpubasepath, cpu, fn);
  FILE* f1 = fopen( buf, "r");
  if( !f1)
  {
    printf( "Could not open '%s'\n", buf);
    free( buf);
    return 1;
  }
  char state = 0;
  size_t read = fread( &state, sizeof(char), 1, f1);
  fclose(f1);

  if( read == 0)
  {
    printf( "Could not read from '%s'\n", buf);
    free( buf);
    return 1;
  }

  char newState = (char)(((int)'0')+(((int)(state-'0')+1)%2));
  FILE* f2 = fopen( buf, "w");
  if( !f2)
  {
    printf( "Could not open '%s'\n", buf);
    free( buf);
    return 1;
  }
  size_t written = fwrite( &newState, sizeof(char), 1, f2);

  fclose( f2);
  free( buf);

  if( written != 1)
  {
    printf( "Error writing byte to file, got %lu\n", written);
    return 1;
  }
    
  return 0;
}

int setMHz( const char* gpu, const char* fn, const char* val)
{
  size_t buflen = strlen(drmbasepath) + strlen(gpu) + strlen(fn) + 3;
  char* buf = calloc( sizeof(char), buflen);
  snprintf( buf, buflen, "%s/%s/%s", drmbasepath, gpu, fn);
  FILE* f = fopen( buf, "w");
  if( !f)
  {
    printf( "Could not open '%s'\n", buf);
    free( buf);
    return 1;
  }
  
  size_t written = fwrite( val, sizeof(char), strlen(val), f);

  fclose( f);
  free( buf);

  if( written != strlen(val))
  {
    printf( "Error writing %lu bytes to file, got %lu\n", strlen(val), written);
    return 1;
  }
    
  return 0;
}

void help()
{
  printf("usage: intel-power-manager-helper options\n");
  printf("  -h  --help  print this text and exit\n");
  printf("  -c  --cpu   toggle CPU state\n");
  printf("  -g  --gpu   select GPU\n");
  printf("  -l  --min   set minimum GPU clock (requires -g)\n");
  printf("  -u  --max   set maximum GPU clock (requires -g)\n");
}
int main( int argc, char** argv)
{
  const char* minfn = "gt_min_freq_mhz";
  const char* maxfn = "gt_max_freq_mhz";
  
  const size_t bufsize = 10;
  char cpu[bufsize+1], gpu[bufsize+1], min[bufsize+1], max[bufsize+1];
  memset( cpu, 0, bufsize+1);
  memset( gpu, 0, bufsize+1);
  memset( min, 0, bufsize+1);
  memset( max, 0, bufsize+1);
  
  int c;
  while (1)
  {
    
    static struct option long_options[] =
        {
            /* These options set a flag. */
            {"cpu", required_argument, 0, 'c'},
            {"gpu", required_argument, 0, 'g'},
            {"min", required_argument, 0, 'l'},
            {"max", required_argument, 0, 'u'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
    /* getopt_long stores the option index here. */
    int option_index = 0;
    c = getopt_long (argc, argv, "l:u:g:c:h",
                     long_options, &option_index);
    
    /* Detect the end of the options. */
    if (c == -1)
        break;
    
    switch (c)
    {
    case 'c':
#ifdef DEBUG
      printf("cpu: %s\n", optarg);
#endif
      strncpy(cpu, optarg, bufsize);
      if( toggleCPU( cpu) != 0) abort();
      break;
    case 'g':
#ifdef DEBUG
      printf("gpu: %s\n", optarg);
#endif
      strncpy(gpu, optarg, bufsize);
      break;
    case 'l':
#ifdef DEBUG
      printf("minimum: %s\n", optarg);
#endif
      if( !*gpu )
      {
        printf( "Required argument missing: -g/--gpu\n");
        abort();
      }
      strncpy(min, optarg, bufsize);
      if( setMHz( gpu, minfn, min) != 0) abort();
      break;
    case 'u':
#ifdef DEBUG
      printf("maximum: %s\n", optarg);
#endif
      if( !*gpu )
      {
        printf( "Required argument missing: -g/--gpu\n");
        abort();
      }
      strncpy(max, optarg, bufsize);
      if( setMHz( gpu, maxfn, max) != 0) abort();
      break;
    case 'h':
      help();
      exit(0);
      break;
    case '?':
      abort();
      break;
    default:
#ifdef DEBUG
      printf("Unknown command line flag '%c'\n", c);
#endif
      abort();
    }
  }

  return 0;
}
