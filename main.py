import cv2
import numpy as np
import requests
from functionsImageDetection import *
from app import *
import json


def getImage():
    r = requests.get("http://localhost:5001/start")
    r= requests.get("http://localhost:5001/detection_cercle")
    decodedjson = r.json()
    deserializedjson = pickle.loads(json.loads(decodedjson).encode('latin-1'))
    cv2.imshow("Deserialized Image", deserializedjson)



