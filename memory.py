#/usr/bin/python

from PIL import Image

#constants
screenH = 128
screenV =   8
path    = "image.jpg"

#Read image pixels, resize them and convert to black and white
img = Image.open(path)
img = img.resize((128,64))
img = img.convert('1', colors=1);
img.show()
pix = img.load()
f = open("src/image.h", "w")


#Write C file with memory
f.write("const uint8_t mem["+str(screenV)+"]["+str(screenH)+"] PROGMEM = {\n")
for y in range(0,screenV):
    f.write("{")
    for x in range(0,screenH):
        value = 0x00
        data  = 0x00
        for w in range(0,8):
            value = pix[x,(y*8)+w]
            value = value // 255   #convert to 1 and 0
            data  = data | (value<<8)
            data  = data >> 1
        if x == screenH-1:
            f.write("},\n")
        else:
            f.write(str(data))
            f.write(",")

f.write("};")

f.close()
