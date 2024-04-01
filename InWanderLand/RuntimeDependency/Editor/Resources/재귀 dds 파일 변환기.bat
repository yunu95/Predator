for /r %%f in (*.png) do (
    echo Converting %%f...
    texconv -f BC3_UNORM -o %%~dpf %%f
)
echo Conversion complete.
pause