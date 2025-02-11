# Photoshop2

This is the second version of a prototype of an image editor, wich was nicknamed "Photoshôp 2.0", developed in C++ as a college assignment. Every tool in the editor was programmed from sratch using the corresponding math/physics formulas for it.

## DESIGN

The interface was created with Qt Creator, a C++ based IDE where you can drag and drop components to form a full layout. It also has a C++ compiler and debugger.

The goal was to create an editor that mimics the style of a "dark mode Photoshop". It has a center frame for the original image, with all the editor tools around it, and a second window that pops up to show the edited image.

## TOOLS

- **V Mirroring**: mirrors the image vertically
- **H Mirroring**: mirrors the image horizontally
- **Zoom Out**: zoom out the image (makes it smaller) and changes its scale based on the 2 inputs to the right of the button.
- **Zoom In**: zoom the image in a scale of 2x.
- **Rotate Right** 
- **Rotate Left**
- **Gray Scale**: changes the Red, Green and Blue values of each pixel to its *luminance* value (L = 0.299\*R + 0.587\*G + 0.114\*B)
- **Negative**: changes the RGB value of each pixel to its opposite in the Hex scale (255 - currentValue)
- **Brightness**: changes the brightness based on the integer value of the input (scalar)
- **Contrast**: changes the contrast based on the float value of the input (vector)
- **Quantization**: apply GrayScale to the image and limit its colors to the number of colors informed on the input
- **Histogram**: apply GrayScale and shows the histogram of the image, i.e a chart of the number of pixels per tone
- **Equalization**: reuses the calculations of the Histogram operation to apply a histogram equalization, reducing strong differences in the tones of the image. It also shows the histograms of both original and edited images
- **Histogram Matching**: matches the tones of the current image with an image given by the user (makes the histogram of the original image the same as the new one)
- **Apply Convolution**: apply a convolution filter given by the user in the float inputs above
- **Reset**: copies the original image to the second window to undo the changes
