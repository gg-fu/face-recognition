# arm-video

#need package
  
opencv-3.2
  
#Note

Each camera has a width * height size allowed to operate, so you need to print the camera information through the code. This time I didn't notice that the camera's width and height format were set randomly, which led to the image scrambling display. I always thought it was yuyv turning RGB that made a mistake. The result was unexpected!!! Special record, with your encouragement.

#install

  qmake Camera.pro
  make
