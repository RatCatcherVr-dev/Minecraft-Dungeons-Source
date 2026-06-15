IF EXIST output rmdir /Q /S output
mkdir output
copy PreOrderStub\MicrosoftGame.config Gaming.Desktop.x64\Debug
makepkg pack /f chunks.xml /d Gaming.Desktop.x64/Debug /pd output /pc