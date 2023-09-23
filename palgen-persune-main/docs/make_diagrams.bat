cd ..
palgen-persune.py --skip-plot -p -w -r png
palgen-persune.py --skip-plot -e -r png -t docs/demo_screenshots/smb.bin
palgen-persune.py -h
echo 
pause
call "make palettes.bat"
cd docs
call "frames to gif.bat"