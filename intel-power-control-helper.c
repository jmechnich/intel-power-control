#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

const char* drmbasepath = "/sys/class/drm";
const char* cpubasepath = "/sys/devices/system/cpu";
const char* backlightpath = "/sys/class/backlight/intel_backlight";
const size_t bufsize = 10;

int toggleCPU( const char* cpu)
{
#ifdef DEBUG
  printf("call toggleCPU(%s)\n", cpu);
#endif
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
#ifdef DEBUG
  printf("call setMHz(%s,%s,%s)\n", gpu, fn, val);
#endif
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

int setBrightness( const char* val)
{
  const char* fn = "brightness";
  size_t buflen = strlen(backlightpath) + strlen(fn) + 2;
  char* buf = calloc( sizeof(char), buflen);
  snprintf( buf, buflen, "%s/%s", backlightpath, fn);
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

void checkPath(const char* arg, const char* val)
{
  if(strchr(val,'.')!=0) goto ABORT;
  if(strchr(val,'/')!=0) goto ABORT;

  return;

  ABORT:
  printf("invalid value for argument '%s': %s\n", arg, val);
  abort();
}

void checkNumber(const char* arg, const char* val)
{
  char* endptr = NULL;

  /* check first if value starts with digit */
  if(!isdigit(*val)) goto ABORT;

  /* try conversion and check errno */
  errno = 0;
  strtol(val,&endptr,10);
  if(errno != 0) goto ABORT;

  /* check if conversion ended prematurely */
  if(strlen(val)!=(endptr-val)) goto ABORT;

  return;

  ABORT:
  printf("invalid value for argument '%s': %s\n", arg, val);
  abort();
}

void help()
{
  printf("usage: intel-power-manager-helper options\n");
  printf("  -h  --help          print this text and exit\n");
  printf("  -c  --cpu N         toggle CPU state for CPU N\n");
  printf("  -g  --gpu N         select GPU N\n");
  printf("  -l  --min N         set minimum GPU clock to N (requires -g)\n");
  printf("  -u  --max N         set maximum GPU clock to N (requires -g)\n");
  printf("  -s  --bst N         set boost GPU clock to N (requires -g)\n");
  printf("  -b  --brightness N  set brightness to N\n");
}

int main( int argc, char** argv)
{
  uid_t euid = geteuid();
  if(euid != 0) {
    printf("intel-power-control-helper: insufficient privileges\n");
    return 1;
  }

  const char* minfn = "gt_min_freq_mhz";
  const char* maxfn = "gt_max_freq_mhz";
  const char* bstfn = "gt_boost_freq_mhz";

  char cpu[bufsize+1], gpu[bufsize+1], min[bufsize+1], max[bufsize+1],
      bst[bufsize+1], bn[bufsize+1];
  memset( cpu, 0, bufsize+1);
  memset( gpu, 0, bufsize+1);
  memset( min, 0, bufsize+1);
  memset( max, 0, bufsize+1);
  memset( bst, 0, bufsize+1);
  memset( bn,  0, bufsize+1);

  int setFreq = 0;

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
            {"bst", required_argument, 0, 's'},
            {"brightness", required_argument, 0, 'b'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
    /* getopt_long stores the option index here. */
    int option_index = 0;
    c = getopt_long (argc, argv, "c:g:l:u:s:b:h",
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
      checkPath("cpu",cpu);
      if( toggleCPU( cpu) != 0) abort();
      break;
    case 'g':
#ifdef DEBUG
      printf("gpu: %s\n", optarg);
#endif
      strncpy(gpu, optarg, bufsize);
      checkPath("gpu",gpu);
      break;
    case 'l':
#ifdef DEBUG
      printf("minimum: %s\n", optarg);
#endif
      strncpy(min, optarg, bufsize);
      checkNumber("min", min);
      setFreq = 1;
      break;
    case 'u':
#ifdef DEBUG
      printf("maximum: %s\n", optarg);
#endif
      strncpy(max, optarg, bufsize);
      checkNumber("max", max);
      setFreq = 1;
      break;
    case 's':
#ifdef DEBUG
      printf("boost: %s\n", optarg);
#endif
      strncpy(bst, optarg, bufsize);
      checkNumber("bst", bst);
      setFreq = 1;
      break;
    case 'b':
#ifdef DEBUG
      printf("set brightness: %s\n", optarg);
#endif
      strncpy(bn, optarg, bufsize);
      checkNumber("brightness", bn);
      if( setBrightness(bn) != 0) abort();
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

  if(setFreq)
  {
    if( !*gpu )
    {
      printf( "Required argument missing: -g/--gpu\n");
      abort();
    }
    if( *min && setMHz( gpu, minfn, min) != 0) abort();
    if( *max && setMHz( gpu, maxfn, max) != 0) abort();
    if( *bst && setMHz( gpu, bstfn, bst) != 0) abort();
    exit(0);
  }

  return 0;
}
