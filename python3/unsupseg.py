import subprocess
from PIL import Image
import numpy as np
import os

class Unsupseg:
    def __init__(self, command="./bin/unsupseg"):
        self.command = command

    def convert_image_to_ppm(self, image_path):
        # Check if the image is a PNG or JPG and convert it to PPM
        if image_path.endswith(".png") or image_path.endswith(".jpg") or image_path.endswith(".jpeg"):
            img = Image.open(image_path)
            ppm_image_path = os.path.splitext(image_path)[0] + ".ppm"
            img.save(ppm_image_path)
            return ppm_image_path
        return image_path

    def run(self, type, image, method, k, pol, SPsize):
        # Convert the image to PPM if it's a PNG or JPG
        image = self.convert_image_to_ppm(image)
        result = subprocess.run([self.command, type, image, method, k, pol, SPsize], capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error running subprocess: {result.stderr}")
        else:
            print(f"Subprocess output: {result.stdout}")

    def load_image(self, image_path):
        with Image.open(image_path) as img:
            print(f"Image mode: {img.mode}, max pixel value: {img.getextrema()}")
            img = img.convert('I')
            img_array = np.array(img)
        return img_array