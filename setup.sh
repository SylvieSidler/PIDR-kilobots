if [ -e .venv ]
then
    echo ".venv already setup"
else
    echo "Setting up .venv and installing requirements"
    python3 -m venv .venv
    pip install --upgrade pip
    pip install -r requirements.txt
fi
echo '.venv/'>>.gitignore
echo '.vscode/'>>.gitignore
echo '__pycache__/'>>.gitignore
echo '.DS_Store'>>.gitignore
source .venv/bin/activate
echo "Setup complete"
