cd ..
del usage.txt
palgen_persune.py --skip-plot -p -w -r png
palgen_persune.py --skip-plot -e -r png -t docs/demo_screenshots/smb.bin
palgen_persune.py -h >> usage.txt
echo 
pause
call "make palettes.bat"
cd docs
call "frames to gif.bat"