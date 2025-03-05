import cv2
import numpy as np

def show_circles_on_img(pathToImage):
    img= cv2.imread(pathToImage,cv2.IMREAD_COLOR)
    output = img.copy()
    grayimg = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    gimgblur = cv2.GaussianBlur(grayimg,(3,3), 0)
    rect_x= 2*177
    rect_y=2*101
    rect_w=(2*544 -2*177)
    rect_h=2*495-2*101
    mask=np.zeros_like(grayimg)
    cv2.rectangle(mask,(rect_x, rect_y), (rect_x + rect_w, rect_y + rect_h), (255), thickness=-1)
    masked_img= cv2.bitwise_and(gimgblur,gimgblur,mask=mask)

    detected_circles = cv2.HoughCircles(masked_img,cv2.HOUGH_GRADIENT,dp =3,minDist=20, param1=60, param2=85, minRadius=20, maxRadius=25)
    if detected_circles is not None: 

        detected_circles = np.round(detected_circles[0,:]).astype("int") 
        for (x,y,r) in detected_circles:
            cv2.circle(output, (x,y), r, (0,255,0),4)
            cv2.rectangle(output,(x-5,y-5),(x+5,y+5),(0,255,255),-1)
        height,width= img.shape[:2]
        scale = 0.5
        smol_img = cv2.resize(output, (int(width*scale), int(height*scale)))    
        cv2.imshow("output", smol_img)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    else:
        print("no circles detected")

def show_coordinates(pathToImage):
    img= cv2.imread(pathToImage,cv2.IMREAD_COLOR)
    output = img.copy()
    grayimg = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    gimgblur = cv2.GaussianBlur(grayimg,(3,3), 0)
    rect_x= 2*177
    rect_y=2*101
    rect_w=(2*544 -2*177)
    rect_h=2*495-2*101
    mask=np.zeros_like(grayimg)
    cv2.rectangle(mask,(rect_x, rect_y), (rect_x + rect_w, rect_y + rect_h), (255), thickness=-1)
    masked_img= cv2.bitwise_and(gimgblur,gimgblur,mask=mask)

    detected_circles = cv2.HoughCircles(masked_img,cv2.HOUGH_GRADIENT,dp =3,minDist=20, param1=60, param2=85, minRadius=20, maxRadius=25)
    if detected_circles is not None: 
        return detected_circles
    else:
        print("no circles detected")
