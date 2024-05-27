import subprocess
from PIL import Image
import numpy as np

class Unsupseg:
    def __init__(self, command="./bin/unsupseg"):
        self.command = command

    def run(self, type, image, method, k, pol, SPsize):
        subprocess.run([self.command, type, image, method, k, pol, SPsize])

    def load_image(self, image_path):
        img = Image.open(image_path)
        img_array = np.array(img)
        return img_array
