import sys, time, math
import requests
import pickle
import json
from collections import deque

import numpy as np
import cv2 as cv

# Importer la configuration
from config import *

# Classe pour mesurer les FPS sur les dernières frames (moyenne glissante)
class FPS(object):
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
        self._font = getattr(cv, DISPLAY["font"]["face"])
        self._fontScale = DISPLAY["font"]["scale"]
        self._fontColor = DISPLAY["font"]["color"]
        self._lineType = DISPLAY["font"]["line_type"]

    def __call__(self, img, text, pos=(40, 40)):
        cv.putText(img, text, pos, self._font, self._fontScale, self._fontColor, self._lineType)

# Classe pour dessiner un cercle détecté sur une image
class CircleDrawer(object):
    def __init__(self):
        self._center_thickness = DISPLAY["circle"]["center_thickness"]
        self._center_color = DISPLAY["circle"]["center_color"]

    def __call__(self, img, circles, color=DISPLAY["circle"]["default_color"], 
                 thickness=DISPLAY["circle"]["default_thickness"], bbox=False):
        circle = np.round(circles[0]).astype("int")
        x, y, r = circle

        cv.circle(output, (x, y), r, color, thickness)
        cv.drawMarker(output, (x, y), color, thickness=thickness)

        if bbox:
            cv.rectangle(output, (x - r, y - r), (x + r, y + r), self._center_color, self._center_thickness)

def get_led_color_name(hue):
    # violet : 140
    # bleu : 120
    if 0 <= hue <= 10 or 160 <= hue <= 180:
        return hue#"rouge"
    elif 10 < hue <= 30:
        return hue#"orange"
    elif 30 < hue <= 45:
        return hue#"jaune"
    elif 45 < hue <= 85:
        return hue#"vert"
    elif 85 < hue <= 130:
        return hue#"bleu"
    elif 130 < hue <= 160:
        return hue#"violet"
    else:
        return hue#"inconnu"

def detect_led_color_in_circle(bgr_image, center, radius, num_edge_samples=8, sample_radius=4):
    h, w = bgr_image.shape[:2]
    x_c, y_c = int(center[0]), int(center[1])
    hsv = cv.cvtColor(bgr_image, cv.COLOR_BGR2HSV)

    best_score = 0
    best_hue = None

    def evaluate_region(x, y):
        nonlocal best_score, best_hue
        if not (0 <= x < w and 0 <= y < h):
            return

        x0 = max(0, x - sample_radius)
        y0 = max(0, y - sample_radius)
        x1 = min(w, x + sample_radius)
        y1 = min(h, y + sample_radius)

        roi = hsv[y0:y1, x0:x1]
        h_vals = roi[:, :, 0].flatten()
        s_vals = roi[:, :, 1].flatten()
        v_vals = roi[:, :, 2].flatten()

        mask = (s_vals > 80) & (v_vals > 100)
        if np.sum(mask) == 0:
            return

        mean_s = np.mean(s_vals[mask])
        mean_v = np.mean(v_vals[mask])
        mean_h = np.mean(h_vals[mask])
        score = mean_s + mean_v

        if score > best_score:
            best_score = score
            best_hue = mean_h

    # 1. Tester le centre
    evaluate_region(x_c, y_c)

    # 2. Tester les bords du cercle
    angles = np.linspace(0, 2 * np.pi, num_edge_samples, endpoint=False)
    for angle in angles:
        x = int(x_c + radius * 0.6 * np.cos(angle))
        y = int(y_c + radius * 0.6 * np.sin(angle))
        evaluate_region(x, y)

    if best_hue is None:
        return "inconnu"

    return get_led_color_name(best_hue)



# Callback vide pour les trackbars OpenCV
def nothing(*arg):
    pass

