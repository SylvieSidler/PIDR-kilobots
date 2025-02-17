import cv2 # À installer après avoir créé un environnement avec la commande : pip install flask opencv-python
from flask import Flask,  Response, send_file

app = Flask(__name__)

camera = cv2.VideoCapture(0) # Connexion avec la caméra qui peut être celle du PC ou celle d'une caméra branchée

def gen_frames():
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
 
# Permet d'obtenir une vidéo de ce qui est filmé par la caméra
@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Permet d'obtenir une image de ce qui est filmé au moment ou la route est spécifiée
@app.route('/capture_image')
def capture_image():
    success, frame = camera.read()
    if success:
        image_path = "captured_image.jpg"
        cv2.imwrite(image_path, frame)
        return send_file(image_path, mimetype='image/jpeg')
    else:
        return "Failed to capture image", 500

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=5000)
