from PIL import Image
import logging
import csv
import sys
import os

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def hex_to_rgb(hex_color):
    hex_color = hex_color.lstrip('#')
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

def color_distance(c1, c2):
    return sum((a-b)**2 for a, b in zip(c1, c2)) ** 0.5

def closest_color(rgb_color, color_palette):
    distances = [color_distance(rgb_color, hex_to_rgb(color)) for color in color_palette]
    return distances.index(min(distances))

def process_image(image_path, color_palette):
    logging.info(f'Processing image: {image_path}')
    
    img = Image.open(image_path)
    img = img.convert('RGB')  
    width, height = img.size
    logging.info(f'Image size: {width}x{height}')
    
    pixels = list(img.getdata())
    logging.info(f'Number of pixels: {len(pixels)}')
    
    result = []
    i = 0
    
    for pixel in pixels:
        closest_index = closest_color(pixel, color_palette)
        result.append(f"0x{closest_index:02X}")
        i = i + 1
        if i % 1000 == 0:  
            logging.info(f'Processed {i}/{len(pixels)} pixels')
            
    output_csv = f"{os.path.splitext(image_path)[0]}.csv"
    
    with open(output_csv, 'w', newline='') as csvfile:
        for i in range(0, len(result), 16):
            row = ','.join(result[i:i+16]) + ','
            csvfile.write(row + '\n')

    
    print(f"CSV file saved as {output_csv}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <image_path>")
        sys.exit(1)
    
    image_path = sys.argv[1]
    
    # Hardcoded Atari color palette
    color_palette = [
        "#000000", "#0F0F0F", "#1B1B1B", "#272727", "#333333", "#414141", "#4F4F4F", "#5E5E5E",
        "#686868", "#787878", "#898989", "#9A9A9A", "#ABABAB", "#BFBFBF", "#D3D3D3", "#EAEAEA",
        "#001600", "#0F2100", "#1A2D00", "#273900", "#334500", "#405300", "#4F6100", "#5D7000",
        "#687A00", "#778A17", "#899B29", "#9AAC3B", "#ABBD4C", "#BED160", "#D2E574", "#E9FC8B",
        "#1C0000", "#271300", "#331F00", "#3F2B00", "#4B3700", "#594500", "#675300", "#756100",
        "#806C12", "#8F7C22", "#A18D34", "#B29E45", "#C3AF56", "#D6C36A", "#EAD77E", "#FFEE96",
        "#2F0000", "#3A0000", "#460F00", "#521C00", "#5E2800", "#6C3600", "#7A4416", "#885224",
        "#925D2F", "#A26D3F", "#B37E50", "#C48F62", "#D6A073", "#E9B487", "#FDC89B", "#FFDFB2",
        "#390000", "#440000", "#50000A", "#5C0F17", "#681B23", "#752931", "#84373F", "#92464E",
        "#9C5058", "#AC6068", "#BD7179", "#CE838A", "#DF949C", "#F2A7AF", "#FFBBC3", "#FFD2DA",
        "#370020", "#43002C", "#4E0037", "#5A0044", "#661350", "#74215D", "#82306C", "#903E7A",
        "#9B4984", "#AA5994", "#BC6AA5", "#CD7BB6", "#DE8CC7", "#F1A0DB", "#FFB4EF", "#FFCBFF",
        "#2B0047", "#360052", "#42005E", "#4E006A", "#5A1276", "#672083", "#762F92", "#843DA0",
        "#8E48AA", "#9E58BA", "#AF69CB", "#C07ADC", "#D18CED", "#E59FFF", "#F9B3FF", "#FFCAFF",
        "#16005F", "#21006A", "#2D0076", "#390C82", "#45198D", "#53279B", "#6135A9", "#6F44B7",
        "#7A4EC2", "#8A5ED1", "#9B6FE2", "#AC81F3", "#BD92FF", "#D0A5FF", "#E4B9FF", "#FBD0FF",
        "#000063", "#00006F", "#140C7A", "#201886", "#2C2592", "#3A329F", "#4841AE", "#574FBC",
        "#615AC6", "#716AD6", "#827BE7", "#948CF8", "#A59DFF", "#B8B1FF", "#CCC5FF", "#E3DCFF",
        "#000054", "#000F5F", "#001B6A", "#002776", "#153382", "#234190", "#31509E", "#405EAC",
        "#4A68B6", "#5A78C6", "#6B89D7", "#7D9BE8", "#8EACF9", "#A1BFFF", "#B5D3FF", "#CCEAFF",
        "#001332", "#001E3E", "#002A49", "#003655", "#004261", "#12506F", "#205E7D", "#2F6D8B",
        "#397796", "#4987A6", "#5B98B7", "#6CA9C8", "#7DBAD9", "#91CEEC", "#A5E2FF", "#BCF9FF",
        "#001F00", "#002A12", "#00351E", "#00422A", "#004E36", "#0B5B44", "#196A53", "#287861",
        "#33826B", "#43927B", "#54A38C", "#65B49E", "#77C6AF", "#8AD9C2", "#9EEDD6", "#B5FFED",
        "#002400", "#003000", "#003B00", "#004700", "#00530A", "#106118", "#1E6F27", "#2D7E35",
        "#378840", "#479850", "#59A961", "#6ABA72", "#7BCB84", "#8FDE97", "#A3F2AB", "#BAFFC2",
        "#002300", "#002F00", "#003A00", "#004600", "#115200", "#1F6000", "#2E6E00", "#3C7C12",
        "#47871C", "#57972D", "#68A83E", "#79B94F", "#8ACA61", "#9EDD74", "#B2F189", "#C9FFA0",
        "#001B00", "#002700", "#0F3200", "#1C3E00", "#284A00", "#365800", "#446600", "#527500",
        "#5D7F00", "#6D8F19", "#7EA02B", "#8FB13D", "#A0C24E", "#B4D662", "#C8EA76", "#DFFF8D",
        "#110E00", "#1D1A00", "#292500", "#353100", "#413D00", "#4F4B00", "#5D5A00", "#6B6800",
        "#76720B", "#85821B", "#97932D", "#A8A43E", "#B9B650", "#CCC963", "#E0DD77", "#F7F48F"
    ]
    
    process_image(image_path, color_palette)
