import sys

HTTP_SERVER = {
    "base_url": "http://localhost:5001",
    "endpoints": {
        "start": "/start",
        "stop": "/stop",
        "set_resolution": "/set_resolution/mode3",
        "set_camera": "/set_camera/0",
        "capture_image": "/capture_image"
    }
}

IMAGE = {
    "width": 800,
    "height": 600
}

ENVIRONMENTS = {
    "maison": {
        "hough": {
            "param1": 38,
            "param2": 40,
            "min_radius": 38,
            "max_radius": 40,
            "distance_max": 10
        },
        "physical": {
            "circle_diameter_cm": 3.3,  
            "communication_radius_cm": 3.5
        }
    },
    "laboratoire": {
        "hough": {
            "param1": 32,
            "param2": 32,
            "min_radius": 9,
            "max_radius": 11,
            "distance_max": 10
        },
        "physical": {
            "circle_diameter_cm": 3.3,
            "communication_radius_cm": 3.5
        }
    },
    "autre": {
        "hough": {
            "param1": 40,
            "param2": 40,
            "min_radius": 60,
            "max_radius": 62,
            "distance_max": 10
        },
        "physical": {
            "circle_diameter_cm": 3,
            "communication_radius_cm": 3.5
        }
    }
}

IMAGE_PROCESSING = {
    "gaussian_blur_kernel": (7, 7),
    "work_freq": 1,
    "kernel": (3, 3)
}

DISPLAY = {
    "font": {
        "face": "FONT_HERSHEY_SIMPLEX",
        "scale": 1,
        "color": (0, 0, 255),
        "line_type": 2
    },
    "circle": {
        "default_color": (0, 255, 0),
        "default_thickness": 4,
        "center_color": (0, 0, 255),
        "center_thickness": 1
    },
    "grid": {
        "thin_line_color": (50, 50, 50),
        "thick_line_color": (100, 100, 100),
        "thin_line_thickness": 1,
        "thick_line_thickness": 2
    },
    "panel": {
        "width": 270,
        "opacity": 0.6
    }
}

# Fonction pour obtenir la configuration active basée sur l'environnement
def get_active_config(env_name=None):
    """
    Retourne la configuration active basée sur l'environnement spécifié.
    Si aucun environnement n'est spécifié, tente de le détecter ou utilise 'maison' par défaut.
    """
    # Liste des environnements disponibles
    available_envs = list(ENVIRONMENTS.keys())
    
    # Si aucun environnement n'est spécifié, chercher dans les arguments système
    if env_name is None:
        if len(sys.argv) > 1 and sys.argv[1] in available_envs:
            env_name = sys.argv[1]
        else:
            env_name = "maison"  # Environnement par défaut
    
    # Vérifier que l'environnement existe
    if env_name not in available_envs:
        print(f"Environnement '{env_name}' inconnu. Utilisation de 'maison' par défaut.")
        print(f"Environnements disponibles: {', '.join(available_envs)}")
        env_name = "maison"
    
    # Récupérer les configurations spécifiques à l'environnement
    env_config = ENVIRONMENTS[env_name]
    
    # Créer et retourner la configuration active
    active_config = {
        "environment": env_name,
        "http_server": HTTP_SERVER,
        "image": IMAGE,
        "hough": env_config["hough"],
        "physical": env_config["physical"],
        "image_processing": IMAGE_PROCESSING,
        "display": DISPLAY
    }
    
    return active_config

# Configuration active (utilisée par défaut)
ACTIVE_CONFIG = get_active_config()

# Alias pour un accès plus facile
HTTP_SERVER = ACTIVE_CONFIG["http_server"]
IMAGE = ACTIVE_CONFIG["image"]
HOUGH = ACTIVE_CONFIG["hough"]
PHYSICAL = ACTIVE_CONFIG["physical"]
IMAGE_PROCESSING = ACTIVE_CONFIG["image_processing"]
DISPLAY = ACTIVE_CONFIG["display"]