if __name__ == "__main__":
    print(f"OpenCV version {cv.__version__}")
    print(f"Utilisation de la configuration pour l'environnement: {ACTIVE_CONFIG['environment']}")

    # Configuration de la capture vidéo via serveur HTTP local
    r = requests.get(f"{HTTP_SERVER['base_url']}{HTTP_SERVER['endpoints']['start']}")
    r = requests.get(f"{HTTP_SERVER['base_url']}{HTTP_SERVER['endpoints']['set_resolution']}")
    r = requests.get(f"{HTTP_SERVER['base_url']}{HTTP_SERVER['endpoints']['set_camera']}")

    width = IMAGE["width"]
    height = IMAGE["height"]

    # Paramètres de la transformée de Hough pour la détection de cercles
    diag = math.sqrt(width**2 + height**2)
    dist = int(.9 * diag)
    min_rad = HOUGH["min_radius"]
    max_rad = HOUGH["max_radius"]
    dmax = HOUGH["distance_max"]
    hough_param_1 = HOUGH["param1"]
    hough_param_2 = HOUGH["param2"]
    
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
    work_freq = IMAGE_PROCESSING["work_freq"]

    # Initialisation des outils : mesure de FPS, écriture texte, dessin de cercles
    fps = FPS()
    writer = TextWriter()
    drawer = CircleDrawer()
    kernel = np.ones(IMAGE_PROCESSING["kernel"], np.uint8)

    process_mode = "calib"

    while True:

        counter += 1
        # Capture de l'image via HTTP et désérialisation
        r = requests.get(f"{HTTP_SERVER['base_url']}{HTTP_SERVER['endpoints']['capture_image']}")
        decodedjson = r.json()
        frame = pickle.loads(json.loads(decodedjson).encode('latin-1'))
        output = frame.copy()

        # Prétraitement de l'image (grayscale + flou)
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        gray = cv.GaussianBlur(gray, IMAGE_PROCESSING["gaussian_blur_kernel"], 0)

        rows = gray.shape[0]

        # Détection de cercles avec les paramètres mis à jour via l'interface
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
                                          20,
                                          param1=hough_param_1,
                                          param2=hough_param_2,
                                          minRadius=min_rad,
                                          maxRadius=max_rad)

            if process_mode == "calib" :
                if circles_det is not None:
                    circles = np.uint16(np.around(circles_det))
                    sorted_circles = sorted(circles[0, :], key=lambda c: (c[1], c[0]))

                    # Calcul automatique de l'échelle à partir des diamètres
                    diameters_px = [2 * i[2] for i in sorted_circles]
                    mean_diameter_px = np.mean(diameters_px)
                    pixels_per_cm = mean_diameter_px / PHYSICAL["calib_diametre_cm"]
                    

            if process_mode == "detect" :

                min_rad = HOUGH["min_radius"]
                max_rad = HOUGH["max_radius"]
                dmax = HOUGH["distance_max"]
                hough_param_1 = HOUGH["param1"]
                hough_param_2 = HOUGH["param2"]

                # Affichage des cercles détectés
                if circles_det is not None:
                    circles = np.uint16(np.around(circles_det))
                    sorted_circles = sorted(circles[0, :], key=lambda c: (c[1], c[0]))

                    # Calcul automatique de l'échelle à partir des diamètres
                    diameters_px = [2 * i[2] for i in sorted_circles]
                    mean_diameter_px = np.mean(diameters_px)
                    pixels_per_cm = mean_diameter_px / PHYSICAL["circle_diameter_cm"]

                    # Rayon de communication converti en pixels
                    extra_radius_px = int(round(PHYSICAL["communication_radius_cm"] * pixels_per_cm))

                    centers_px = []
                    centers_cm = []
                    detected_colors = []


                    for idx, i in enumerate(sorted_circles):
                        x_px, y_px, r_px = i
                        centers_px.append((x_px, y_px))

                        # Conversion en cm avec origine en haut à gauche
                        x_cm = x_px / pixels_per_cm
                        y_cm = y_px / pixels_per_cm
                        centers_cm.append((x_cm, y_cm))

                        # Cercle d'origine
                        cv.circle(output, (x_px, y_px), 1, (0, 100, 100), 1)
                        cv.circle(output, (x_px, y_px), r_px, (0, 255, 0), 1)
                        cv.putText(output, f"{idx+1}", (x_px + 10, y_px - 10),
                                cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv.LINE_AA)

                        color_name = detect_led_color_in_circle(frame, (x_px, y_px), r_px)
                        detected_colors.append(color_name)
                        
                        # --- Détection de couleur LED ---
                        led_color = detect_led_color_in_circle(frame, (x_px, y_px), r_px)
                        cv.putText(output, f"{led_color}", (x_px + 10, y_px + 10),
                                cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv.LINE_AA)

                        # Cercle supplémentaire de communication (en bleu)
                        if AFFICHAGE["cercle_communication"] == True :
                            if led_color == "rouge":
                                cv.circle(output, (x_px, y_px), extra_radius_px, (0, 0, 255), 1)
                            else :
                                cv.circle(output, (x_px, y_px), extra_radius_px, (255, 0, 0), 1)

                    # Afficher l'échelle utilisée
                    cv.putText(output, f"1 cm = {pixels_per_cm:.2f} px", (20, output.shape[0] - 20),
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
                            if dist_cm <= PHYSICAL["communication_radius_cm"] + r2_cm or dist_cm <= PHYSICAL["communication_radius_cm"] + r1_cm:
                                # Dessiner une ligne rouge entre les centres
                                if AFFICHAGE["lien_communication"] == True :
                                    cv.line(output, centers_px[i], centers_px[j], (0, 0, 0), 2)
                    
                    if detected_colors and all(c == detected_colors[0] for c in detected_colors):
                        cv.putText(output, "Success: All LEDs are the same color!",
                                (50, 50), cv.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2, cv.LINE_AA)


        # Affichage des contours (optionnel, pour visualisation)
        if edges is not None:
            output[edges != 0] = (0, 0, 255)

        # Affichage des cercles min/max pour référence visuelle
        min_circle[0][2] = int(min_rad)
        max_circle[0][2] = int(max_rad)
        drawer(output, min_circle, (255, 0, 255), 1, False)
        drawer(output, max_circle, (255, 0, 255), 1, False)

        # === Quadrillage tous les 5 cm et 10 cm avec coordonnées ===
        if 'pixels_per_cm' in locals() and pixels_per_cm > 0:
            spacing_5cm = int(round(5 * pixels_per_cm))
            spacing_10cm = int(round(10 * pixels_per_cm))
            height, width = output.shape[:2]

            for x in range(0, width, spacing_5cm):
                thickness = DISPLAY["grid"]["thin_line_thickness"] if x % spacing_10cm != 0 else DISPLAY["grid"]["thick_line_thickness"]
                color = DISPLAY["grid"]["thin_line_color"] if thickness == 1 else DISPLAY["grid"]["thick_line_color"]
                cv.line(output, (x, 0), (x, height), color, thickness)

                if x % spacing_10cm == 0:
                    x_cm = x / pixels_per_cm
                    if x_cm == 0:
                        cv.putText(output, f"{int(x_cm)}", (x + 2, 15),
                                cv.FONT_HERSHEY_PLAIN, 1, (0, 0, 0), 1, cv.LINE_AA)
                    else:
                        cv.putText(output, f"{int(x_cm+1)}", (x + 2, 15),
                                cv.FONT_HERSHEY_PLAIN, 1, (0, 0, 0), 1, cv.LINE_AA)

            for y in range(0, height, spacing_5cm):
                thickness = DISPLAY["grid"]["thin_line_thickness"] if y % spacing_10cm != 0 else DISPLAY["grid"]["thick_line_thickness"]
                color = DISPLAY["grid"]["thin_line_color"] if thickness == 1 else DISPLAY["grid"]["thick_line_color"]
                cv.line(output, (0, y), (width, y), color, thickness)

                if y % spacing_10cm == 0:
                    y_cm = y / pixels_per_cm
                    cv.putText(output, f"{int(y_cm+1)}", (5, y - 2),
                            cv.FONT_HERSHEY_PLAIN, 1, (0, 0, 0), 1, cv.LINE_AA)
        
        # Affichage des coordonnées converties (panneau en bas à droite)
        if process_mode == "detect" :
            if 'centers_cm' in locals() and centers_cm:
                panel_width = DISPLAY["panel"]["width"]
                panel_height = min(200, 20 * len(centers_cm) + 10)
                x0 = output.shape[1] - panel_width - 20
                y0 = output.shape[0] - panel_height - 20

                overlay = output.copy()
                cv.rectangle(overlay, (x0, y0), (x0 + panel_width + 10, y0 + panel_height + 10), (0, 0, 0), -1)
                cv.addWeighted(overlay, DISPLAY["panel"]["opacity"], output, 1 - DISPLAY["panel"]["opacity"], 0, output)

                for idx, (x_cm, y_cm) in enumerate(centers_cm):
                    text = f"ID {idx+1}: ({x_cm:.1f} cm, {y_cm:.1f} cm)"
                    #cv.putText(output, text, (x0 + 10, y0 + 20 + idx * 20),
                    #        cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv.LINE_AA)

        # Affichage de l'image
        cv.imshow('frame', output)

        # Quitter la boucle si touche "q" pressée
        key = cv.waitKey(1)
        if 0xFF & key == ord('q'):
            break
        if 0xFF & key == ord('c'):
            process_mode = "calib"
        if 0xFF & key == ord('d'):
            process_mode = "detect"

    # Nettoyage
    cv.destroyAllWindows()
    r = requests.get(f"{HTTP_SERVER['base_url']}{HTTP_SERVER['endpoints']['stop']}")