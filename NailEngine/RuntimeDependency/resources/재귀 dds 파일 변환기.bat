for /r %%f in (*.png) do (
    echo Converting %%f...
    texconv -f BC3_UNORM -m 1 -o %%~dpf %%f
)
echo Conversion complete.
pause