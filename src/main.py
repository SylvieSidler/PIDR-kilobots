import cv2
import numpy as np
import requests
from functionsImageDetection import *
from app import *
import json



def getImage():
    r = requests.get("http://localhost:5001/start")
    r= requests.get("http://localhost:5001/capture_image")
    decodedjson = r.json()
    deserializedjson = pickle.loads(json.loads(decodedjson).encode('latin-1'))
    return deserializedjson
    #cv2.imshow("Deserialized Image", deserializedjson)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()

def detectCircles():
    img = getImage()
    return return_coordinates(img)


def showCircles():
    coordandimg= detectCircles()
    show_circles_on_img(coordandimg[0],coordandimg[1])

if __name__=="__main__":
    showCircles()
