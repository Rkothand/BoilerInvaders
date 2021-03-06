#!/usr/bin/env python3

width = 2
height = 16
name = 'badBullet'

# Initialize to zero
arr = [ [0 for x in range(0,height)] for y in range(0,width) ]

border = 0
black = 0x0000
white = 0xffff
green = 15<<5 # 0x0710
blue = 0x0000FF
red= 0xFF0000

#?
cyan = 0xFFFF00

# Make a checkerboard with green boundaries
for x in range(0, width):
  for y in range(0, height):
    if x < border or x >= width-border:
      arr[x][y] = white
      continue
    if y < border or y >= height-border:
      arr[x][y] = white
      continue
    if (x // 20) % 2 == 0:
      if (y // 20) % 2 == 0:
        arr[x][y] = green
      else:
        arr[x][y] = green
    else:
      if (y // 20) % 2 == 0:
        arr[x][y] = green
      else:
        arr[x][y] = green

out = open('background.c','w')

# Print out the header
out.write("const struct {\n")
out.write("  unsigned int width;\n")
out.write("  unsigned int height;\n")
out.write("  unsigned int bytes_per_pixel;\n")
out.write("  unsigned char pixel_data[%d * %d * 2 + 1];\n" % (width,height))
out.write("} %s = {\n" % name)
out.write("  %d, %d, 2,\n" % (width,height))

# Print out one row at a time from top to bottom
for y in range(0, height):
  out.write('  "')
  for x in range(0, width):
    # Print each 16-bit value in little-endian format
    out.write('\\%03o\\%03o' % (arr[x][y]&0xff, arr[x][y]>>8))
  out.write('"\n')

out.write("};\n")

