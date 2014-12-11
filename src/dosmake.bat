@echo off
if %1.==clean. goto clean

rem This needs zmac and mkasmver. We assume they're in ..\dosutils.

..\dosutils\mkasmver <start.z >asmver.z
echo assembling system...
..\dosutils\zmac nc100.z
echo assembling boot prog...
..\dosutils\zmac boot.z
echo making system binary...
copy /b boot.bin + nc100.bin + 4x6font.dat ..\bin\zcn.bin >nul
goto done

rem used for `dosmake clean'.
:clean
echo y|del *.lst >nul
echo y|del *.bin >nul
echo y|del asmver.z >nul
goto done

:done
echo done.
