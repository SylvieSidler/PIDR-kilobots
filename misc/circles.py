import sys, time, math
from  collections import deque

import numpy as np

import cv2 as cv

class FPS (object):
    def __init__(self,avarageof=50):
        self.frametimestamps = deque(maxlen=avarageof)
    def __call__(self):
        self.frametimestamps.append(time.time())
        if(len(self.frametimestamps) > 1):
            return len(self.frametimestamps)/ \
                (self.frametimestamps[-1]-self.frametimestamps[0])
        else:
            return 0.0

class TextWriter(object) : 
    def __init__(self):
        self._font                   = cv.FONT_HERSHEY_SIMPLEX
        self._fontScale              = 1
        self._fontColor              = (0,0,255)
        self._lineType               = 2
    def __call__(self, img, text, pos=(40,40)) :
        cv.putText(img, text, pos, 
                    self._font, 
                    self._fontScale,
                    self._fontColor,
                    self._lineType)

class CircleDrawer(object) : 
    def __init__(self):
        
        self._center_thickness = 1
        self._center_color = (0, 0, 255)
        
    def __call__(self, img, circles, color=(0,255,0), thickness=4, bbox=False) :
       
        circle = np.round(circles[0]).astype("int")
      
        x, y, r = circle
        cv.circle(output, (x, y), r, color, thickness)

        cv.drawMarker(  output, (x,y), color, thickness=thickness)
        
       
        if bbox :
            cv.rectangle(output, (x-r, y-r), (x+r, y+r),
                         self._center_color,
                         self._center_thickness)
        
        
def nothing(*arg): # used as a call back for GUIq
    pass
    
if __name__ == "__main__" : 
            
    print ("openCV version {}".format(cv.__version__))
    #print(cv.getBuildInformation())
    
    # Create a VideoCapture object
    cap = cv.VideoCapture(2)
    
    # Check if camera opened successfully
    if cap.isOpened() == False : 
        print ("Unable to read camera feed" )
        sys.exit(0)

    #width = cap.get(cv.CAP_PROP_FRAME_WIDTH)
    #height= cap.get(cv.CAP_PROP_FRAME_HEIGHT)

    cap.set(cv.CAP_PROP_FRAME_WIDTH, 800)
    cap.set(cv.CAP_PROP_FRAME_HEIGHT, 600)
    
    width = 800
    height = 600
    #
    # hough parameters 
    #
    # 
    diag = math.sqrt(width**2 + height**2) # image diagonal
    dist = int(.9 * diag) # distance between 2 hugh circles
    min_rad = int(width / 20)  # min and max of considered circles 
    max_rad = int(min_rad * 8 )
    dmax = 10 # min distance between 2 circles
    hough_param_1 = 70 # see https://docs.opencv.org/4.5.1/dd/d1a/group__imgproc__feature.html#ga47849c3be0d0406ad3ca45db65a25d2d
    hough_param_2 = 50

    #
    # create a window
    #
    #
    cv.namedWindow('frame')
    cv.createTrackbar('par1', 'frame', hough_param_1, 500, nothing)
    cv.createTrackbar('par2', 'frame', hough_param_2, 500, nothing)
    cv.createTrackbar('dmax', 'frame', dmax, 50, nothing)
    cv.createTrackbar('minr', 'frame', min_rad , int(width/2), nothing)
    cv.createTrackbar('maxr', 'frame', max_rad , int(width/2), nothing)

    #
    # Main loop
    #
    #
    old_circle = np.zeros((1,3))
    min_circle = np.zeros((1,3))
    max_circle = np.zeros((1,3))
    min_circle[0][0] = int(width/2)
    min_circle[0][1] = int(height/2)
    min_circle[0][2] = int(min_rad)
    max_circle[0][0] = int(width/2)
    max_circle[0][1] = int(height/2)
    max_circle[0][2] = int(max_rad)

    edges = None

    counter = 0
    work_freq = 1 # process every work_freq  frame

    fps = FPS()
    writer = TextWriter()
    drawer = CircleDrawer()
    
    kernel = np.ones((3,3),np.uint8) # used below in erosion/dilatation
    
    while True :
        counter += 1 

        # read a frame 
        ret, frame = cap.read()
        if ret != True:
            continue # if could not,  skip 
   
        output = frame.copy()

        #
        # Preprocessing 
        #
        #
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        gray = cv.GaussianBlur(gray, (5, 5), 0)
        gray = cv.medianBlur(gray,5)
        gray = cv.adaptiveThreshold(gray, 255,\
                                    cv.ADAPTIVE_THRESH_GAUSSIAN_C,\
                                    cv.THRESH_BINARY, 11, 3.5)
	
        
        gray = cv.erode(gray, kernel, iterations=1)
        gray = cv.dilate(gray, kernel, iterations=1)
        
        #
        # hough
        #
        if counter % work_freq == 0 :
            counter = 0
           
            # read GUI values
            hough_param_1 = cv.getTrackbarPos('par1', 'frame')
            hough_param_2 = cv.getTrackbarPos('par2', 'frame')
            dmax = cv.getTrackbarPos('dmax', 'frame')
            min_rad = cv.getTrackbarPos('minr', 'frame')
            max_rad = cv.getTrackbarPos('maxr', 'frame')
            hough_param_1 = max(hough_param_1, 1)
            hough_param_2 = max(hough_param_2, 1)


            # this is to draw the edges used by hough
            edges = cv.Canny(gray, threshold1=hough_param_1,
                             threshold2=hough_param_1/2.,
                             apertureSize=3,
                             L2gradient=True)
            # detect
            circles_det = cv.HoughCircles(gray, cv.HOUGH_GRADIENT,
                                          2,
                                          dist,
                                          param1=hough_param_1,
                                          param2=hough_param_2,
                                          minRadius=min_rad,
                                          maxRadius=max_rad )

            # compare to the previoussly detected circle 
            #if  circles_det is not None :
            #    circles_det = circles_det[0]
            #    diff = (circles_det - old_circle)**2
            #    n_diff = np.sum(diff)
            #    if n_diff > dmax**2 :
            #        old_circle = circles_det
            #else :
            #    old_circle = np.zeros((1,3))
            
        
        #
        # draw the detected circle and edges 
        #
        if circles_det is not None:
            for c in circles_det :
                drawer(output,  c)

        if edges is not None :
            output[edges != 0] = (0, 0, 255)
        
        #
        # draw max and min diameter 
        #
        min_circle[0][2] = int(min_rad)
        max_circle[0][2] = int(max_rad)
        drawer(output, min_circle, (255, 0, 255), 1, False)
        drawer(output, max_circle, (255, 0, 255), 1, False)
                 
        writer(output, "Fps={:06.2f}".format(fps()))
        cv.imshow('frame', output)
    
        # Press Q on keyboard to stop 
        if cv.waitKey(1) & 0xFF == ord('q'):
            break
        
    # When everything done, release the video capture and video write objects
    cap.release()
 
    # Closes all the frames
    cv.destroyAllWindows() 
