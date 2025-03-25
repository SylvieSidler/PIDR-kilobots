if [ -e .venv ] || [ -e venv ]
then
    if [ -e .venv ] 
    then
        echo ".venv already setup"
        echo '.venv/'>>.gitignore
        source .venv/bin/activate
    else 
        echo "venv already setup"
        echo 'venv/'>>.gitignore
        source venv/bin/activate
    fi
else
    echo "Setting up .venv and installing requirements"
    python3 -m venv .venv
    pip install --upgrade pip
    pip install -r requirements.txt
fi
echo '.vscode/'>>.gitignore
echo '__pycache__/'>>.gitignore
echo '.DS_Store'>>.gitignore
echo "Setup complete"
