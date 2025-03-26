import cv2
import numpy as np
import requests
from functionsImageDetection import *
import json
import pickle

def getImage(mode):
    r = requests.get("http://localhost:5001/start")
    r = requests.get("http://localhost:5001/"+mode)
    r = requests.get("http://localhost:5001/capture_image")
    decodedjson = r.json()
    deserializedjson = pickle.loads(json.loads(decodedjson).encode('latin-1'))
    return deserializedjson
    
    # cv2.imshow("image", getImage())

    # while True:
    #     if cv2.waitKey(1) & 0xFF == ord('q'):
    #         break

    # cv2.destroyAllWindows()


def detectCircles(mode,dp,minDist, param1, param2, minRadius, maxRadius):
    img = getImage(mode)
    #img = cv2.imread("pidr-24-25-kilobots/Photos/photos_800x600/Photo_le_06-03-2025_à_17.31__3.jpg")
    return return_coordinates(img,dp,minDist, param1, param2, minRadius, maxRadius)


def showCircles(mode,dp,minDist, param1, param2, minRadius, maxRadius):
    coordandimg= detectCircles(mode,dp,minDist, param1, param2, minRadius, maxRadius)
    show_circles_on_img(coordandimg[0],coordandimg[1])

if __name__=="__main__":
    #mode = "mode1"
    #mode = "mode2"
    #mode = "mode3"
    #mode = "mode4"
    mode = "mode5"
    #cv2.imshow("image", getImage())
    if mode == "mode1":
        showCircles(mode,3.5,20,25,30,4,7) #AMELIORER
    elif mode == "mode2":
        showCircles(mode,3.25,15,70,70,12,16)
    elif mode == "mode3":
        showCircles(mode,3,30,50,55,8,15)
    elif mode == "mode4":
        showCircles(mode,3,20,60,85,20,25)
    elif mode == "mode5":
        showCircles(mode,3,50,80,85,30,40)
        
    cv2.destroyAllWindows()
