# Detecting licence plates in car images without external libraries
PC  
Compile with: gcc main.c -o main  
Run with: main FILENAME.ppm    

RaspberryPI:  
Compile with:  sudo gcc main.c -o main -lm  
Run with: ./main FILENAME.ppm  
  
# Introduction

The purpose of this project is to develop software that allows the detection of
European single row license plate without using any external libraries, only pure image procesing.
The following requirements have been imposed on the project
the following requirements have been imposed: The input and
input and output formats for the programs should be PPM or PGM images.
The code should be in C. Without using any external libraries. Demonstration of running
program on a Raspberry PI 3 board during the class.

# Software used

To create the project Visual Studio Code editor version 1.68 was used
and gcc version 9.2.0 compiler (GCC). To preview the images in the format
IrfanViev was used to preview images in ppm format, while to preview binary values of files
binary file values Notepad++ with Hexeditor plugin was used.

# Algorithms used

The following algorithms were used for array detection:

1. converting the image to grayscale by extracting the
    1. convert the image to grayscale by extracting the average of the R G B components,

2. normalize the histogram according to algorithm 5.1 from the class,

Binarization of the image through the Otsu method, and through a fixed
    threshold value,

4. image convolution,

5. edge detection using Sobel operator,

Image modification by dilation,

Blob detection algorythm,

# Method of operation

The input parameter of the program is an image in ppm format with dimensions
Such an image can be obtained on the Internet, resized to a decent
Internet, reduced to a decent size, and converted to ppm
ppm format. It is important to remove any comments in the ppm file
ppm file, which some online converters add to the header of the photo.

![in](https://user-images.githubusercontent.com/69490354/173242459-ddd5eeda-d723-40f4-9bc4-f9defa74b182.jpg)

The next step is to create a grayscale copy of this photo.
This is achieved by extracting the average value of the R G B
components of one pixel. This image is then saved
under the name grayscaled.ppm.

![gray](https://user-images.githubusercontent.com/69490354/173242479-275c5eeb-373e-4749-bec1-9eed90117be6.jpg)


Once this is done, we align the histogram, and
then binarize our image. At first, we used the
Otsu algorithm for this purpose at first, but during testing it turned out that the rigid value
value showed better results. The image after binarization is saved as
tresholded.pgm

![bin](https://user-images.githubusercontent.com/69490354/173242488-cdf81ed2-e1bb-45fa-81df-dae797e899ce.jpg)


After binarization, we move on to edge detection using
using the sobel operator. The resulting image is saved as edge.pgm.

![sobel](https://user-images.githubusercontent.com/69490354/173242495-bb793f04-8a3d-4f2c-8e64-a363c20cdb21.jpg)

Next, we perform dilation of the image. We repeat this step several times
so that the detected edges form a cluster of white pixels. In our
tests the appropriate number of repetitions is at least 6. The image after
dilation is saved as dialeted.pgm.

![dial](https://user-images.githubusercontent.com/69490354/173242505-7d248e5a-738c-4d44-8907-f34ad00d08e9.jpg)

The last stage is the detection of the so-called blobs, and cutting
the area of interest. This stage consists in calculating the distance
of white pixels from the previously found clusters. If their
distance is smaller than the given treshold (94 pixels), the pixel
is added to the previously found blob. After examining
the whole area, we are looking for such a blob, which by its proportions
resembles a license plate (4.5 lengths to 1 height) and cuts
this area. This stage is the most sensitive to interference, so its
efficiency is exposed to oversights resulting from previous
steps. The localized array is saved as crop.pgm.

![crop](https://user-images.githubusercontent.com/69490354/173242518-ec9b1207-42cd-4aa5-a761-292399dc84a3.jpg)

# Ideas for development

The change that needs forfeiting is edge detection. The operator
sobel operator works well, but the question arises whether using de
canny algorithm for this purpose would not be a better solution. Due to the good
results, the first version of the project remains with the Sabel operator.
Another area for improvement is blob detection. A potentially better
solution would be to detect closed areas and then
filtering based on size, position, aspect ratio. Due to
complexity of the algorithm for detecting such areas (Connected Component
Labeling), it was left to blob detection.





