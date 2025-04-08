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
            sorted_circles = sorted(circles[0, :], key=lambda c: (c[1], c[0]))

            # Calcul automatique de l’échelle à partir des diamètres
            diameters_px = [2 * i[2] for i in sorted_circles]
            mean_diameter_px = np.mean(diameters_px)
            pixels_per_cm = mean_diameter_px / 3.3  # ← basé sur ton diamètre réel en cm

            # Rayon de 7 cm converti en pixels
            extra_radius_px = int(round(7 * pixels_per_cm))

            centers_px = []
            centers_cm = []

            for idx, i in enumerate(sorted_circles):
                x_px, y_px, r_px = i
                centers_px.append((x_px, y_px))

                # Conversion en cm avec origine en haut à gauche
                x_cm = x_px / pixels_per_cm
                y_cm = y_px / pixels_per_cm
                centers_cm.append((x_cm, y_cm))

                # Cercle d'origine
                cv.circle(output, (x_px, y_px), 1, (0, 100, 100), 1)
                cv.circle(output, (x_px, y_px), r_px, (0, 255, 0), 2)
                cv.putText(output, f"{idx+1}", (x_px + 10, y_px - 10),
                        cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv.LINE_AA)

                # Cercle supplémentaire de 7 cm (en bleu)
                cv.circle(output, (x_px, y_px), extra_radius_px, (255, 0, 0), 2)

            # Afficher l’échelle utilisée
            cv.putText(output, f"1 cm = {pixels_per_cm:.2f} px", (20, output.shape[0] - 20),
                    cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv.LINE_AA)

            # Affichage des coordonnées converties
            panel_height = min(200, 30 * len(centers_cm) + 10)
            overlay = output.copy()
            cv.rectangle(overlay, (10, 10), (280, 10 + panel_height), (0, 0, 0), -1)
            cv.addWeighted(overlay, 0.6, output, 0.4, 0, output)

            for idx, (x_cm, y_cm) in enumerate(centers_cm):
                text = f"ID {idx+1}: ({x_cm:.1f} cm, {y_cm:.1f} cm)"
                cv.putText(output, text, (20, 35 + idx * 20),
                        cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv.LINE_AA)

            # Liaisons : créer des lignes rouges entre kilobits pouvant communiquer
            for i in range(len(centers_cm)):
                for j in range(i + 1, len(centers_cm)):
                    x1_cm, y1_cm = centers_cm[i]
                    x2_cm, y2_cm = centers_cm[j]

                    # Distance entre centres en cm
                    dist_cm = math.hypot(x2_cm - x1_cm, y2_cm - y1_cm)

                    # Rayon réel des cercles en cm (dérivé de leur rayon pixels)
                    r1_cm = sorted_circles[i][2] / pixels_per_cm
                    r2_cm = sorted_circles[j][2] / pixels_per_cm

                    # Si les zones de com se recoupent
                    if dist_cm <= 7 + r2_cm or dist_cm <= 7 + r1_cm:
                        # Dessiner une ligne rouge entre les centres
                        cv.line(output, centers_px[i], centers_px[j], (0, 0, 255), 2)



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