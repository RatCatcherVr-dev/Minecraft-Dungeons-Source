if exist BuildTools (
	cd BuildTools
	call set_networktype 0
	cd ..
) else (
	call set_networktype 0
)
