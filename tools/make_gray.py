import sys

from PIL import Image, ImageOps

from tools import create_image

def main():
    source = sys.argv[1]
    output_path = sys.argv[2]
    try:
        new_size = int(sys.argv[3])
    except IndexError:
        new_size = None

    img = Image.open(source)

    if new_size is not None:
        img = img.resize((new_size, new_size))

    grey = ImageOps.grayscale(img)

    pixels = grey.load()

    data = [pixels[y, x] for x in range(img.width) for y in range(img.height)]

    output = create_image(data)

    output.save(output_path)
    print(*data, sep="\n")

if __name__ == "__main__":
    main()
