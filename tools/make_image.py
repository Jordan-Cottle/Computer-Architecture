import sys

from PIL import Image


def load_data(data_file_name):
    """ Load file and return list of integer values inside. """

    with open(data_file_name, "r") as data_file:
        return [int(line) for line in data_file.readlines()]


def create_image(data, format="RGB"):

    # Compute dimensions for square image
    width = height = int(len(data) ** (1 / 2))

    # Create blank image
    image = Image.new(format, (width, height))

    image.putdata(data)

    return image


def main(data_source, output_path):
    data = load_data(data_source)

    maximum = max(data)
    while maximum > 0xFF:
        data = [datum // 2 for datum in data]
        maximum = max(data)

    rgb_data = [(datum, datum, datum) for datum in data]

    img = create_image(rgb_data)
    img.save(output_path)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
