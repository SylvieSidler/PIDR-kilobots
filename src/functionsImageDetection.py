import cv2
import numpy as np


def show_circles_on_img(coord, image_array):
    output = image_array.copy()
    if coord is not None: 

        coord = np.round(coord[0,:]).astype("int") 
        for (x,y,r) in coord:
            cv2.circle(output, (x,y), r, (0,255,0),4)
            #cv2.rectangle(output,(x-5,y-5),(x+5,y+5),(0,255,255),-1)
        height,width= image_array.shape[:2]
        scale = 0.5
        smol_img = cv2.resize(output, (int(width*scale), int(height*scale)))    
        #cv2.imshow("output", smol_img)
        cv2.imshow("output", output)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    else:
        print("no circles detected")
        

def return_coordinates(image_array, dp1,minDist1, param11, param21, minRadius1, maxRadius1):
    output = image_array.copy()
    grayimg = cv2.cvtColor(image_array,cv2.COLOR_BGR2GRAY)
    gimgblur = cv2.GaussianBlur(grayimg,(7,7), 0)
    #rect_x= 2*300
    #rect_y=2*170
    #rect_w=(2*920 -2*300)
    #rect_h=2*820-2*170
    #mask=np.zeros_like(grayimg)
    #cv2.rectangle(mask,(rect_x, rect_y), (rect_x + rect_w, rect_y + rect_h), (255), thickness=-1)
    #masked_img= cv2.bitwise_and(gimgblur,gimgblur,mask=mask)

    detected_circles = cv2.HoughCircles(gimgblur,cv2.HOUGH_GRADIENT,dp=dp1,minDist=minDist1, param1=param11, param2=param21, minRadius=minRadius1, maxRadius=maxRadius1)
    if detected_circles is not None: 
        return [detected_circles,image_array]
    else:
        print("no circles detected")
