import sys, time, math
import requests
import pickle
import json
from  collections import deque

import numpy as np
import cv2 as cv

# Classe pour mesurer les FPS sur les dernières frames (moyenne glissante)
class FPS (object):
    def __init__(self, avarageof=50):
        self.frametimestamps = deque(maxlen=avarageof)

    def __call__(self):
        self.frametimestamps.append(time.time())
        if len(self.frametimestamps) > 1:
            return len(self.frametimestamps) / (self.frametimestamps[-1] - self.frametimestamps[0])
        else:
            return 0.0

# Classe utilitaire pour afficher du texte sur une image
class TextWriter(object):
    def __init__(self):
        self._font = cv.FONT_HERSHEY_SIMPLEX
        self._fontScale = 1
        self._fontColor = (0, 0, 255)
        self._lineType = 2

    def __call__(self, img, text, pos=(40, 40)):
        cv.putText(img, text, pos, self._font, self._fontScale, self._fontColor, self._lineType)

# Classe pour dessiner un cercle détecté sur une image
class CircleDrawer(object):
    def __init__(self):
        self._center_thickness = 1
        self._center_color = (0, 0, 255)

    def __call__(self, img, circles, color=(0, 255, 0), thickness=4, bbox=False):
        circle = np.round(circles[0]).astype("int")
        x, y, r = circle

        cv.circle(output, (x, y), r, color, thickness)
        cv.drawMarker(output, (x, y), color, thickness=thickness)

        if bbox:
            cv.rectangle(output, (x - r, y - r), (x + r, y + r), self._center_color, self._center_thickness)

# Callback vide pour les trackbars OpenCV
def nothing(*arg):
    pass

if __name__ == "__main__":
    print("openCV version {}".format(cv.__version__))

    # Configuration de la capture vidéo via serveur HTTP local
    r = requests.get("http://localhost:5001/start")
    r = requests.get("http://localhost:5001/set_resolution/mode3")
    r = requests.get("http://localhost:5001/set_camera/0")

    width = 800
    height = 600

    # Paramètres de la transformée de Hough pour la détection de cercles
    diag = math.sqrt(width**2 + height**2)
    dist = int(.9 * diag)
    min_rad = 38
    max_rad = 39
    dmax = 10
    hough_param_1 = 20
    hough_param_2 = 35

    # Création de la fenêtre et des sliders pour modifier les paramètres Hough
    cv.namedWindow('frame')
    cv.createTrackbar('par1', 'frame', hough_param_1, 500, nothing)
    cv.createTrackbar('par2', 'frame', hough_param_2, 500, nothing)
    cv.createTrackbar('dmax', 'frame', dmax, 50, nothing)
    cv.createTrackbar('minr', 'frame', min_rad, int(width / 2), nothing)
    cv.createTrackbar('maxr', 'frame', max_rad, int(width / 2), nothing)

    # Initialisation de variables pour la détection
    old_circle = np.zeros((1, 3))
    min_circle = np.zeros((1, 3))
    max_circle = np.zeros((1, 3))
    min_circle[0][:2] = int(width / 2), int(height / 2)
    min_circle[0][2] = min_rad
    max_circle[0][:2] = int(width / 2), int(height / 2)
    max_circle[0][2] = max_rad

    edges = None
    counter = 0
    work_freq = 1  # traiter une image sur n

    # Initialisation des outils : mesure de FPS, écriture texte, dessin de cercles
    fps = FPS()
    writer = TextWriter()
    drawer = CircleDrawer()
    kernel = np.ones((3, 3), np.uint8)

    while True:
        counter += 1
        # Capture de l’image via HTTP et désérialisation
        r = requests.get("http://localhost:5001/capture_image")
        decodedjson = r.json()
        frame = pickle.loads(json.loads(decodedjson).encode('latin-1'))
        output = frame.copy()

        # Prétraitement de l’image (grayscale + flou)
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        gray = cv.GaussianBlur(gray, (7, 7), 0)

        rows = gray.shape[0]

        # Détection de cercles avec les paramètres mis à jour via l’interface
        if counter % work_freq == 0:
            counter = 0

            hough_param_1 = max(cv.getTrackbarPos('par1', 'frame'), 1)
            hough_param_2 = max(cv.getTrackbarPos('par2', 'frame'), 1)
            dmax = cv.getTrackbarPos('dmax', 'frame')
            min_rad = cv.getTrackbarPos('minr', 'frame')
            max_rad = cv.getTrackbarPos('maxr', 'frame')

            # Génération des contours avec Canny (pour affichage uniquement)
            edges = cv.Canny(gray, threshold1=hough_param_1,
                             threshold2=hough_param_1 / 2.,
                             apertureSize=3,
                             L2gradient=True)

            # Détection des cercles avec HoughCircles
            circles_det = cv.HoughCircles(gray,
                                          cv.HOUGH_GRADIENT,
                                          3,
                                          10,
                                          param1=hough_param_1,
                                          param2=hough_param_2,
                                          minRadius=min_rad,
                                          maxRadius=max_rad)

        # Affichage des cercles détectés
        if circles_det is not None:
            circles = np.uint16(np.around(circles_det))
            # Trier les cercles dans l’ordre de lecture (haut → bas, gauche → droite)
            sorted_circles = sorted(circles[0, :], key=lambda c: (c[1], c[0]))

            for idx, i in enumerate(sorted_circles):
                center = (i[0], i[1])
                radius = i[2]
                # Dessiner le centre
                cv.circle(output, center, 1, (0, 100, 100), 1)
                # Dessiner le cercle
                cv.circle(output, center, radius, (0, 255, 0), 2)
                # Écrire l’identifiant du cercle à côté du centre
                cv.putText(output,
                        f"{idx+1}",
                        (center[0] + 10, center[1] - 10),  # décalé pour lisibilité
                        cv.FONT_HERSHEY_SIMPLEX,
                        0.5,
                        (0, 0, 0),
                        1,
                        cv.LINE_AA)

        # Affichage des contours (optionnel, pour visualisation)
        if edges is not None:
            output[edges != 0] = (0, 0, 255)

        # Affichage des cercles min/max pour référence visuelle
        min_circle[0][2] = int(min_rad)
        max_circle[0][2] = int(max_rad)
        drawer(output, min_circle, (255, 0, 255), 1, False)
        drawer(output, max_circle, (255, 0, 255), 1, False)

        # Affichage du FPS en haut à gauche
        writer(output, "Fps={:06.2f}".format(fps()))
        cv.imshow('frame', output)

        # Quitter la boucle si touche "q" pressée
        if cv.waitKey(1) & 0xFF == ord('q'):
            break

    # Nettoyage
    cv.destroyAllWindows()
    r = requests.get("http://localhost:5001/stop")