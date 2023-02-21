# BMP-Editor

The objective of this project is to develop a console application that allows 
users to apply various filters to images, much like the filters found in widely-used image editing tools.

# Supported image format

The input and output graphic files should be in BMP format. 
BMP format supports quite a few variations, but this task will use the 
uncompressed 24-bit BMP format without a color table. The type of `DIB header` used is `BITMAPINFOHEADER`.

# Command-line arguments format

Command-line arguments must satisfy this template:

```{program name} {path to BMP input file} {path to output file} [-{filter1 name} [filter1 first param] [filter1 second param] ...] [-{filter2 name} [filter2 first param] [filter2 second param] ...] ...```

You can check all 10 available filters in the program's help message.

### Example

```./bmp_editor input.bmp output.bmp -gs -blur 0.777 -crystal 32```


