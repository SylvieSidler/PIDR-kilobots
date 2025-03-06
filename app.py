import cv2
import threading
import numpy as np
from flask import Flask, Response
from functionsImageDetection import *
import pickle

app = Flask(__name__)
camera = None  # Caméra initialement désactivée
lock = threading.Lock()  # Empêche deux requêtes simultanées

@app.route('/start', methods=['GET'])
def start_camera():
    global camera
    with lock:
        if camera is None:
            camera = cv2.VideoCapture(2)  # Ouvre la caméra par défaut
            if not camera.isOpened():
                camera = None
                return "Failed to access the camera", 500
    return "Camera started", 200

@app.route('/stop', methods=['GET'])
def stop_camera():
    global camera
    with lock:
        if camera is not None:
            camera.release()  # Libère les ressources associées à la caméra
            camera = None
    return "Camera stopped", 200

@app.route('/capture_image', methods=['GET'])
def capture_image():
    global camera
    with lock:
        if camera is None:
            return "Camera is not started", 400

        success, frame = camera.read()
        if not success:
            return "Failed to capture image", 500

        serialized_as_json = json.dumps(pickle.dumps(a).decode('utf-8'))

        return serialized_as_json

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=5001) # Ne faites pas gaffe au port, le port 5000 était déjà utilisé lorsque j'ai codé


# Comment ça fonctionne : 
# Après ouverture de l'environnement et installation des librairies (opencv et Flask)
# Il faut lancer à nouveau terminal
# Et exécuter la commande suivante : curl http://localhost:5001/start
# Permettant le démarrage de la caméra. Cette commande peut être effectué dans n'importe quel dossier, il est juste bon à noter que l'image que vous capturerez sera enregistrée là ou vous vous situez
# La commande suivante : curl http://localhost:5001/capture_image --output image.jpg
# Permet de capturer l'image
# Et pour finir la commande suivante : curl http://localhost:5001/stop
# Stoppe la caméra
