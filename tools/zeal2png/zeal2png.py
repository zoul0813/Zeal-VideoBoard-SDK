#!/usr/bin/env python3

import math
from PIL import Image
import argparse
from pathlib import Path

tiles = []
tile_width = 16
tile_height = 16
tile_size = tile_width * tile_height
tiles_per_sheet = 16

def get_tilesheet_size(tiles):
  width = 1
  height = 1
  count = len(tiles)
  # max 16 tiles across
  if count > tiles_per_sheet:
    width = tiles_per_sheet
    height = math.ceil(len(tiles) / tiles_per_sheet)
  else:
    width = count
    height = 1

  print("width", width, "height", height)
  return (width * tile_width,height*tile_height)

def RGB565toRGB(rgb565, palette):
  # print("palette", rgb565)
  lo = palette[rgb565 * 2]
  hi = palette[rgb565 * 2 + 1]
  # print("  color", hi, lo)
  return (hi,lo)

def getPalette(paletteFile):
  pf = open(paletteFile, "rb")
  pb = pf.read()
  palette = []
  for b in pb:
    palette.append(b)
  return palette

def convert(tilesetFile, paletteFile):
  palette = getPalette(paletteFile)

  f = open(tilesetFile, mode="rb")
  tiles = []
  tile = f.read(tile_size)
  image_count = 0
  while tile:
    if len(tile) < tile_size:
      break

    pixels = bytearray()
    for pixel in tile:
      (hi,lo) = RGB565toRGB(pixel, palette)
      pixels.append(lo)
      pixels.append(hi)

    image = Image.frombytes(
      "RGB", # mode
      (tile_width,tile_height), # size
      pixels, # data
      "raw", # decoder name
      "BGR;16",
      0, 1
      )

    if image:
      tiles.append(image)
      image_count += 1
    tile = f.read(tile_size)
  f.close()

  tilesheet_size = get_tilesheet_size(tiles)
  spritesheet = Image.new(mode="RGB", size=tilesheet_size)

  sprites_x = int(tilesheet_size[0]/tile_width)
  sprites_y = int(tilesheet_size[1]/tile_height)
  for x in range(0,sprites_x):
    for y in range(0, sprites_y):
      index = (y*x) + x
      spritesheet.paste(tiles[index], (x * tile_width,y*tile_height))

  print("sprite count", image_count)
  return spritesheet

def main():
  parser = argparse.ArgumentParser("zeal2png")
  parser.add_argument("-t","--tileset", help="Zeal Tileset (ZTS)", required=True)
  parser.add_argument("-p", "--palette", help="Zeal Palette (ZTP)", required=True)
  parser.add_argument("-o", "--output", help="Output PNG Filename")
  args = parser.parse_args()

  output = convert(args.tileset, args.palette)

  outputFile = args.output
  if outputFile == None:
    outputFile = Path(args.tileset).with_suffix(".png")
  print("output", outputFile)
  output.save(outputFile, "PNG")

if __name__ == "__main__":
  main()