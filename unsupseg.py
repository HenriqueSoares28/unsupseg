import subprocess
from PIL import Image
import numpy as np

# Definir o comando e os parâmetros
command = "./bin/unsupseg"
type = "1"  # 0: grayscale, 1: color
image = "Tiff/getIT_27.ppm"
method = "1"  # 0: Divisive clustering by OIFT, 1: Divisive clustering by MST, 2: Divisive clustering by OIFT with DCCsize
k = "2000"  # The desired number of regions
pol = "1.0"  # Boundary polarity in [-1.0, 1.0]
SPsize = "30"  # Superpixel size in pixels


subprocess.run([command, type, image, method, k, pol, SPsize])


imagem_gerada = "out/label.pgm"
img = Image.open(imagem_gerada)

img_array = np.array(img)

print(img_array)

