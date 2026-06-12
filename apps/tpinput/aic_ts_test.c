/****************************************************************************
 * vendor/artinchip/apps/tpinput/aic_ts_test.c
 *
 * Copyright (C) 2024 ArtInChip Technology Co., Ltd.
 *
 * Generic Touchscreen Test Application
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <nuttx/input/touchscreen.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_AIC_TS_TEST_DEVPATH
#  define CONFIG_AIC_TS_TEST_DEVPATH "/dev/input0"
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void print_touch_point(const struct touch_point_s *point)
{
  printf("  ID: %d  Pos: (%d, %d)\n", point->id, point->x, point->y);
  printf("  Size: %dx%d  Pressure: %d\n", point->w, point->h, point->pressure);
  printf("  Flags: 0x%02X", point->flags);

  if (point->flags & TOUCH_DOWN)           printf(" DOWN");
  if (point->flags & TOUCH_MOVE)           printf(" MOVE");
  if (point->flags & TOUCH_UP)             printf(" UP");
  if (point->flags & TOUCH_ID_VALID)       printf(" ID_VALID");
  if (point->flags & TOUCH_POS_VALID)      printf(" POS_VALID");
  if (point->flags & TOUCH_PRESSURE_VALID) printf(" PRE_VALID");
  if (point->flags & TOUCH_SIZE_VALID)     printf(" SIZE_VALID");
  if (point->flags & TOUCH_GESTURE_VALID)  printf(" GESTURE");
  printf("\n");

  if (point->flags & TOUCH_GESTURE_VALID)
    {
      printf("  Gesture: ");
      switch (point->gesture)
        {
          case TOUCH_SINGLE_CLICK: printf("Single Click\n"); break;
          case TOUCH_DOUBLE_CLICK: printf("Double Click\n"); break;
          case TOUCH_SLIDE_UP:     printf("Slide Up\n");     break;
          case TOUCH_SLIDE_DOWN:   printf("Slide Down\n");   break;
          case TOUCH_SLIDE_LEFT:   printf("Slide Left\n");   break;
          case TOUCH_SLIDE_RIGHT:  printf("Slide Right\n");  break;
          case TOUCH_PALM:         printf("Palm\n");         break;
          default:                 printf("Unknown (%d)\n", point->gesture); break;
        }
    }
  printf("  Timestamp: %llu us\n", (unsigned long long)point->timestamp);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  struct pollfd pfd;
  int fd;
  ssize_t nbytes;
  struct touch_sample_s *sample;
  int sample_count = 0;
  uint8_t max_points = 5;
  char *devpath = CONFIG_AIC_TS_TEST_DEVPATH;

  printf("\nArtInChip Touchscreen Test Application\n");
  printf("======================================\n");

  /* Parse command line arguments */
  if (argc > 1)
    {
      devpath = argv[1];
    }

  printf("Device: %s\n", devpath);
  printf("Press Ctrl+C to exit\n\n");

  /* Open the touchscreen device in non-blocking mode */
  fd = open(devpath, O_RDONLY | O_NONBLOCK);
  if (fd < 0)
    {
      printf("ERROR: Failed to open %s: %s\n", devpath, strerror(errno));
      return EXIT_FAILURE;
    }

  /* Try to get the actual maximum number of touch points supported by the driver */
  if (ioctl(fd, 0, (unsigned long)((uintptr_t)&max_points)) < 0)
    {
      /* If IOCTL is not supported, fallback to default 5 */
      max_points = 5;
    }
  max_points = 1;
  printf("Successfully opened touchscreen device (Max Points: %d).\n", max_points);
  printf("Waiting for touch events...\n\n");

  /* Allocate buffer for touch sample based on actual max points */
  sample = (struct touch_sample_s *)malloc(SIZEOF_TOUCH_SAMPLE_S(max_points));
  if (!sample)
    {
      printf("ERROR: Failed to allocate sample buffer\n");
      close(fd);
      return EXIT_FAILURE;
    }

  /* Prepare poll structure */
  pfd.fd     = fd;
  pfd.events = POLLIN;

  /* Response Ctrl+C */
  int running = 1;
  void sigint_handler(int signo) {
    running = 0;
  }
  signal(SIGINT, sigint_handler);

  while (running)
    {
      int ret = poll(&pfd, 1, -1);
      if (ret < 0)
        {
          if (errno == EINTR) {
            if (!running) break;
            continue;
          }
          printf("ERROR: poll() failed: %s\n", strerror(errno));
          break;
        }
      if (!(pfd.revents & POLLIN))
        {
          continue;
        }
      nbytes = read(fd, sample, SIZEOF_TOUCH_SAMPLE_S(max_points));
      if (nbytes < 0)
        {
          if (errno == EAGAIN || errno == EINTR) {
            if (!running) break;
            continue;
          }
          printf("ERROR: read() failed: %s\n", strerror(errno));
          break;
        }
      if (nbytes < sizeof(struct touch_sample_s))
        {
          continue;
        }
      printf("=== Sample %d (Points: %ld) ===\n", ++sample_count, (long)sample->npoints);
      if (sample->npoints > 0)
        {
          for (int i = 0; i < sample->npoints && i < max_points; i++)
            {
               print_touch_point(&sample->point[i]);
            }
        }
      printf("-------------------\n");
    }

  free(sample);
  close(fd);

  return EXIT_SUCCESS;
}
