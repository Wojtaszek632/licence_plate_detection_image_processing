# Detecting licence plates in car images without external libraries
## PC  

Compile with: 
```
gcc main.c -o main  
```
Run with: 
```
main FILENAME.ppm    
```
## RaspberryPI:  

Compile with:  
```
sudo gcc main.c -o main -lm  
```
Run with: 
```
./main FILENAME.ppm  
```  
# Introduction

The purpose of this project is to develop software that allows the detection of
European single-row license plate without using any external libraries (only pure data processing).
The following requirements have been imposed on the project: 
- The input and output formats for the programs should be PPM or PGM images.
- The code should be in C without using any external libraries. 
- Running program on a Raspberry Pi 3 board.

# Software used

- GCC 9.2.0 to compile the project
- IrfanViev to preview images in PPM format.
- Notepad++ with the Hexeditor plugin to edit binary files.

# Algorithms used

The following algorithms were used:

1. Convert the image to grayscale by extracting the average of the R G B components,  
2. Histogram normalization
3. Binarization of the image with the Otsu method, and with a fixed threshold value,  
4. 3x3 image convolution  
5. Edge detection using Sobel operator,  
6. Repeated dilation,  
7. Blob detection  

# Method of operation

The input of the program is an image in ppm format. Such an image can be obtained on the Internet, reduced to a 512x512 size, and converted to ppm format. It is important to remove any comments in the ppm header section, which some online converters add.

![in](https://user-images.githubusercontent.com/69490354/173242459-ddd5eeda-d723-40f4-9bc4-f9defa74b182.jpg)

The first step is to create a grayscale copy of this photo.
This is achieved by extracting the average value of the R G B
components of one pixel. This image is then saved
under the name grayscaled.ppm.

![gray](https://user-images.githubusercontent.com/69490354/173242479-275c5eeb-373e-4749-bec1-9eed90117be6.jpg)


Once this is done, we normalize the histogram, and binarize our image. At first, we used the
Otsu algorithm, but during testing it turned out that the rigid treshold showed better results. The image after binarization is saved as
tresholded.pgm

![bin](https://user-images.githubusercontent.com/69490354/173242488-cdf81ed2-e1bb-45fa-81df-dae797e899ce.jpg)


After binarization, we move to edge detection using the Sobel operator (both vertical and horizontal edges). The resulting image is saved as edge.pgm.

![sobel](https://user-images.githubusercontent.com/69490354/173242495-bb793f04-8a3d-4f2c-8e64-a363c20cdb21.jpg)

Next, we perform dilation of the image. We repeat this step several times
so that the detected edges form a cluster of white pixels. In our
tests the number of repetitions turned out to be at least 6. The image after
dilation is saved as dialeted.pgm.

![dial](https://user-images.githubusercontent.com/69490354/173242505-7d248e5a-738c-4d44-8907-f34ad00d08e9.jpg)

The last stage is the detection of the so-called blobs, and cutting
the area of interest. This stage consists of calculating the distance
between white pixels and the previously found clusters. If their
distance is smaller than the given treshold (empirically set to 94 pixels), the pixel
is added to the previously found blob. After examining
the whole area, we are looking for such a blob, which by its proportions
resembles a license plate (4.5 lengths to 1 height). We crop this area and save the licence plate. This stage is the most sensitive to interference, so its
efficiency is exposed to oversights resulting from previous
steps. The localized array is saved as crop.pgm.

![crop](https://user-images.githubusercontent.com/69490354/173242518-ec9b1207-42cd-4aa5-a761-292399dc84a3.jpg)

# Ideas for development

The change that needs most work is the edge detection. The 
sobel  operator works well, but the question arises whether using de
canny algorithm for this purpose would not be a better solution. Due to the fairly good
results, the first version of the project remains with the Sabel operator.
Another area for improvement is blob detection. A potentially better
solution can be implemented to detect closed areas and then
filter it based on its size, position and aspect ratio. Due to
complexity of the algorithm for detecting such areas (Connected Component
Labeling), it was left to blob detection.





