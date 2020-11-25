import sys

from PIL import Image

data_source = sys.argv[1]
output_path = sys.argv[2]

def load_data(data_file_name):
    """ Load file and return list of integer values inside. """

    with open(data_file_name, 'r') as data_file:
        return [int(line) for line in data_file.readlines()]

def create_image(data):

    # Compute dimensions for square image
    width = height = int(len(data) ** (1/2))

    # Create blank greyscale image
    image = Image.new('L', (width, height))

    image.putdata(data)

    return image

img = create_image(load_data(data_source))
img.save(output_path)