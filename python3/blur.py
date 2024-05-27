#Import required Image library
from PIL import Image, ImageFilter

#Open existing image
OriImage = Image.open('Tiff/getIT_27.ppm')
OriImage.show()

blurImage = OriImage.filter(ImageFilter.BoxBlur(10))
blurImage.show()
#Save blurImage
blurImage.save('simBlurImage.ppm')