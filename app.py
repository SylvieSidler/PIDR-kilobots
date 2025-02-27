import cv2 # À installer après avoir créé un environnement avec la commande : pip install flask opencv-python
from flask import Flask,  Response, send_file

app = Flask(__name__)

camera = cv2.VideoCapture(0) # Connexion avec la caméra qui peut être celle du PC ou celle d'une caméra branchée (téléphone)

# Permet d'obtenir une image de ce qui est filmé au moment ou la route est spécifiée
@app.route('/capture_image')
def capture_image():
    success, frame = camera.read()
    if success:
        image_path = "captured_image1.jpg"
        cv2.imwrite(image_path, frame)
        return send_file(image_path, mimetype='image/jpeg')
    else:
        return "Failed to capture image", 500

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=5000)