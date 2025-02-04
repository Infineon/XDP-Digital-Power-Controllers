if exist .venv (
	echo "python virtual environment .venv already setup skipping install"
	REM rmdir .venv /s
	REM python -m venv .venv
) else (
	echo "creating new python virtual environment .venv"
	python -m venv .venv
)

echo activating the python virtual environment .venv
CALL .venv/Scripts/activate
where python
echo installing requirements.txt python required libraries
pip install -r requirements.txt